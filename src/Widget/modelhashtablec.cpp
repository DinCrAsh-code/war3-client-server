//============================================================================
//  0x6F4F09C0 - SStringRefTable::Ptr (Widget/stringref.h): the string-keyed
//  registry lookup.  Same body as Containers/tshashstr.inl's generic
//  TSHashTable<T,HASHKEY_STRI>::Ptr(const char*) - written out again here
//  under SStringRefTable's own name rather than derived from that template,
//  for the reason Widget/sprresource_thunks.cpp's own registries give:
//  deriving one shipped symbol from another puts the body under the base's
//  mangled name, which is not the name this address carries.  Storm_509 is
//  HASHKEY_STRI::Compare, confirmed by the dump's own `call Storm_509`.
//
//  Was a naked redirect in Widget/stringref_thunks.cpp; replaced now that
//  its real body is reconstructed.  (Its sibling, NewNode at 0x6F4F1680,
//  is not - it dispatches through the table's own AllocNode virtual, whose
//  real body is below this dump's depth cut, so it stays a thunk there.)
//============================================================================
#include "stringref.h"
#include "allochashtable.h"

//  Storm ordinal 590 (string hash) - see Containers/tshash.inl's own
//  declaration of the same import.
unsigned int __stdcall Storm_590(const char* s);

SStringRef* SStringRefTable::Ptr(const char* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590(key);

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        SStringRef* node = (SStringRef*)link;

        if (node->m_hashval == hash && HASHKEY_STRI::Compare(node->m_key, key) == 0)
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}
