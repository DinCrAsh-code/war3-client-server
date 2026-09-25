//============================================================================
//  0x6F004390 - SPreferenceCache::Find: the preference store's bucket walk.
//
//  Storm's TSHashTable<T,K>::PtrHashOnly (Containers/tshash.inl, 0x6F42CCF0)
//  instruction for instruction - the mask test, the masked Tail() into the
//  walk, the raw m_prevlink step out of Prev() and the `> 0` sign test that
//  ends it - with the bucket expression written out again in the step
//  because the shipped code reloads m_buckets and m_mask there.
//
//  It is *not* spelled as an instantiation of that template, and the second
//  parameter is why: the shipped call sites (CPreferencesWar3::GetIntValue
//  and ::StoreValue) push `offset unk_6FAAE310`, a module-global the callee
//  never reads, where PtrHashOnly's `const K&` would push the address of a
//  key object.  Both produce the same instruction, but the name this repo
//  already gave the address is SPreferenceCache::Find and one address
//  carries one name, so the body lives here under that name rather than
//  renaming two call sites in preferences.cpp to reach a template.
//
//  Its own translation unit: both call sites are real calls.
//============================================================================
#include "preferences.h"

SPreferenceEntry* SPreferenceCache::Find(int key, void*)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    //  Walked as a raw `int` and ended on `> 0`, not as a pointer tested
    //  against null: Prev() hands the link field back as it stands and the
    //  value the walk ends on is the terminator's complemented address.
    //  Spelling it this way - the cast to the node type inside the body,
    //  not in the loop condition - is what CSkinValueTable::Find
    //  (gameuiskintable.cpp) needed for the same walk, and it is worth the
    //  same instructions here.
    int link = (int)m_buckets[key & m_mask].Tail();
    while (link > 0)
    {
        SPreferenceEntry* node = (SPreferenceEntry*)link;

        if (node->m_hashval == (unsigned int)key)
            return node;

        link = (int)m_buckets[key & m_mask].Prev(node);
    }
    return 0;
}
