//============================================================================
//  0x6F631350 - RecomputeClampRectFromBox.  CItem vtable slots 32/104
//  closure, batch K continuation (depth 12-25+).
//
//  Fills g_pumpClampRect (flt_6FACEBB0, Input/inputeventpump.h - the same
//  global inputcursorclamp.cpp's own ClampCursorToScreenBounds reads) from
//  a caller-supplied 4-float box: a null box zeroes all four fields, and a
//  real one runs each axis pair through the already-reconstructed
//  ScaleFromUiSpan (Math/uispan.cpp, UI units -> normalised-by-divide) and
//  then through this closure's own FACEDATA_ClampToClientRect
//  (Misc/citemclosure_facedata_clamprect.cpp, whose own header explains why
//  it is naked) before converting the two clamped-int results back to
//  float and storing them.
//
//  `__declspec(naked)` for the same reason as FACEDATA_ClampToClientRect
//  itself: it calls that function twice, and satisfying its own
//  eax/ebx-register-pointer parameters from ordinary C++ would need
//  exactly this same inline assembly anyway - there is no advantage to a
//  non-naked wrapper here, and a naked body is the only way to keep this
//  transcription literal rather than reshaped.  `box` arrives in `ecx`.
//============================================================================
#include "inputeventpump.h"
#include "uispan.h"

//  Declared here (not defined - see Misc/citemclosure_facedata_clamprect.cpp
//  for the naked body and why its two float parameters are stack-only)
//  purely so the __asm block below gets the right decorated `__stdcall`
//  symbol to `call` by name.
void __stdcall FACEDATA_ClampToClientRect(float xScale, float yScale);

__declspec(naked) void __fastcall RecomputeClampRectFromBox(const float* /*box*/)
{
    __asm {
        sub     esp, 0x20
        push    esi
        mov     esi, ecx
        test    esi, esi
        jnz     have_box

        fldz
        pop     esi
        fst     dword ptr [g_pumpClampRect]
        fst     dword ptr [g_pumpClampRect + 4]
        fst     dword ptr [g_pumpClampRect + 8]
        fstp    dword ptr [g_pumpClampRect + 12]
        add     esp, 0x20
        retn

have_box:
        fld     dword ptr [esi]
        push    ebx
        sub     esp, 8
        fstp    dword ptr [esp + 0x30 - 0x2C]
        lea     edx, [esp + 0x30 - 0x18]
        fld     dword ptr [esi + 4]
        lea     ecx, [esp + 0x30 - 0x1C]
        fstp    dword ptr [esp + 0x30 - 0x30]
        call    ScaleFromUiSpan

        fld     dword ptr [esi + 8]
        sub     esp, 8
        fstp    dword ptr [esp + 0x30 - 0x2C]
        lea     edx, [esp + 0x30 - 0x20]
        fld     dword ptr [esi + 0x0C]
        lea     ecx, [esp + 0x30 - 0x14]
        fstp    dword ptr [esp + 0x30 - 0x30]
        call    ScaleFromUiSpan

        fld     dword ptr [esp + 0x28 - 0x20]
        sub     esp, 8
        fstp    dword ptr [esp + 0x30 - 0x2C]
        lea     ebx, [esp + 0x30 - 0x10]
        fld     dword ptr [esp + 0x30 - 0x1C]
        lea     eax, [esp + 0x30 - 0x0C]
        fstp    dword ptr [esp + 0x30 - 0x30]
        call    FACEDATA_ClampToClientRect

        fld     dword ptr [esp + 0x28 - 0x18]
        sub     esp, 8
        fstp    dword ptr [esp + 0x30 - 0x2C]
        lea     ebx, [esp + 0x30 - 8]
        fld     dword ptr [esp + 0x30 - 0x14]
        lea     eax, [esp + 0x30 - 4]
        fstp    dword ptr [esp + 0x30 - 0x30]
        call    FACEDATA_ClampToClientRect

        fild    dword ptr [esp + 0x28 - 0x10]
        pop     ebx
        pop     esi
        fstp    dword ptr [g_pumpClampRect]
        fild    dword ptr [esp + 0x20 - 0x0C]
        fstp    dword ptr [g_pumpClampRect + 4]
        fild    dword ptr [esp + 0x20 - 8]
        fstp    dword ptr [g_pumpClampRect + 8]
        fild    dword ptr [esp + 0x20 - 4]
        fstp    dword ptr [g_pumpClampRect + 12]
        add     esp, 0x20
        retn
    }
}
