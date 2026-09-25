//============================================================================
//  Two small leaves reached from CUnit's own constructor tree
//  (sub_6F2A0E30, 0x6F28xxxx module, out of scope on its own - 120+
//  functions per its calltree dump, and not one of this batch's 50) - both
//  are self-contained enough to reconstruct without the constructor
//  itself.
//
//  0x6F28B740 - looks up a fixed FourCC (0x6F024F00, a plain constant
//  getter already reconstructed as GetFourCC_6F024F00) and hands every
//  live unit of that type off to a per-instance callback
//  (sub_6F28B6C0, no dump anywhere in this tree - left a naked thunk).
//  `this` is the object handed through as the callback's context, and the
//  trailing `1` is EnumerateUnitsInWorld's own fourth argument the same
//  way CGameWorld::SetTextAgentsPaused (gameuiplayerenum.cpp) passes 0 -
//  a real value here, not a placeholder, per the shipped stack layout.
//
//  0x6F28CAA0 - resets the object's own +0x250 handle pair
//  (unit.h's m_ref250) to the "no handle" sentinel {-1,-1} between two
//  undumped ctor-helper calls (sub_6F277370, sub_6F282FC0 - both left
//  naked thunks, no dump anywhere in this tree), then a third
//  (sub_6F283F60, same), then dispatches CWidget::AddFootprint (slot 82,
//  +0x148, retn 0Ch matches the three pushed 1's) and CUnit::SetFlagBit2
//  (slot 40, +0xA0, retn 4 matches the one pushed 1) through the vtable -
//  both real slot targets already reconstructed (widget_addfootprint.cpp,
//  unit_flagbit2.cpp) - rather than a direct call, matching the shipped
//  indirection exactly.
//============================================================================
#include "unit.h"
#include "agenteventbinding.h"   // EnumerateUnitsOfType, UnitQueryVisitFn
#include "widget.h"              // CWidget::AddFootprint (slot 82)

unsigned int GetFourCC_6F024F00();

//  0x6F28B6C0 - the per-instance callback EnumerateUnitsOfType hands each
//  matching unit to.  No dump anywhere in this tree; naked thunk.
__declspec(naked) int __fastcall UnitCallback_6F28B6C0(void*, void*)
{
    __asm
    {
        mov     eax, 06F28B6C0h
        jmp     eax
    }
}

//  0x6F28B740 - ctor helper: notify every existing unit of this fixed type
//  about `this`.
void CUnit::NotifyExistingUnitsOfType_6F28B740()
{
    EnumerateUnitsOfType(GetFourCC_6F024F00(), UnitCallback_6F28B6C0,
                         this, 1);
}

//  0x6F277370 / 0x6F282FC0 / 0x6F283F60 - undumped ctor-helper leaves
//  called with only `this` in ecx.  Naked thunks, no dump anywhere in
//  this tree.
__declspec(naked) void CUnit::CtorHelper_6F277370()
{
    __asm
    {
        mov     eax, 06F277370h
        jmp     eax
    }
}

__declspec(naked) void CUnit::CtorHelper_6F282FC0()
{
    __asm
    {
        mov     eax, 06F282FC0h
        jmp     eax
    }
}

__declspec(naked) void CUnit::CtorHelper_6F283F60()
{
    __asm
    {
        mov     eax, 06F283F60h
        jmp     eax
    }
}

//  0x6F28CAA0 - ctor helper: reset the +0x250 handle pair to "none" and
//  re-run the footprint/flag setup through the object's own vtable.
void CUnit::InitFootprintAndFlags_6F28CAA0()
{
    CtorHelper_6F277370();
    m_ref250.m_b = (unsigned int)-1;
    m_ref250.m_t = (unsigned int)-1;
    CtorHelper_6F282FC0();
    CtorHelper_6F283F60();
    AddFootprint(1, 1, 1);
    SetFlagBit2(1);
}

//  0x6F28CAF0 - cunit_agent5_worklist.md, CUnit closure BFS depth 1-2.
//  A tiny wrapper immediately ahead of CtorHelper_6F282FC0 in the object
//  file: set CAgentWar3::m_flags (agentwar3.h) bit 0x40, then tail-call
//  the same undumped ctor helper.  A `jmp`, not a `call` + `retn`: this
//  wrapper has no stack cleanup of its own, so the callee's own `retn`
//  (currently the naked thunk's own, until 0x6F282FC0 gets a real dump)
//  stands in for it directly.
void CUnit::SetFlagBit40_6F28CAF0()
{
    m_flags |= 0x40;
    CtorHelper_6F282FC0();
}
