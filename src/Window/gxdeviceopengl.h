//============================================================================
//  CGxDeviceOpenGl - the OpenGL backend's own device object, as reached from
//  the game's real Win32 window procedure (0x6F52A6F0).  See
//  docs/targets/WndProc.md and docs/notes/pause-mouse-capture.md for how
//  this address was found: user-confirmed live (user_knowledge.json),
//  traced from CGxDeviceOpenGl's own vtable slot +0xC (0x6F52AD00) up
//  through its Init/RegisterClass/CreateWindow chain.
//
//  Field layout is exact only where a function in this file's own cluster
//  touches it; everything else is `char m_reservedNN[...]` per CLAUDE.md.
//  Two independent sources agree on the shape: this cluster's own reads and
//  writes, and CGxDeviceOpenGl::CGxDeviceOpenGl's zero-init sweep
//  (0x6F52AFA0, asm/sub_6F52B1C0_0x6F52B1C0_calltree_asm.md - not itself
//  reconstructed here, out of this session's scope, but its field list is
//  what pins several of the offsets below with a second, independent read).
//============================================================================
#ifndef GXDEVICEOPENGL_H
#define GXDEVICEOPENGL_H

#include <windows.h>

//----------------------------------------------------------------------------
//  The six-dword "requested display mode" record CGxDeviceOpenGl::
//  ApplyDisplayMode (0x6F52A350) is handed a pointer to, and whose fields it
//  forwards individually into EnumDisplayDevicesA/EnumDisplaySettingsA's own
//  iDevNum/iModeNum and into SetWindowPos's cx/cy.  The trailing two dwords
//  are saved back into the global mirror below but never re-read by
//  anything in this cluster, so they stay unnamed past their offset.
//----------------------------------------------------------------------------
struct SDisplayModeRequest
{
    unsigned int m_deviceIndex;    // +0x00 - EnumDisplayDevicesA's iDevNum
    unsigned int m_modeIndex;      // +0x04 - EnumDisplaySettingsA's iModeNum
    unsigned int m_width;          // +0x08 - SetWindowPos's cx
    unsigned int m_height;         // +0x0C - SetWindowPos's cy
    unsigned int m_reserved10;     // +0x10 - saved, never re-read here
    unsigned int m_reserved14;     // +0x14 - saved, never re-read here
};

//  dword_6FACBD5C - the last-applied SDisplayModeRequest, compared against
//  a fresh request with an inlined memcmp (ApplyDisplayMode's own
//  `sub eax,4`/`jnb` unrolled-dword loop) and overwritten field by field
//  once a real mode change goes through.  DATA row: dword_6FACBD5C only -
//  the other five dwords in this record have no individual IDA name of
//  their own in any dump this session pulled, so they are not separately
//  entered in funcmap.py.
extern SDisplayModeRequest g_currentDisplayMode;   // dword_6FACBD5C

//  dword_6FACBD58 - the re-entrancy guard Init (0x6F52AD00, not
//  reconstructed here) and its own teardown counterpart set around every
//  mode change; WndProc's WM_SETFOCUS/WM_KILLFOCUS cases both refuse to
//  touch the device while this is set.
extern int g_gxDeviceOpenGlBusy;                   // dword_6FACBD58

//  dword_6FACBD74 - the raw-input bridge function pointer this whole
//  investigation is about.  Installed by CGxDeviceOpenGl's own vtable slot
//  +0xC (0x6F52AD00, not reconstructed here - see the header comment
//  above); WndProc calls through it for every message its own switch does
//  not special-case.  Piece 2 of this session's task: the address really
//  stored here, traced statically through sub_6F52AD00 ->
//  sub_6F526D10 -> sub_6F009580 (a literal `mov edx, offset sub_6F6C6AA0`
//  ahead of the call, unconditional - the same install happens whichever
//  GX backend gets created), is CGxRawInputBridge (wininputbridge.cpp).
typedef int (__fastcall *GxRawInputBridgeFn)(HWND hWnd, UINT wParam,
                                             LPARAM lParam);
extern GxRawInputBridgeFn g_pGxRawInputBridge;     // dword_6FACBD74

