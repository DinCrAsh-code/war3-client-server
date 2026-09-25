//============================================================================
//  0x6F52A6F0 - CGxDeviceOpenGl_WndProc, the game's real WndProc.
//
//  Registered under the literal "Warcraft III" window class name by
//  0x6F52ABE0 (outside this function's own call tree - not reconstructed
//  this session).  User-confirmed live (user_knowledge.json, "0x6F52A6F0").
//  Three other RegisterClass window procs exist in the binary
//  (0x6F6BB100/0x6F52C4F0/0x6F52CD00, docs/notes/pause-mouse-capture.md);
//  all three default every mouse/keyboard message straight to
//  DefWindowProcA and are dead ends for input routing, so they are not
//  reconstructed here either.
//
//  Special-cases WM_CREATE/WM_DESTROY/WM_SIZE/WM_SETFOCUS/WM_KILLFOCUS/
//  WM_PAINT/WM_ERASEBKGND/WM_SYSCOMMAND; everything else - every mouse and
//  keyboard message included - falls through to one generic forward: if
//  the device is attached and g_pGxRawInputBridge is set, call it with
//  (hWnd, wParam, lParam) and swallow the message; otherwise
//  DefWindowProcA.  That forward is Piece 2 of this session's task - see
//  gxdeviceopengl.h's own comment on g_pGxRawInputBridge for how the real
//  address (0x6F6C6AA0, wininputbridge.cpp) was traced.
//
//  The switch's own dense range is msg 1..15 (a real jump table, compacted
//  through a byte table at 0x6F52A8F8 - docs/msvc-vc8-idioms.md, "A
//  jump-table switch's own data table shows up on one side only"); msg
//  0x14 (WM_ERASEBKGND) and 0x112 (WM_SYSCOMMAND) are handled by two
//  individual compares outside that range, both landing in the same
//  `switch (Msg)` below - not written as separate `if`s, because a single
//  sparse switch over exactly this case set is what actually produces the
//  shipped shape (dense jump table for the low cluster, individual
//  compares for the two outliers).
//
//  This TU also carries the WndProc's own direct callees that live in the
//  same 0x6F52Axxx neighbourhood - CGxDeviceOpenGl_ReleaseGLContext
//  (0x6F52A550), CGxDeviceOpenGl_ApplyDisplayMode (0x6F52A350),
//  CGxDeviceOpenGl_AcquireGLContext (0x6F52A690), and the trivial
//  CGxDeviceOpenGl_ReadField0xD8 (0x6F5271E0) - one original module's
//  worth of Win32/GL window plumbing.
//
//  What is NOT reproduced: the /GS stack-buffer cookie every one of these
//  frames carries (`mov eax, dword_6FAAE140` / `xor eax, esp` / a slot
//  store on entry, the mirrored xor/call sub_6F7E1059 on every exit).  This
//  repo's fixed /GS- never emits it; same accepted gap as
//  GameUI/chatfontvalue.cpp and Net/netproviderbnet_seh.cpp document for
//  their own buffer-carrying frames.  IDENTICAL, not DIFFERS.
//============================================================================
#include "gxdeviceopengl.h"

extern "C" int memcmp(const void*, const void*, unsigned int);
extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

//  dword_6FACBD5C/58/74 are declared `extern` only, in gxdeviceopengl.h -
//  never defined here.  Defining a funcmap.DATA global instead of
//  declaring it is its own live-crash class docs/notes/mix-config-78-
//  crashes.md and CLAUDE.md both name (`CObserver::Dispatch`/
//  `EnsureResource`): build_mix.py binds a DATA row to the real address
//  only for a symbol that comes back EXTRN, so a local definition here
//  would give the hooked build a private, always-zero copy of
//  g_pGxRawInputBridge instead of the pointer the real, unhooked
//  0x6F52AD00 Init path actually installs - every message would look
//  like the bridge was never set.  tools/link_check.py is what catches
//  this; see its own "DATA BINDING" bucket.

//  0x6F531560 - wglMakeCurrent(NULL,NULL) then wglDeleteContext(hglrc).
//  __fastcall: the context in ecx, nothing on the stack (`retn`).  Small
//  and self-contained enough to reconstruct alongside its one caller
//  rather than thunk (contrast with the four deeper OpenGL-context leaves
//  in gxdeviceopenglctxthunks.cpp, which are not) - but an ordinary C++
//  body (`wglMakeCurrent(0,0); wglDeleteContext(hglrc);`) has this
//  toolchain tail-call the second, unused-return-value call
//  (`jmp DWORD PTR [wglDeleteContext]`) instead of the shipped
//  `push esi ; call` - so it is transcribed `__declspec(naked)` instead,
//  the same technique this repo already uses for `SecurityCheckCookie`.
__declspec(naked) void __fastcall ReleaseGLContextHandle(HGLRC hglrc)
{
    __asm
    {
        push    esi
        push    0
        push    0
        mov     esi, ecx
        call    DWORD PTR [wglMakeCurrent]
        push    esi
        call    DWORD PTR [wglDeleteContext]
        pop     esi
        retn
    }
}

