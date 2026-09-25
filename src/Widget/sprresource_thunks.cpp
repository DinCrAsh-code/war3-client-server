//============================================================================
//  The three sprite-resource registries' own Find/Insert/Unlink, and the
//  three constructors under them.
//
//  Find/Insert/Unlink are field-for-field Containers/allochashtable.h's
//  `TAllocatorHashTable<T, HASHKEY_STRI, N>::Ptr/SetKey/Unlink` - see
//  sprresource.h's own header comment for why they are written out here
//  rather than reached through that template.
//
//  The three constructors stay redirects: none is in this dump - every one
//  is below its depth cut - and each pulls in half a dozen more callees
//  (a sorted TSGrowableArray build via qsort, a placement-registration
//  callback) whose own field layouts nothing in this closure establishes.
//  The declared argument bytes agree with each shipped `retn <n>`, and for
//  every one of these the count is fixed by the shipped call site rather
//  than guessed: the caller pushes N dwords and never adjusts esp
//  afterwards, so the callee cleans exactly those N.
//============================================================================
#include "sprresource.h"

//----------------------------------------------------------------------------
//  0x6F4ECF20 / 0x6F4EFB10 / 0x6F4EFBE0 - find the node for a key.  See
//  allochash.inl's Ptr() for the same body; HASHKEY_STRI::Compare is Storm
//  ordinal 509 (SStrnICmp), which is what each dump's own `call Storm_509`
//  confirms.
//----------------------------------------------------------------------------
SSprAnimList* SSprAnimListRegistry::Find(void* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590((const char*)key);

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        SSprAnimList* node = (SSprAnimList*)link;

        if (node->m_hashval == hash
            && HASHKEY_STRI::Compare(node->m_key, (const char*)key) == 0)
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}

SSprLinkTable* SSprLinkTableRegistry::Find(void* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590((const char*)key);

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        SSprLinkTable* node = (SSprLinkTable*)link;

        if (node->m_hashval == hash
            && HASHKEY_STRI::Compare(node->m_key, (const char*)key) == 0)
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}

SSprLinkTableSecondary* SSprLinkTableSecondaryRegistry::Find(void* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590((const char*)key);

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        SSprLinkTableSecondary* node = (SSprLinkTableSecondary*)link;

        if (node->m_hashval == hash
            && HASHKEY_STRI::Compare(node->m_key, (const char*)key) == 0)
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4ED0E0 / 0x6F4F0010 / 0x6F4F0070 - SetKey().  Move the node to the
//  bucket the key hashes to, stamp the hash, and take a private copy of the
//  string - see allochash.inl's SetKey() for the same body and the
//  `m_key != key` re-use guard's own reasoning.  Void: see sprresource.h's
//  comment on why the header no longer claims this returns the node.
//----------------------------------------------------------------------------
void SSprAnimListRegistry::Insert(SSprAnimList* made, void* key)
{
    unsigned int hash = Storm_590((const char*)key);

    LinkNode(made, hash);
    made->m_hashval = hash;

    if (made->m_key != key)
    {
        if (made->m_key != 0)
            SMemFree((void*)made->m_key, "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA38, 0);

        made->m_key = Storm_507((const char*)key,
                                "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA39);
    }
}

void SSprLinkTableRegistry::Insert(SSprLinkTable* made, void* key)
{
    unsigned int hash = Storm_590((const char*)key);

    LinkNode(made, hash);
    made->m_hashval = hash;

    if (made->m_key != key)
    {
        if (made->m_key != 0)
            SMemFree((void*)made->m_key, "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA38, 0);

        made->m_key = Storm_507((const char*)key,
                                "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA39);
    }
}

void SSprLinkTableSecondaryRegistry::Insert(SSprLinkTableSecondary* made,
                                            void* key)
{
    unsigned int hash = Storm_590((const char*)key);

    LinkNode(made, hash);
    made->m_hashval = hash;

    if (made->m_key != key)
    {
        if (made->m_key != 0)
            SMemFree((void*)made->m_key, "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA38, 0);

        made->m_key = Storm_507((const char*)key,
                                "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA39);
    }
}

//----------------------------------------------------------------------------
//  0x6F4EB600 - take a node back out of the anim-list registry; the release
//  path's first call.  Same guard and the same two-link splice
//  allochash.inl's Unlink() has, written out again here because this
//  instantiation's own dump fully inlines TSLink::Unlink() at both call
//  sites instead of leaving a call (a register-allocation difference
//  between instantiations, not a different algorithm - see
//  allochash.inl's own comment on the "three spellings measured" for this
//  exact shape).
//----------------------------------------------------------------------------
void SSprAnimListRegistry::Unlink(SSprAnimList* node)
{
    if (node->m_hashlink.m_prevlink != 0)
    {
        node->m_hashlink.Unlink();
        node->m_listlink.Unlink();
    }
}

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  The three constructors, `this` in ecx and two stack arguments each
//  (`retn 8`) - members and not free __fastcalls, which would put the first
//  of those two in edx and clean four bytes too few.
ADDR_THUNK(void SSprAnimList::Construct(void*, const char*), 0x6F4ECC60)
ADDR_THUNK(void SSprLinkTable::Construct(void*, const char*), 0x6F4EF810)
ADDR_THUNK(void SSprLinkTableSecondary::Construct(void*, const char*),
           0x6F4EF980)

//  Each registry's own LinkNode - Insert()'s one real call.  See
//  sprresource.h's comment on SSprAnimListRegistry::LinkNode.
ADDR_THUNK(void SSprAnimListRegistry::LinkNode(SSprAnimList*, unsigned int),
           0x6F4ECFB0)
ADDR_THUNK(void SSprLinkTableRegistry::LinkNode(SSprLinkTable*, unsigned int),
           0x6F4EFDB0)
ADDR_THUNK(void SSprLinkTableSecondaryRegistry::LinkNode(
               SSprLinkTableSecondary*, unsigned int), 0x6F4EFE90)

#undef ADDR_THUNK
