//============================================================================
//  0x6F2B88A0 - the lookup instantiation for ITEM_TYPE_DATA.  See
//  itemtypetable.h; the body is tshash.inl's, a third copy of the same
//  template alongside tshashtype.cpp's and the footprint table's.
//============================================================================
#include "itemtypetable.h"
#include "tshash.inl"

template ITEM_TYPE_DATA*
TSHashTable<ITEM_TYPE_DATA, AGILE_TYPE_ID>::Ptr(unsigned int,
                                                const AGILE_TYPE_ID&);