//  0x6F5271E0 - `fld [ecx+0D8h]; retn`.  See m_field0D8's own comment in
//  gxdeviceopengl.h.
float __fastcall CGxDeviceOpenGl_ReadField0xD8(CGxDeviceOpenGl* device)
{
    return device->m_field0D8;
}

//  0x6F527250 - see gxdeviceopengl.h's own comment on this function.
bool __fastcall CGxDeviceOpenGl_IsDisplayModeUnset(CGxDeviceOpenGl* device)
{
    return device->m_lastRequestedMode.m_width == 0;
}

//  0x6F531580 - `retn` and nothing else.  `__declspec(naked)` rather than
//  a plain empty body: an ordinary empty function gets optimised away
//  entirely at its one call site (ReleaseGLContext's own `call SYM` to it
//  simply vanishes at /O2), losing a real instruction the shipped stream
//  still has.
__declspec(naked) void CGxDeviceOpenGl_NoOpStub1537()
{
    __asm { retn }
}

//  0x6F52A550 - release the current GL context (if any), then restore the
//  window's gamma ramp and release its DC (if any).
void __fastcall CGxDeviceOpenGl_ReleaseGLContext(CGxDeviceOpenGl* device)
{
    if (device->m_hGLRC != 0)
    {
        CGxDeviceOpenGl_DestroyTextures(device);
        CGxDeviceOpenGl_NoOpStub1537();
        ReleaseGLContextHandle(device->m_hGLRC);
        device->m_hGLRC = 0;
    }

    if (device->m_hDC != 0)
    {
        SetDeviceGammaRamp(device->m_hDC, device->m_gammaRamp);
        ReleaseDC(device->m_hWnd, device->m_hDC);
        device->m_hDC = 0;
    }
}

//  0x6F52A350 - apply, or (given a null request) revert, a fullscreen
//  display mode.
//
//    request == NULL:
//      already reverted (g_currentDisplayMode.m_modeIndex == 0) -> no-op;
//      otherwise ChangeDisplaySettingsExA back to the desktop mode, zero
//      the saved record, ShowWindow(SW_MINIMIZE).
//    request != NULL, unchanged from g_currentDisplayMode (inlined
//      memcmp over the whole 0x18-byte record) -> no-op.
//    request != NULL, different -> EnumDisplayDevicesA/
//      EnumDisplaySettingsA/ChangeDisplaySettingsExA, save the new record,
//      SetWindowPos to the new size, ShowWindow(SW_MAXIMIZE).
void CGxDeviceOpenGl::ApplyDisplayMode(const SDisplayModeRequest* request)
{
    if (request != 0)
    {
        if (memcmp(request, &g_currentDisplayMode,
                   sizeof(g_currentDisplayMode)) == 0)
            return;

        DISPLAY_DEVICEA displayDevice;
        memset(&displayDevice, 0, sizeof(displayDevice));
        displayDevice.cb = sizeof(displayDevice);

        DEVMODEA devMode;
        memset(&devMode, 0, sizeof(devMode));
        devMode.dmSize = sizeof(devMode);

        EnumDisplayDevicesA(0, request->m_deviceIndex, &displayDevice, 0);
        EnumDisplaySettingsA(displayDevice.DeviceName, request->m_modeIndex,
                            &devMode);
        ChangeDisplaySettingsExA(displayDevice.DeviceName, &devMode, 0,
                                 CDS_FULLSCREEN, 0);

        g_currentDisplayMode = *request;

        //  0x2108 = SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_DEFERERASE - the
        //  literal flags word the dump pushes; not decomposed into the
        //  usual named OR-of-flags because SWP_NOZORDER/SWP_NOACTIVATE are
        //  NOT part of it despite the window not moving in Z-order either.
        SetWindowPos(m_hWnd, 0, 0, 0, request->m_width, request->m_height,
                    0x2108);
        ShowWindow(m_hWnd, SW_MAXIMIZE);
    }
    else
    {
        if (g_currentDisplayMode.m_width == 0)
            return;

        ChangeDisplaySettingsExA(0, 0, 0, 0, 0);
        g_currentDisplayMode.m_deviceIndex = 0;
        g_currentDisplayMode.m_modeIndex   = 0;
        g_currentDisplayMode.m_width       = 0;
        g_currentDisplayMode.m_height      = 0;
        g_currentDisplayMode.m_reserved10  = 0;
        g_currentDisplayMode.m_reserved14  = 0;

        ShowWindow(m_hWnd, SW_MINIMIZE);
    }
}

