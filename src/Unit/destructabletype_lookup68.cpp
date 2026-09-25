//============================================================================
//  0x6F268A10 - slot 94's own helper (0x6F269640, destructabletypedata.cpp
//  really calls it, rather than tail-jumping, since slot 94 has an out-
//  pointer of its own to return).  Own translation unit so that call stays
//  real.
//
//  0x6FAAE470 is g_CFloatZero's own encoded bits (CFloat.h) - the "not
//  found" default, handed back as a raw dword because the out-pointer this
//  fills is `unsigned int*`, matching CWidget's own slot 94 signature
//  exactly (widget.h).
//============================================================================
#include "destructabletypedata.h"
#include "CFloat.h"

unsigned int* __fastcall LookupReserved68(unsigned int* out, unsigned int footprintType)
{
    AGILE_TYPE_ID key(footprintType);
    CDestructableTypeData* data =
        g_destructableTypeTable.Ptr(key.Hash(), key);
    *out = data ? data->m_reserved68 : *(const unsigned int*)&g_CFloatZero;
    return out;
}
