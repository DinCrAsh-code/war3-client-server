//============================================================================
//  The two TSHashTable members that only a *string*-keyed instantiation has:
//  a lookup that hashes the key itself, and a node unlink.  Included only by
//  the translation unit that owns such an instantiation.
//
//  tshash.inl comes with it because the same instantiation needs
//  Initialize/CheckGrow/Rehash/NewNode, whose bodies do not depend on the
//  key at all.
//============================================================================
#include "tshash.inl"
#include "allochashtable.h"

//----------------------------------------------------------------------------
//  0x6F4CD050 - find the node for a string key.  The same walk Ptr(hashval,
//  key) has, with the hash taken here and the compare going through the key
//  policy: HASHKEY_STR is Storm ordinal 508 (case sensitive), HASHKEY_STRI
//  is 509.
//----------------------------------------------------------------------------
template <class T, class K>
T* TSHashTable<T, K>::Ptr(const char* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590(key);

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        T* node = (T*)link;

        if (node->m_hashval == hash && K::Compare(node->m_key, key) == 0)
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4CCA70 - take a node out of its bucket and out of the table-wide list.
//----------------------------------------------------------------------------
template <class T, class K>
void TSHashTable<T, K>::Unlink(T* ptr)
{
    //  See Containers/allochash.inl's twin for the three spellings measured
    //  here; the shipped build's second callee-saved register is not
    //  source-addressable.
    if (ptr->m_hashlink.m_prevlink != 0)
    {
        ptr->m_hashlink.Unlink();
        ptr->m_listlink.Unlink();
    }
}
