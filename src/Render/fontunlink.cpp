//============================================================================
//  0x6F4CCA70 - TSHashTable<FONTHASHOBJ,HASHKEY_STR>::Unlink.  Its own
//  translation unit: CreateFont calls it for real on the failure path.
//============================================================================
#include "text.h"
#include "tshashstr.inl"

template void CFontTable::Unlink(FONTHASHOBJ*);
