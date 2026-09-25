//============================================================================
//  All seven TSHashTable<T,K> members for one internal hash table reached
//  from inside the JASS instance constructor's own closure (the 0x6F4549E0
//  neighbourhood, which walks a whole trail of TAllocatorHashTable/
//  TSHashTable members for named tables - ScriptDataTable, FuncAddr,
//  JassHandle, String2HandleReg, FuncAddr2HandleReg, FuncAddr2NameReg - see
//  jassfuncaddrlookup.cpp for the one member of that trail this session's
//  own closure actually reaches and can therefore name for real).  This
//  table's own construction site never surfaced in this session's slice of
//  the call tree, so - like jassnativeregtable.cpp's table - its element
//  type is not recoverable from here: none of the seven members below ever
//  touch anything past m_hashval (offset 0, the fixed point tshash.inl's
//  Rehash already documents) and m_hashlink, whose offset is 0x04 - the
//  plain, non-virtual TSHashObject<T,K> layout, with nothing this call tree
//  reads past it.  Own TU because it is a self-contained table (this
//  session owns all seven members, unlike jassnativeregtable.cpp's table)
//  and the real class name would otherwise collide with jassinstancetablec.cpp's.
//============================================================================
#include "storm.h"
#include "tsarray.inl"
#include "tshash.inl"
#include "tslist.inl"

struct JassInstanceSlotB : public TSHashObject<JassInstanceSlotB, HASHKEY_NONE>
{
};

typedef JassInstanceSlotB NODE;
typedef TSExplicitList<NODE> BUCKET;

//  0x6F4488D0
template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
//  0x6F44F2D0
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
//  0x6F452AF0
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
//  0x6F453CA0
template void TSHashTable<NODE, HASHKEY_NONE>::Initialize();
//  0x6F454470
template int TSHashTable<NODE, HASHKEY_NONE>::CheckGrow(unsigned int);
//  0x6F453D00
template void TSHashTable<NODE, HASHKEY_NONE>::Rehash(unsigned int);
//  0x6F454780
template NODE* TSHashTable<NODE, HASHKEY_NONE>::NewNode(unsigned int, void*, void*);

//  UnlinkAll() this table's own Initialize/Rehash call (through
//  SetLinkOffset(), a Storm/storm.h inline) - T-independent code with no
//  address of its own in this closure.
template void TSExplicitList<NODE>::UnlinkAll();
