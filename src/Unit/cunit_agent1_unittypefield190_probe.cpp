//============================================================================
//  0x6F29C050 - UnitTypeField190's own probe body (same CFloat-out-pointer
//  shape as UnitTypeField74_6F29BDB0 etc., Unit/unittypequeries.cpp).  Own
//  translation unit so its one caller (cunit_agent1_unittypefield190.cpp,
//  0x6F29F020) reaches it with a real out-of-line `call` rather than
//  having this build inline it away - measured: kept in the same TU as
//  its caller, this leaf disappears entirely and the wrapper's own
//  generated stream balloons from 8 to 21 instructions.
//============================================================================
#include "unittypetable.h"
#include "CFloat.h"

CFloat* __fastcall UnitTypeField190_6F29C050(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field190;
    else
        *out = g_CFloatZero;
    return out;
}
