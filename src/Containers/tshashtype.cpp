//============================================================================
//  The registry table: TSHashTable instantiated for AGILE_TYPE_DATA.  Only
//  the lookup is reached from this call tree, and it is a second copy of the
//  same template - AGILE_TYPE_DATA carries a vftable, so its node header sits
//  four bytes further in and none of the offsets are shared with the
//  relationship table's copy.
//============================================================================
#include "agiletype.h"
#include "tshash.inl"

template AGILE_TYPE_DATA*
TSHashTable<AGILE_TYPE_DATA, AGILE_TYPE_ID>::Ptr(unsigned int,
                                                 const AGILE_TYPE_ID&);
