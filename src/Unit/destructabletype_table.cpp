//============================================================================
//  0x6F2686C0 - TSHashTable<CDestructableTypeData, AGILE_TYPE_ID>::Ptr, the
//  one template member every one of the four lookup helpers
//  (destructabletype_lookup*.cpp) calls.  See destructabletypedata.h.
//============================================================================
#include "destructabletypedata.h"
#include "tshash.inl"

template CDestructableTypeData*
TSHashTable<CDestructableTypeData, AGILE_TYPE_ID>::Ptr(unsigned int, const AGILE_TYPE_ID&);
