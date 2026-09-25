//============================================================================
//  0x6F319890 and 0x6F319920 - the two hash-table lookups the skin system
//  is built out of.  Both bodies are byte-for-byte identical, which is
//  normal in this image (see docs/msvc-vc8-idioms.md, "Four identical
//  functions at four addresses is normal here"): the shipped link had no
//  /OPT:ICF, and these are two instantiations over two different node types.
//
//  A table is a power-of-two array of TSExplicitLists (storm.h - twelve
//  bytes each: the link offset, then the terminator's two words) plus the
//  mask at +0x24, which is -1 while the table has never been built.  The
//  walk enters on the *masked* Tail() and steps on the raw Prev(), testing
//  its sign - the pattern every TSExplicitList walk in this binary has, and
//  the same shape Misc/lockedkeylist.cpp already carries.
//
//  Both the hash and the string are compared, in that order, which is what
//  makes the SStrnICmp call conditional on the cheap test.
//
//  Own translation unit; neither calls the other.
//============================================================================
#include "storm.h"
#include "config.h"     // SStrnICmp (ordinal 509), Storm_590 (the hash)
#include "gameuiskin.h"

//----------------------------------------------------------------------------
//  0x6F319890 - a value inside one skin section.
//----------------------------------------------------------------------------
SSkinValue* CSkinValueTable::Find(const char* name)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590(name);

    int link = (int)m_pBuckets[hash & m_mask].Tail();
    while (link > 0)
    {
        SSkinValue* node = (SSkinValue*)link;

        if (node->m_hash == hash
            && SStrnICmp(node->m_name, name, 0x7FFFFFFF) == 0)
            return node;

        link = (int)m_pBuckets[hash & m_mask].Prev(node);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F319920 - a section inside the skin table.
//----------------------------------------------------------------------------
SSkinSection* CSkinSectionTable::Find(const char* name)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590(name);

    int link = (int)m_pBuckets[hash & m_mask].Tail();
    while (link > 0)
    {
        SSkinSection* node = (SSkinSection*)link;

        if (node->m_hash == hash
            && SStrnICmp(node->m_name, name, 0x7FFFFFFF) == 0)
            return node;

        link = (int)m_pBuckets[hash & m_mask].Prev(node);
    }

    return 0;
}
