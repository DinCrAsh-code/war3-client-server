//============================================================================
//  0x6F268D90 - the tail target of slot 66 (0x6F269660,
//  destructabletypedata.cpp).  Own translation unit so that tail-jump
//  stays real.
//============================================================================
#include "destructabletypedata.h"

float __fastcall LookupReservedA0(unsigned int footprintType)
{
    AGILE_TYPE_ID key(footprintType);
    CDestructableTypeData* data =
        g_destructableTypeTable.Ptr(key.Hash(), key);
    return data ? data->m_reservedA0 : 0.0f;
}
