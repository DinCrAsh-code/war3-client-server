//============================================================================
//  0x6F29F020 - a thin wrapper around UnitTypeField190_6F29C050 (the
//  CUnit-type-table CFloat-out-pointer probe family
//  Unit/unittypequeries.cpp documents in full, e.g. UnitTypeField74_
//  6F29BDB0) that reads its fourCC argument from the receiver's own +0x30
//  instead of taking it directly.  Reached off CUnit's own vtable BFS
//  (cunit_agent1_worklist.md).  `this`'s own class is not established
//  anywhere in this call tree (a plain field read, not a virtual call, so
//  nothing here forces it to be CUnit even though the BFS root is), so it
//  is modelled as a bare receiver rather than a guessed class.
//============================================================================
#include "CFloat.h"

//  0x6F29C050 - cunit_agent1_unittypefield190_probe.cpp (a separate TU so
//  it stays a real out-of-line call here rather than being inlined away).
//  Not this batch's own worklist heading, so not claimed on its own.
CFloat* __fastcall UnitTypeField190_6F29C050(CFloat* out, unsigned int fourCC);

struct SUnitTypeField190Host { char m_reserved00[0x30]; unsigned int m_fourCC30; };

struct SUnitTypeField190Wrapper { CFloat* __thiscall QueryField190(CFloat* out); };

//  0x6F29F020
CFloat* __thiscall SUnitTypeField190Wrapper::QueryField190(CFloat* out)
{
    UnitTypeField190_6F29C050(out, ((SUnitTypeField190Host*)this)->m_fourCC30);
    return out;
}
