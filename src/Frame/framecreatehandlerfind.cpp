//============================================================================
//  0x6F5C8F60 / 0x6F5C8FF0 - the two create-handler registries' lookups by
//  name, one instantiation of Containers/tshashstr.inl's string-keyed walk
//  each.
//
//  Two identical function bodies at two addresses is normal here: the
//  shipped link had no /OPT:ICF, so one template over two node types stays
//  two functions (docs/msvc-vc8-idioms.md, "Four identical functions at
//  four addresses is normal here").
//
//  Own translation unit: both registration loops call one of these for
//  real, and their own NewNode lives in another.
//============================================================================
#include "framereg.h"
#include "tshashstr.inl"

template FRAMENAMECREATEHANDLER*
         TSHashTable<FRAMENAMECREATEHANDLER, HASHKEY_STR>::Ptr(const char*);
template SIMPLEFRAMENAMECREATEHANDLER*
         TSHashTable<SIMPLEFRAMENAMECREATEHANDLER, HASHKEY_STR>::Ptr(const char*);
