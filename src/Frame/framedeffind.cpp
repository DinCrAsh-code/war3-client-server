//============================================================================
//  0x6F5C7920 - TSHashTable<BASEFRAMEHASHNODE,HASHKEY_STR>::Ptr, the
//  frame-definition registry's lookup by name.
//
//  The template's string-keyed walk (Containers/tshashstr.inl); everything
//  that makes this instantiation different from the two handler tables is
//  the node layout, and that is BASEFRAMEHASHNODE's vftable pushing the
//  hash to +0x04 and the key to +0x18.
//
//  Own translation unit: both create entry points call it for real.
//============================================================================
#include "framereg.h"
#include "tshashstr.inl"

template BASEFRAMEHASHNODE*
         TSHashTable<BASEFRAMEHASHNODE, HASHKEY_STR>::Ptr(const char*);