//----------------------------------------------------------------------------
//  The device object itself.  Only the members this cluster (WndProc and
//  its four real callees) touches are named; everything else is
//  exact-sized `char[]` padding per CLAUDE.md.  Never `sizeof()`'d - always
//  reached through a pointer this cluster is handed (GetWindowLongA's own
//  GWL_USERDATA slot) - so the struct does not need to run all the way out
//  to CGxDeviceOpenGl's real size.
//----------------------------------------------------------------------------
struct CGxDeviceOpenGl
{
    void*  m_vftable;                  // +0x000
    char   m_reserved004[0x94 - 0x004];// CGxDevice's own base fields, plus
                                       // whatever CGxDeviceOpenGl adds ahead
                                       // of the block below - none of it is
                                       // read by this cluster

    //  +0x094 - the last display mode actually applied (ApplyDisplayMode's
    //  own inlined-memcmp comparison target and per-field write-back).
    //  +0x09C (m_width) doing double duty as "has any mode ever been
    //  requested" is not a struct-layout mistake - see
    //  CGxDeviceOpenGl::IsDisplayModeUnset's own comment below.
    SDisplayModeRequest m_lastRequestedMode;   // +0x094..+0x0AB

    char   m_reservedAC[0xD8 - 0xAC];  // CGxDeviceOpenGl::CGxDeviceOpenGl's
                                       // own zero-init sweep touches several
                                       // dwords in here (gamma clamp count,
                                       // three flag words); none is read by
                                       // this cluster, so none is named
    float  m_field0D8;                 // +0x0D8 - read only by
                                       // CGxDeviceOpenGl_ReadField0xD8
                                       // (0x6F5271E0), whose one caller
                                       // forwards it as the float argument
                                       // of an unreconstructed vtable slot 7
                                       // call ("present"-shaped: this, a
                                       // float, then the int 1) - purpose
                                       // past that is not determined by
                                       // this call tree
    char   m_reservedDC[0x578 - 0xDC];
    HWND   m_hWnd;                     // +0x578
    HDC    m_hDC;                      // +0x57C
    HGLRC  m_hGLRC;                    // +0x580
    char   m_gammaRamp[0x600];         // +0x584 - a WORD[3][256] ramp
                                       // (SetDeviceGammaRamp/
                                       // GetDeviceGammaRamp's own lpRamp);
                                       // 0x584+0x600 = 0xB84, exactly where
                                       // CGxDeviceOpenGl::CGxDeviceOpenGl's
                                       // own zero-init sweep picks back up -
                                       // independent confirmation of the
                                       // 0x600 size
    char   m_reservedB84[0xBE4 - 0xB84];
    float  m_viewportX;                // +0xBE4 - WM_SIZE always writes 0.0f
    float  m_viewportY;                // +0xBE8 - WM_SIZE always writes 0.0f
    float  m_viewportHeight;           // +0xBEC - WM_SIZE's HIWORD(lParam)
    float  m_viewportWidth;            // +0xBF0 - WM_SIZE's LOWORD(lParam)

    //  0x6F52A350 - apply (or, given a null request, revert) a fullscreen
    //  display mode.  A real member (not a free `__fastcall` function
    //  taking `device` explicitly): the dump's own `retn 4` is `this` in
    //  ecx plus exactly one stack argument, which is what an ordinary
    //  `__thiscall` member with one pointer parameter compiles to - a
    //  free function declared `__fastcall(device, request)` instead
    //  pushes zero stack bytes where the shipped callee cleans four,
    //  the exact class of live crash tools/abi_audit.py's FATAL bucket
    //  exists to catch (docs/notes/hook-abi-crash-classes.md).
    //  `request` NULL means "go back to the desktop mode"; non-null and
    //  unchanged from g_currentDisplayMode is a no-op; non-null and
    //  different re-applies through EnumDisplayDevicesA/
    //  EnumDisplaySettingsA/ChangeDisplaySettingsExA and repositions the
    //  window.
    void ApplyDisplayMode(const SDisplayModeRequest* request);
};

