//============================================================================
//  The agile type registry: a table of AGILE_TYPE_DATA records keyed by
//  four-character type id, each record naming its parent type and carrying a
//  memo table of the ancestry questions already answered about it.
//
//  The two class names come out of the binary rather than out of a guess: the
//  RTTI type-descriptor name the allocator is handed while the memo table's
//  bucket array is resized is
//  `TSExplicitList<AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP, -572662307>`.
//============================================================================
#ifndef AGILETYPE_H
#define AGILETYPE_H

#include "storm.h"

//----------------------------------------------------------------------------
//  A four-character type id, hashed as a string: byte at a time from the low
//  end, stopping at the first zero byte, so "hfoo" hashes over four bytes and
//  a two-letter id over two.  The mixing constants and the
//  seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3 recurrence are Storm's
//  string hash; the table it folds each byte through is only sixteen entries
//  wide and is indexed by the byte's two nibbles separately, which is this
//  binary's variant of it.
//----------------------------------------------------------------------------
extern const unsigned int g_agileHashTable[16];     // 0x6F9548D0

class AGILE_TYPE_ID
{
public:
    AGILE_TYPE_ID() {}
    AGILE_TYPE_ID(unsigned int id) : m_id(id) {}

    unsigned int Hash() const;      // 0x6F4C8520

    bool operator==(const AGILE_TYPE_ID& other) const
    {
        return m_id == other.m_id;
    }

    unsigned int m_id;
};

//: The root of the type hierarchy - bytes 'a','g','a','+'.  Every type is
//: derived from it, which is the first thing both entry points test for.
const unsigned int kAgileRootTypeId = 0x2B616761;

class CAgent;

//----------------------------------------------------------------------------
//  The object AGILE_TYPE_DATA parks at +0x70: the pool a registered type's
//  instances are handed back to.  Only the one slot CAgent::ReleaseSelf
//  (agent_releaseself.cpp) calls is named; slot 0 is declared only to hold
//  the slot, and neither is ever defined - nothing constructs one of these,
//  so no vtable of it is emitted and nothing has to link.
//----------------------------------------------------------------------------
class CAgileTypeAllocator
{
public:
    virtual void Slot00();
    virtual void ReleaseObject(CAgent* obj);
};

//----------------------------------------------------------------------------
//  One type's record.  It is itself a hash-table node (keyed by its own type
//  id in the registry) and it carries a vftable, which is what pushes the
//  node header from +0x00 to +0x04 and makes the registry's lookup a second
//  instantiation of TSHashTable::Ptr rather than a shared one.
//----------------------------------------------------------------------------
class AGILE_TYPE_DATA : public TSHashObject<AGILE_TYPE_DATA, AGILE_TYPE_ID>
{
public:
    virtual ~AGILE_TYPE_DATA();

    //------------------------------------------------------------------------
    //  "is my type derived from that one" answered once and remembered.
    //------------------------------------------------------------------------
    struct AGILE_TYPE_RELATIONSHIP
        : public TSHashObject<AGILE_TYPE_RELATIONSHIP, AGILE_TYPE_ID>
    {
        int m_derived;      // 0x18
    };

    //  The table owns its nodes, so the concrete class is the one that
    //  supplies AllocNode; that override is not in this call tree (it is
    //  reached through the vftable) and neither is anything it might add to
    //  the layout, so nothing is invented for it here.
    class AGILE_TYPE_RELATIONSHIP_TABLE
        : public TSHashTable<AGILE_TYPE_RELATIONSHIP, AGILE_TYPE_ID>
    {
    public:
        virtual ~AGILE_TYPE_RELATIONSHIP_TABLE();
        virtual AGILE_TYPE_RELATIONSHIP* AllocNode(
            TSExplicitList<AGILE_TYPE_RELATIONSHIP>* bucket,
            void* arg1, void* arg2);
    };

    AGILE_TYPE_RELATIONSHIP_TABLE m_relations;      // 0x1C
    char            m_reserved44[0x28];

    //  0x6C - a lazily-built per-type object, filled in on first use by
    //  QueryAgileTypeObject (0x6F4729C0, agiletypequery.cpp) and never
    //  cleared.  Nothing in this codebase reads what is in it, only that it
    //  is null until that function has run once.
    void*           m_field6C;      // 0x6C

    //  0x70 - the pool this type's objects come from and go back to.
    //  CAgent::ReleaseSelf (agent_releaseself.cpp) is the first thing to
    //  reach into what used to be one 0x34-byte reserved block: it looks
    //  its own type up here and hands `this` to slot 1 of whatever sits at
    //  this offset, through a genuine virtual call.  Splitting the block
    //  is what lets that call compile to the shipped
    //  `mov ecx,[eax+70h]` / `mov edx,[ecx]` / `mov eax,[edx+4]`, with the
    //  loaded pointer used as both vtable base and `this`; reaching the
    //  same bytes through a `char*` cast costs an extra `mov ecx,eax`.
    CAgileTypeAllocator* m_allocator;   // 0x70
    char            m_reserved74[4];    // 0x74 - still untouched
    unsigned int    m_parent;       // 0x78  type id, not a pointer
};

//----------------------------------------------------------------------------
//  The registry itself, a second instantiation of the same table.
//----------------------------------------------------------------------------
class AGILE_TYPE_TABLE : public TSHashTable<AGILE_TYPE_DATA, AGILE_TYPE_ID>
{
public:
    virtual ~AGILE_TYPE_TABLE();
    virtual AGILE_TYPE_DATA* AllocNode(TSExplicitList<AGILE_TYPE_DATA>* bucket,
                                       void* arg1, void* arg2);
};

//: 0x6F471910 - the entry point everything else calls.
int __fastcall AgileTypeIsDerivedFrom(unsigned int type, unsigned int base);

//: 0x6F471BC0 - see agiletype.cpp for why this leaf exists at all.
unsigned int GetAgileRootTypeId();

#endif