//  0x6F52A690 - GetDC, create a GL context for it (0x6F5314F0, thunked -
//  deep pixel-format plumbing outside this session's scope), make it
//  current and run two further device-internal setup calls (0x6F531600 -
//  GL_EXTENSION string parsing; 0x6F5328A0 - state block init; both
//  thunked for the same reason).  Returns nonzero on success, having
//  released the DC and fallen back through CGxDeviceOpenGl_ReleaseGLContext
//  on any failure.
int __fastcall CGxDeviceOpenGl_AcquireGLContext(CGxDeviceOpenGl* device)
{
    device->m_hDC = GetDC(device->m_hWnd);
    if (device->m_hDC == 0)
    {
        CGxDeviceOpenGl_ReleaseGLContext(device);
        return 0;
    }

    device->m_hGLRC = CGxDeviceOpenGl_CreateGLContext(device->m_hWnd);
    if (device->m_hGLRC == 0)
    {
        CGxDeviceOpenGl_ReleaseGLContext(device);
        return 0;
    }

    wglMakeCurrent(device->m_hDC, device->m_hGLRC);
    CGxDeviceOpenGl_ParseGLExtensions();
    CGxDeviceOpenGl_InitStateBlock(device);
    return 1;
}

//  0x6F52A6F0 - see this file's own header comment.
LRESULT CALLBACK CGxDeviceOpenGl_WndProc(HWND hWnd, UINT Msg, WPARAM wParam,
                                         LPARAM lParam)
{
    CGxDeviceOpenGl* device =
        (CGxDeviceOpenGl*)GetWindowLongA(hWnd, GWL_USERDATA);

    switch (Msg)
    {
    case WM_CREATE:
        SetWindowLongA(hWnd, GWL_USERDATA,
                      (LONG)((LPCREATESTRUCTA)lParam)->lpCreateParams);
        break;

    case WM_DESTROY:
        CGxDeviceOpenGl_ReleaseGLContext(device);
        device->ApplyDisplayMode(0);
        ShowWindow(hWnd, SW_HIDE);
        break;

    case WM_SIZE:
        device->m_viewportX = 0.0f;
        device->m_viewportY = 0.0f;
        device->m_viewportHeight = (float)HIWORD(lParam);
        device->m_viewportWidth  = (float)LOWORD(lParam);
        break;

    case WM_SETFOCUS:
        if (g_gxDeviceOpenGlBusy)
            break;
        if (CGxDeviceOpenGl_IsDisplayModeUnset(device))
            break;
        if (device->m_hGLRC != 0)
            break;
        device->ApplyDisplayMode(&device->m_lastRequestedMode);
        CGxDeviceOpenGl_AcquireGLContext(device);
        {
            //  vtable slot 7 (+0x1C) - unreconstructed ("present"-shaped:
            //  this, a float, then the int 1).  No committed vtable for
            //  CGxDeviceOpenGl, so tools/vtable_dispatch_audit.py's own
            //  judged set correctly skips this site as unknown rather than
            //  guessing at it.
            typedef void (__thiscall *GxPresentFn)(CGxDeviceOpenGl*, float,
                                                   int);
            GxPresentFn present = (GxPresentFn)(*(void***)device)[7];
            present(device, CGxDeviceOpenGl_ReadField0xD8(device), 1);
        }
        break;

    case WM_KILLFOCUS:
        if (g_gxDeviceOpenGlBusy)
            break;
        if (CGxDeviceOpenGl_IsDisplayModeUnset(device))
            break;
        CGxDeviceOpenGl_ReleaseGLContext(device);
        device->ApplyDisplayMode(0);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_ERASEBKGND:
        return 0;

    case WM_SYSCOMMAND:
        if (wParam == SC_SCREENSAVE)
            return 0;
        if (wParam == SC_MONITORPOWER)
            return 0;
        //  FALLTHROUGH - every other system command reaches the same
        //  generic forward as any other unhandled message.

    default:
        if (device != 0 && g_pGxRawInputBridge != 0)
            return g_pGxRawInputBridge(hWnd, (UINT)wParam, lParam);
        return DefWindowProcA(hWnd, Msg, wParam, lParam);
    }

    return 0;
}
