//============================================================================
//  The unit-type registry's copy of TSHashTable::Ptr - 0x6F29A8C0.
//
//  Its own translation unit for the same reason tshashtype.cpp is: every
//  one of the four accessors in unittypequeries.cpp calls it for real, so a
//  visible definition would let MSVC inline it into all four.
//============================================================================
#include "unittypetable.h"
#include "tshash.inl"

template SUnitTypeRecord*
TSHashTable<SUnitTypeRecord, AGILE_TYPE_ID>::Ptr(unsigned int,
                                                 const AGILE_TYPE_ID&);
