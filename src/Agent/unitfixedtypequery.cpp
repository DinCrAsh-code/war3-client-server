//============================================================================
//  0x6F1ED120 - stage a small on-stack record from `this` and two incoming
//  pointers, then hand every live unit of a fixed type ('Asla',
//  sub_6F1D6100 - GetConstant_6F1D6100, already a plain leaf elsewhere)
//  to a callback one hop past this agent's own BFS cut (0x6F1E83C0 -
//  a naked redirect below, same convention cunit_agent8_abilityfactories.cpp
//  already uses for an out-of-slice callee).  EnumerateUnitsOfType itself
//  is already reconstructed (unitqueryoftype.cpp); its own `recurse`
//  argument is fixed 0 here, same as its one other call site.
//
//  The record's four fields are exactly the ones the shipped body writes,
//  in the order the callback (0x6F1E83C0's own `[ebx+0Ch]`, `[edi]`,
//  `[edi+4]`, `[edi+8]`, `[edi+0Ch]` reads) shows they are read back
//  through - not named further since nothing in this agent's own slice
//  reaches their real meaning.
//============================================================================
#include "agenteventbinding.h"

extern "C" int __fastcall UnitVisitCallback_6F1E83C0(void*, void*);  // 0x6F1E83C0 - defined below as a naked redirect
unsigned int GetConstant_6F1D6100();  // 0x6F1D6100 - 'Asla' (already a leaf elsewhere)

namespace {
struct SFixedTypeQueryCtx6F1ED120
{
    void* m_field00;   // +0x00 - incoming ecx
    int   m_field04;   // +0x04 - incoming pair's first word
    int   m_field08;   // +0x08 - incoming pair's second word
    int   m_field0C;   // +0x0C - incoming stack pointer's target
};
}

void __fastcall EnumerateFixedTypeUnits_6F1ED120(void* owner, const int* pair,
                                                  const int* single)
{
    SFixedTypeQueryCtx6F1ED120 ctx;
    ctx.m_field00 = owner;
    ctx.m_field04 = pair[0];
    ctx.m_field08 = pair[1];
    ctx.m_field0C = *single;

    EnumerateUnitsOfType(GetConstant_6F1D6100(), UnitVisitCallback_6F1E83C0, &ctx, 0);
}

//  0x6F1E83C0 - naked redirect into the real, unhooked shipped callback.
//  Not this agent's own slice; a real reconstruction is somebody else's
//  worklist item. retn 0 (fastcall, two register args, no stack bytes) is
//  read straight off its own raw_bytes, not inferred from this signature.
extern "C" __declspec(naked) int __fastcall UnitVisitCallback_6F1E83C0(void*, void*)
{
    __asm { mov eax, 06F1E83C0h }
    __asm { jmp eax }
}
