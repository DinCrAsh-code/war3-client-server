//============================================================================
//  0x6F29CAC0 - UnitTypeField230, one more of the CUnit-type-table one-shot
//  probe family Unit/unittypequeries.cpp already documents in full
//  (UnitTypeField5C_6F29BAF0 etc.) - same `Ptr(id.Hash(), id)` shape,
//  reached off CUnit's own vtable BFS (cunit_agent1_worklist.md).
//============================================================================
#include "unittypetable.h"

int __fastcall UnitTypeField230_6F29CAC0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field230 : 0;
}
