//============================================================================
//  0x6F291920 - reached from CUnit's slot107 closure (per-template
//  initialiser, 0x6F2A0E30).  Populate the +0x2D8 anim-prop set from the
//  given data source, conditionally replay the unit's own idle/attack
//  animation pair (comparing the first PlayAnimation's returned float
//  against a fixed threshold to choose PlayAnimation vs.
//  PlayAnimationLooping for the second call - CUnit's own slots 35/36,
//  unit_playanim.cpp), conditionally refresh the uber-splat attachment,
//  then refresh the model-color field (+0x2D4, m_int2D4) from the
//  footprint registry and rebuild the animation index table.
//
//  Written as a naked transcription rather than a normal C++ call
//  sequence because two of the callees need a register set up by hand
//  that an ordinary C++ call through their existing declared signatures
//  would not reproduce: RefreshUberSplatAttachment is declared upstream
//  (Misc/citemclosure_batchL_thunks.cpp) as a plain `__stdcall(void)`
//  STORM_THUNK with no `this` modelled at all, and GetFootprintModelColor
//  (Pathfinding/footprintshadowoffset.cpp) returns its one-int struct
//  through a hidden pointer in ecx with the real `id` argument shifted
//  into edx - a shape C++ can express for a *fresh* call but not for
//  reusing the two virtual-call return values and the local scratch slot
//  the way this body's own register traffic does.  Per
//  docs/msvc-vc8-idioms.md, a naked body is not eligible for a register-
//  allocation excuse - it should score 1.000 by construction - so this is
//  a direct instruction-for-instruction copy, not a paraphrase.
//============================================================================
#include "unit.h"
#include "footprintpair.h"

//  0x6F28E170 - already declared upstream (Misc/citemclosure_batchL_thunks.cpp)
//  as a plain `__stdcall(void)` STORM_THUNK; redeclared here (same address,
//  same signature) since that .cpp has no header of its own to include.
void __stdcall RefreshUberSplatAttachment(void);

extern float g_animReplayThreshold;   // flt_6F8750B4

#pragma warning(disable : 4035)   // naked: no return value, leaves via the
                                  // transcribed asm's own retn

__declspec(naked) void __thiscall CUnit::PopulateAnimPropsAndModelColor(unsigned int typeId,
                                                            int replayAnim,
                                                            int refreshSplat)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp + 0Ch]        ; typeId (arg_0)
        push    edi
        mov     esi, ecx                ; this
        call    PopulateField2D8FromAnimProps
        cmp     dword ptr [esp + 10h], 0   ; replayAnim (arg_4)
        jz      skipReplay
        mov     eax, [esi]
        mov     edx, [eax + 8Ch]        ; slot35 - CUnit::PlayAnimation
        push    1
        push    200h
        push    0
        mov     ecx, esi
        call    edx
        fcomp   g_animReplayThreshold
        push    1
        mov     ecx, esi
        fnstsw  ax
        test    ah, 44h
        mov     eax, [esi]
        jp      useLooping
        mov     edx, [eax + 8Ch]        ; slot35 again
        push    200h
        jmp     mergeCall
    useLooping:
        mov     edx, [eax + 90h]        ; slot36 - CUnit::PlayAnimationLooping
        push    0
    mergeCall:
        push    4
        call    edx
        fstp    st(0)
    skipReplay:
        cmp     dword ptr [esp + 14h], 0   ; refreshSplat (arg_8)
        jz      skipSplat
        mov     ecx, esi
        call    RefreshUberSplatAttachment
    skipSplat:
        mov     edx, edi
        lea     ecx, [esp + 0Ch]        ; reuse the typeId stack slot as the
                                        ; hidden return buffer
        ;  GetFootprintModelColor (0x6F32D390, Pathfinding/footprintshadowoffset.cpp) -
        ;  called by raw address rather than by name: MASM's automatic
        ;  symbol lookup does not resolve a fastcall free function whose
        ;  mangled name is shifted by a hidden-return-buffer parameter.
        mov     eax, 06F32D390h
        call    eax
        mov     eax, [eax]
        push    edi
        mov     ecx, esi
        mov     [esi + 2D4h], eax
        call    RebuildAnimationIndexTable
        pop     edi
        pop     esi
        retn    0Ch
    }
}
