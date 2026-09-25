//============================================================================
//  0x6F5CB1A0 / 0x6F5CB240 - TSHashTable<T,HASHKEY_STR>::NewNode for the two
//  create-handler registries, one instantiation of Containers/tshash.inl's
//  template each.
//
//  Instantiating NewNode drags Initialize, CheckGrow and Rehash in with it -
//  they are what it calls, they are real calls in the shipped stream
//  (0x6F5CA8F0 and 0x6F5CAFC0 for the first table) and none of the three is
//  in this dump, so none of them is named in funcmap.py.  Their bodies are
//  the same template that already matches at 0x6F4700B0 / 0x6F470CC0 /
//  0x6F4701D0.
//
//  Own translation unit: the registration loops call NewNode for real, and
//  the lookup that runs first is in a third.
//============================================================================
#include "framereg.h"
#include "tshash.inl"

template FRAMENAMECREATEHANDLER*
         TSHashTable<FRAMENAMECREATEHANDLER, HASHKEY_STR>::NewNode(
             unsigned int, void*, void*);
template SIMPLEFRAMENAMECREATEHANDLER*
         TSHashTable<SIMPLEFRAMENAMECREATEHANDLER, HASHKEY_STR>::NewNode(
             unsigned int, void*, void*);
