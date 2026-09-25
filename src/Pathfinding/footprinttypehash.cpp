//============================================================================
//  The footprint type table: TSHashTable instantiated for CFootprintTypeData.
//  0x6F319810 Ptr, 0x6F327D90 Initialize, 0x6F32A810 CheckGrow,
//  0x6F328030 Rehash, 0x6F32B5B0 NewNode.
//============================================================================
#include "footprinttype.h"
#include "tshash.inl"

template CFootprintTypeData*
TSHashTable<CFootprintTypeData, AGILE_TYPE_ID>::Ptr(unsigned int, const AGILE_TYPE_ID&);
template void TSHashTable<CFootprintTypeData, AGILE_TYPE_ID>::Initialize();
template int  TSHashTable<CFootprintTypeData, AGILE_TYPE_ID>::CheckGrow(unsigned int);
template void TSHashTable<CFootprintTypeData, AGILE_TYPE_ID>::Rehash(unsigned int);
template CFootprintTypeData*
TSHashTable<CFootprintTypeData, AGILE_TYPE_ID>::NewNode(unsigned int, void*, void*);
