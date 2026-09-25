//============================================================================
//  CAbility slot 165's own GameUI-rendering closure
//  (asm/sub_6F02E160_0x6F02E160_calltree_asm.md), cability_depth5_closure_
//  worklist.json batch 3 - a small per-widget cache keyed by a two-field
//  key (an object handle plus a sub-index), the same TSHashTable<T, K>
//  template every other instantiation in this binary shares
//  (Storm/storm.h, Containers/tshash.inl), this time with K itself a
//  two-int compound rather than a single AGILE_TYPE_ID.
//============================================================================
#ifndef WIDGET_SELECTIONDEPTHCACHE_H
#define WIDGET_SELECTIONDEPTHCACHE_H

#include "storm.h"
#include "unitmembership.h"

//----------------------------------------------------------------------------
//  The two-int key: an object pointer/handle and a sub-index.  Hashed by
//  SDepthCacheKey::Hash below - the same rolling string-hash
//  AGILE_TYPE_ID::Hash (Agent/agileid.cpp) uses, generalised from one
//  dword to two.
//----------------------------------------------------------------------------
class SDepthCacheKey
{
public:
    SDepthCacheKey() {}
    SDepthCacheKey(unsigned int field0, unsigned int field4)
        : m_field0(field0), m_field4(field4) {}

    unsigned int Hash() const;      // 0x6F4C8570

    bool operator==(const SDepthCacheKey& other) const
    {
        return m_field0 == other.m_field0 && m_field4 == other.m_field4;
    }

    unsigned int m_field0;
    unsigned int m_field4;
};

class CDepthCacheRecord
    : public TSHashObject<CDepthCacheRecord, SDepthCacheKey>
{
public:
    char m_reserved18[0x1C - 0x18];
};

class CDepthCacheTable : public TSHashTable<CDepthCacheRecord, SDepthCacheKey>
{
public:
    virtual ~CDepthCacheTable();
    virtual CDepthCacheRecord* AllocNode(
        TSExplicitList<CDepthCacheRecord>* bucket, void* arg1, void* arg2);
};

//----------------------------------------------------------------------------
//  The unrecovered owner class holding the CDepthCacheTable at +0x160
//  and, per 0x6F421E70/0x6F41F6A0, a further owner pointer at +0x20C.
//  QueryField1C's own definition is split into its own translation unit
//  (widget_selectiondepthcachequery.cpp) so UpdateState's real call to
//  it (the shipped code has one) does not get inlined away.
//----------------------------------------------------------------------------
class CDepthCacheOwner
{
public:
    //  0x6F372C60 - resolve the depth-cache record for (a0, a1), and
    //  hand back its own +0x1C field's address, or null with no record.
    void* QueryField1C(unsigned int a0, unsigned int a1);
    //  0x6F372F00 - a small three-way state update on the resolved
    //  record's own (+0x1C+0x5BC) field, gated by whether it is
    //  currently 0, 1 or 2: state 0 moves to 1 or 2 depending on
    //  whether `flag` is nonzero, state 2 only advances to 1 when
    //  `flag` is zero, state 1 never changes.
    void UpdateState(unsigned int a0, unsigned int a1, unsigned int flag);
    //  0x6F41F6A0 - a plain field read, this object's own +0x20C.
    SUnitMembershipList* GetOwnerAt20C();
    //  0x6F421E70 - the same field, forwarded into
    //  SUnitMembershipList::Contains (Unit/unitmembershiplist.cpp) when
    //  present.
    int QueryOwnerAt20CField(void* unit);

    char             m_reserved00[0x160];
    CDepthCacheTable m_table;         // 0x160, sizeof 0x28
    char             m_reserved188[0x20C - 0x188];
    SUnitMembershipList* m_ownerAt20C;    // 0x20C
};

#endif
