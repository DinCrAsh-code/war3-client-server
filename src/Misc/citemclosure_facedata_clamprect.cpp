//============================================================================
//  One more FACEDATA-adjacent helper - CItem vtable slots 32/104 closure,
//  batch K, depth 12-25+.  See Misc/citemclosure_facedata_metrics.cpp's own
//  header for how "FACEDATA" is ground truth rather than a guess.
//============================================================================
#include "storm.h"
#include "wininputbridge.h"

//  The CRT's own float-to-int truncation helper - a plain `(int)floatVal`
//  cast in ordinary C++ compiles to `call __ftol2_sse` (see
//  docs/msvc-vc8-idioms.md's "A float-to-unsigned truncation is inline; a
//  float-to-int one is a call"). `link_check.py`'s own CRT allowlist is why
//  this is the raw symbol and not jassstormterraingates.cpp's own
//  CrtFloatToIntHelperGate: that naked body's C++ linkage mangles to a
//  different symbol nothing auto-generated code (or this one) ever actually
//  calls - every real `(int)floatVal` truncation, including this naked
//  one's, resolves straight to the CRT's own `__ftol2_sse` in libcmt.lib.
//  `extern "C"` still applies this compiler's one leading underscore of
//  `__cdecl` decoration on top of the name's own two, producing
//  `___ftol2_sse` - a symbol nothing on the real link line defines
//  (`link_check.py` catches exactly that) - so the linker is told to
//  redirect that decorated reference back to the real, undecorated symbol
//  every other truncation's own auto-generated call already resolves
//  through.
extern "C" void __ftol2_sse();
#pragma comment(linker, "/alternatename:___ftol2_sse=__ftol2_sse")

//----------------------------------------------------------------------------
//  0x6F630BA0 - clamp two output values against the cached client rect,
//  scaled by a per-axis multiplier: fetch {left,top,right,bottom} through
//  GetCachedClientRect, round `(right-left)*arg_0` to an int and clamp it
//  into `[left, right-1]` at `*outX` (register-passed - see below), then
//  the same for `(bottom-top)*arg_4` clamped into `[top, bottom-1]` at
//  `*outY`.
//
//  `__declspec(naked)`: `outX` arrives in `eax` and `outY` in `ebx`, with
//  no `mov eax/ebx, ecx/edx` anywhere in the shipped body - this is another
//  whole-program-optimised internal helper (both pointers are already live
//  in its one caller in this closure before the call).  The two multiplier
//  arguments are real stack parameters (`retn 8`), unlike the pointers.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FACEDATA_ClampToClientRect(float /*xScale*/,
                                                              float /*yScale*/)
{
    __asm {
        sub     esp, 0x14
        push    ebp
        push    esi
        push    edi
        lea     ecx, [esp + 0x20 - 0x10]
        mov     edi, eax
        call    GetCachedClientRect
        mov     ebp, [esp + 0x20 - 8]
        mov     esi, [esp + 0x20 - 0x10]
        mov     eax, ebp
        sub     eax, esi
        mov     [esp + 0x20 - 0x14], eax
        fild    dword ptr [esp + 0x20 - 0x14]
        fmul    dword ptr [esp + 0x20 + 0]
        call    __ftol2_sse
        add     eax, esi
        cmp     eax, esi
        mov     [edi], eax
        jge     skip_lo_x
        mov     [edi], esi
skip_lo_x:
        cmp     [edi], ebp
        jl      skip_hi_x
        add     ebp, -1
        mov     [edi], ebp
skip_hi_x:
        mov     esi, [esp + 0x20 - 4]
        mov     edi, [esp + 0x20 - 0x0C]
        mov     ecx, esi
        sub     ecx, edi
        mov     [esp + 0x20 + 0], ecx
        fild    dword ptr [esp + 0x20 + 0]
        fmul    dword ptr [esp + 0x20 + 4]
        call    __ftol2_sse
        add     eax, edi
        cmp     eax, edi
        mov     [ebx], eax
        jge     skip_lo_y
        mov     [ebx], edi
skip_lo_y:
        cmp     [ebx], esi
        jl      skip_hi_y
        lea     edx, [esi - 1]
        mov     [ebx], edx
skip_hi_y:
        sub     esi, [ebx]
        sub     esi, edi
        pop     edi
        mov     [ebx], esi
        pop     esi
        pop     ebp
        add     esp, 0x14
        retn    8
    }
}
