//============================================================================
//  The relationship table: TSHashTable instantiated for
//  AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP.  Everything in the shipped
//  0x6F470000-0x6F471400 neighbourhood comes from this instantiation.
//============================================================================
#include "agiletype.h"
#include "tshash.inl"

typedef AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP REL;

template REL* TSHashTable<REL, AGILE_TYPE_ID>::Ptr(unsigned int,
                                                   const AGILE_TYPE_ID&);
template void TSHashTable<REL, AGILE_TYPE_ID>::Initialize();
template int  TSHashTable<REL, AGILE_TYPE_ID>::CheckGrow(unsigned int);
template void TSHashTable<REL, AGILE_TYPE_ID>::Rehash(unsigned int);
template REL* TSHashTable<REL, AGILE_TYPE_ID>::NewNode(unsigned int, void*, void*);
template REL* TSHashTable<REL, AGILE_TYPE_ID>::New(unsigned int,
                                                   const AGILE_TYPE_ID&,
                                                   void*, void*);
