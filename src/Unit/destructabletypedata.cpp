//============================================================================
//  CDestructable's four type-data-table slots: 66, 69, 94 and 95
//  (Method_0x108/Method_0x114/Method_0x178/Method_0x17C).  All four hash
//  `m_footprintType` and look it up in the same table
//  (destructabletypedata.h) through the one TSHashTable<T,K>::Ptr
//  instantiation (destructabletype_table.cpp), but not identically:
//
//    * slots 66 and 94 tail-jump/call into a real, standalone helper each
//      (LookupReservedA0/LookupReserved68, destructabletype_lookupa0.cpp/
//      destructabletype_lookup68.cpp) - both have their own address in the
//      dump (0x6F268D90/0x6F268A10) and their own `verify/target/*.txt`,
//      so each stays in its own translation unit to keep that call real;
//    * slots 69 and 95 do not - the "hash, look up, return one int field
//      or 0" logic at 0x6F268830/0x6F268800 has no `verify/target/*.txt`
//      of its own (IDA folds it into its one caller's own listing), which
//      is what a private, fully-inlined copy with no other caller looks
//      like, and is why it is written out directly in each slot body below
//      rather than factored into a shared function the way 66/94's is.
//============================================================================
#include "destructable.h"
#include "destructabletypedata.h"

//  destructabletype_lookupa0.cpp
float __fastcall LookupReservedA0(unsigned int footprintType);
//  destructabletype_lookup68.cpp
unsigned int* __fastcall LookupReserved68(unsigned int* out, unsigned int footprintType);

//----------------------------------------------------------------------------
//  0x6F269660 - slot 66 (+0x108).
//----------------------------------------------------------------------------
float CDestructable::Method_0x108()
{
    return LookupReservedA0(m_footprintType);
}

//----------------------------------------------------------------------------
//  0x6F269630 / 0x6F268830 - slot 69 (+0x114).
//----------------------------------------------------------------------------
int CDestructable::Method_0x114()
{
    AGILE_TYPE_ID key(m_footprintType);
    CDestructableTypeData* data = g_destructableTypeTable.Ptr(key.Hash(), key);
    return data ? data->m_reserved1C : 0;
}

//----------------------------------------------------------------------------
//  0x6F269640 - slot 94 (+0x178).
//----------------------------------------------------------------------------
unsigned int* CDestructable::Method_0x178(unsigned int* out)
{
    LookupReserved68(out, m_footprintType);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F269620 / 0x6F268800 - slot 95 (+0x17C).
//----------------------------------------------------------------------------
unsigned int CDestructable::Method_0x17C()
{
    AGILE_TYPE_ID key(m_footprintType);
    CDestructableTypeData* data = g_destructableTypeTable.Ptr(key.Hash(), key);
    return data ? (unsigned int)data->m_reserved18 : 0;
}