//  0x6F527250 - IDA folds this onto Concurrency::details::
//  UMSThreadVirtualProcessor::CanCriticalEnter (identical-code-folding, not
//  a real relation - docs/msvc-vc8-idioms.md, "IDA can fold a game function
//  onto a CRT symbol").  Its one real caller here is WndProc's WM_SETFOCUS
//  case, on a `CGxDeviceOpenGl*`, reading offset +0x9C - which is
//  `m_lastRequestedMode.m_width`, the third dword of the very struct above.
//  Read as "has a display mode ever actually been requested" (width still
//  zero means never): WM_SETFOCUS only tries to reacquire the GL context
//  when this is true, i.e. when a real mode has been requested before.
bool __fastcall CGxDeviceOpenGl_IsDisplayModeUnset(CGxDeviceOpenGl* device);

//  0x6F5271E0 - `fld [ecx+0D8h]; retn` verbatim; see m_field0D8's own
//  comment above for what little is known about the value.
float __fastcall CGxDeviceOpenGl_ReadField0xD8(CGxDeviceOpenGl* device);

//  0x6F52A350 - CGxDeviceOpenGl::ApplyDisplayMode is declared on the
//  struct itself, above (a real `this`-in-ecx-plus-one-stack-argument
//  member, not a free `__fastcall` function - see its own comment there
//  for why).

//  0x6F531D90 - deep OpenGL resource teardown (walks a per-something
//  texture array, glDeleteTextures + a further callee per entry).  Not
//  reconstructed - out of this session's scope, generic GL plumbing
//  unrelated to input routing (gxdeviceopenglctxthunks.cpp).  `this` in
//  ecx (called with ecx left over from its caller, never reloaded - the
//  same "callee reads ecx the caller left there" idiom
//  docs/msvc-vc8-idioms.md already documents), `retn 0`.
void __fastcall CGxDeviceOpenGl_DestroyTextures(CGxDeviceOpenGl* device);

//  0x6F531580 - `retn` and nothing else; a real, if empty, reconstruction
//  rather than a thunk.
void CGxDeviceOpenGl_NoOpStub1537();

//  0x6F52A550 - tear down the current GL context (if any) and restore the
//  window's gamma ramp/release its DC (if any).  Called from WM_DESTROY
//  and from the WM_KILLFOCUS "give up the context" path.
void __fastcall CGxDeviceOpenGl_ReleaseGLContext(CGxDeviceOpenGl* device);

//  0x6F5314F0 - choose a pixel format and wglCreateContext it.  Deep
//  OpenGL plumbing, thunked (gxdeviceopenglctxthunks.cpp).  __fastcall,
//  `hWnd` in ecx, `retn 0`.
HGLRC __fastcall CGxDeviceOpenGl_CreateGLContext(HWND hWnd);

//  0x6F531600 - parse `GL_VERSION`/extension strings into module globals.
//  No real parameter (ecx is whatever the caller's own register allocator
//  happened to leave there, never read).  Thunked, same reason as above.
void CGxDeviceOpenGl_ParseGLExtensions();

//  0x6F5328A0 - GL state-block init off several byte fields near the start
//  of the device object.  Thunked, same reason.  __fastcall, `this` in
//  ecx, `retn 0`.
void __fastcall CGxDeviceOpenGl_InitStateBlock(CGxDeviceOpenGl* device);

//  0x6F52A690 - acquire (or fail to) a fresh GL context for the window:
//  GetDC, then CGxDeviceOpenGl_CreateGLContext, then wglMakeCurrent plus
//  the two further device-internal setup calls above.  Returns nonzero on
//  success, releasing everything through CGxDeviceOpenGl_ReleaseGLContext
//  on any failure.  Called from WndProc's WM_SETFOCUS reacquire path.
int __fastcall CGxDeviceOpenGl_AcquireGLContext(CGxDeviceOpenGl* device);

//  0x6F52A6F0 - the game's real WndProc, registered under the literal
//  "Warcraft III" window class name (0x6F52ABE0, not reconstructed here -
//  outside WndProc's own call tree, out of this session's scope).  Every
//  mouse and keyboard message that isn't one of the system messages below
//  falls through to g_pGxRawInputBridge.
LRESULT CALLBACK CGxDeviceOpenGl_WndProc(HWND hWnd, UINT Msg, WPARAM wParam,
                                         LPARAM lParam);

#endif
