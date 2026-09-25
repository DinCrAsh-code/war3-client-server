//============================================================================
//  The agent enumerators (0x6F467570, 0x6F47Exxxx/0x6F47Fxxxx) and the cell
//  collector under them (0x6F47Dxxxx).
//
//  "Enumerate every agent within `radius` of (x, y)" is done in two halves.
//  The collector - AgileAgentQuery below - turns a world circle into the
//  half-open cell rectangle covering it, walks every registration chained
//  into those cells, and appends the ones whose classification the query
//  does not exclude.  The enumerator then walks the collected set, applies
//  the real circular distance test (the cell rectangle is only a
//  conservative superset), and hands each survivor to the caller's
//  callback.
//
//  Six enumerator bodies exist because two independent choices are made in
//  the caller and neither is a run-time branch inside the leaf: whether an
//  agent's own collision radius counts towards "within" (0x6F47F1F0 family)
//  or only its centre does (0x6F47E9E0 family), and whether the type filter
//  is absent, an exact agile type id, or an "is derived from" test.
//  0x6F467570 and the two dispatchers below make those choices.
//============================================================================
#ifndef AGENTQUERY_H
#define AGENTQUERY_H

#include "game.h"
#include "spatialgrid.h"
#include "agenteventbinding.h"

class CAgent;   //  agent.h

//----------------------------------------------------------------------------
//  What a CPathTrace keeps at +0x30, and what the agile type registry's own
//  per-type instance list (0x6F46D0B0, EnumerateUnitsOfType's worker) is a
//  list of: the registration standing for one live agent.
//
//  Both walks refuse to visit a node whose +0x20 is non-zero and both hand
//  the callback its +0x54, so those are the two members named.  Nothing in
//  either says what +0x20 counts, only that a non-zero one is not a
//  candidate.
//----------------------------------------------------------------------------
struct SAgileAgent
{
    char          m_reserved00[0x08];
    //  +0x08 - the next registration of the same agile *type*.  The type
    //  registry's own per-type instance list is threaded through this word
    //  and nothing else in this repo reaches it; the spatial enumerators
    //  find their candidates through the grid instead.  See
    //  src/Agent/agiletypeinstances.cpp.
    SAgileAgent*  m_nextOfType;   // +0x08
    char          m_reserved0C[0x20 - 0x0C];
    int           m_retired;      // +0x20
    char          m_reserved24[0x54 - 0x24];
    CAgent*       m_agent;        // +0x54
};

//----------------------------------------------------------------------------
//  One collected candidate: the registration, and the classification word
//  that let it through.  Eight bytes - every walk over the collected set
//  indexes it as `[base + i*8]`.
//----------------------------------------------------------------------------
struct SAgentQueryHit
{
    CGridRegistration* m_reg;         // +0x00
    unsigned int       m_classBits;   // +0x04
};

//----------------------------------------------------------------------------
//  The collector.  A CMemBlock-backed array of SAgentQueryHit (the same
//  block-plus-chunk-plus-capacity-plus-count shape CCellRecordArray has)
//  that remembers which rectangle of which grid it was last filled for, so
//  that re-asking the same question costs nothing.
//
//  The RTTI name in the pool's own Storm allocation tag is
//  ".PAVAgileAgentQuery@@", so this class's name is the shipped one and not
//  a reconstruction of it.
//----------------------------------------------------------------------------
class AgileAgentQuery : public CMemBlock
{
public:
    //  0x6F47D480 - store *value into every slot from `start` up to, but not
    //  past, min(m_count, start + count).  The sibling of
    //  CCellRecordArray::FillRecords, instruction for instruction.
    void FillEntries(unsigned int start, const SAgentQueryHit* value,
                     unsigned int count);

    //  0x6F47D810 - make room for `count` more entries, growing the block in
    //  whole growth chunks, then fill them with *value.
    int AppendEntries(const SAgentQueryHit* value, unsigned int count);

    //  0x6F46AC70 - drop `count` entries starting at `start`, sliding what
    //  follows down over them.  The only caller passes (0, m_count), which
    //  makes it "forget everything collected"; it is still spelled as the
    //  general erase the shipped body is.
    int RemoveEntries(unsigned int start, unsigned int count);

    //  0x6F47D8A0 - walk one cell's registration chain and append every
    //  candidate in it this query does not exclude.
    void CollectCell(CCellGrid* grid, int cell, int stamp);

    //  0x6F47DA10 - clip `box` to the grid's own extent and CollectCell()
    //  every cell left of it.
    void CollectBox(CCellGrid* grid, const SCellBox* box);

    //  0x6F47DAD0 - forget what is collected and collect again, for
    //  whichever rectangle and grid the query is currently pointed at:
    //  m_anchor's own, when it has one, and its explicit pair otherwise.
    void Refresh();

    //  0x6F46EBE0 - AgileAgentQuery's own vtable slot 0 (its only slot -
    //  Class Informer lists it a 1-method class,
    //  agent_worktrees/classes/0x6F951A28.json).  Real base per the RTTI
    //  hierarchy is NIpse::CPmQuery<AgileAgentFilter>, not modelled as a
    //  real C++ base here any more than CMemBlockT<T>/CDynTable<T> are
    //  above - see Misc/ntempest_cpmquery_scalardtors.cpp's own
    //  CPoQuery/CSeQuery for the identical "restamp the immediate base's
    //  own vtable literal, because this class never got its own
    //  vtable-stamping constructor reconstructed" shape this reuses
    //  verbatim, one level up: this slot restamps
    //  CPmQuery<AgileAgentFilter>'s own vtable
    //  (??_7?$CPmQuery@VAgileAgentFilter@@@NIpse@@6B@, 0x6F951844) rather
    //  than a distinct AgileAgentQuery vtable literal, then - if any
    //  entries are collected - clears them via the freestanding
    //  RemoveEntries body directly (not the virtual dispatch, not
    //  Clear()'s own `if (m_count != 0)` phrasing: the shipped code tests
    //  `[esi+1Ch]` into eax first and pushes that same eax as the count,
    //  which optimises identically to `RemoveEntries(0, m_count)` guarded
    //  the same way but is written that way here to keep the local
    //  hoisted the same way the dump does), releases the CMemBlock
    //  allocation via ReleaseCMemBlockBase, and conditionally frees
    //  `this` - the same three-part shape
    //  CPmQuery_AgileAgentFilter::ScalarDeletingDestructor already has.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  The three inline setters the enumerators reach it through.  Each
    //  throws the collected set away, because it is about to be answering a
    //  different question; SetBox does so unconditionally where the other
    //  two only do it when the value actually changes, which is what the
    //  shipped code's own branch structure says.
    void Clear()
    {
        if (m_count != 0)
            RemoveEntries(0, m_count);
    }

    void SetAnchor(CGridRegistration* anchor)
    {
        if (m_anchor != anchor)
        {
            Clear();
            m_anchor = anchor;
        }
    }

    void SetBox(const SCellBox& box)
    {
        Clear();
        m_box = box;
    }

    void SetGrid(CCellGrid* grid)
    {
        if (grid != m_grid)
        {
            Clear();
            m_grid = grid;
        }
    }

    unsigned int       m_growChunk;    // +0x14
    unsigned int       m_capacity;     // +0x18
    unsigned int       m_count;        // +0x1C
    //  +0x20 - when set, the registration whose own rectangle and grid the
    //  query follows instead of the explicit pair below.
    CGridRegistration* m_anchor;       // +0x20
    SCellBox           m_box;          // +0x24
    CCellGrid*         m_grid;         // +0x34
    char               m_reserved38[0x40 - 0x38];
    //  +0x40 - non-zero while an enumeration is running on this object.
    int                m_running;      // +0x40
    //  +0x44 - the classification bits that disqualify a candidate.
    unsigned int       m_excludeMask;  // +0x44
};

//----------------------------------------------------------------------------
//  The six leaves take the query radius in ecx and everything else on the
//  stack, so the radius is `this` - the same shape SUnitQueryCircleCentre
//  (agenteventbinding.h) already has for the centre object of the circle
//  form.  Only the first CFloat is ever read.
//----------------------------------------------------------------------------
struct SQueryRadius
{
    //  0x6F47E9E0 / 0x6F47EF40 / 0x6F47EC80 - the agent's centre must be
    //  inside the circle.
    void EnumerateInCircle(const int* x, const int* y, unsigned int mask,
                           UnitQueryVisitFn fn, void* ctx) const;
    void EnumerateInCircleOfType(const int* x, const int* y,
                                 unsigned int mask, unsigned int type,
                                 UnitQueryVisitFn fn, void* ctx) const;
    void EnumerateInCircleDerivedFrom(const int* x, const int* y,
                                      unsigned int mask, unsigned int base,
                                      UnitQueryVisitFn fn, void* ctx) const;

    //  0x6F47F1F0 / 0x6F47F7B0 / 0x6F47F4C0 - the agent's own collision
    //  circle need only touch it.
    void EnumerateTouchingCircle(const int* x, const int* y,
                                 unsigned int mask, UnitQueryVisitFn fn,
                                 void* ctx) const;
    void EnumerateTouchingCircleOfType(const int* x, const int* y,
                                       unsigned int mask, unsigned int type,
                                       UnitQueryVisitFn fn, void* ctx) const;
    void EnumerateTouchingCircleDerivedFrom(const int* x, const int* y,
                                            unsigned int mask,
                                            unsigned int base,
                                            UnitQueryVisitFn fn,
                                            void* ctx) const;

    CFloat m_radius;
};

//----------------------------------------------------------------------------
//  0x6F47FAF0 / 0x6F47FB50 - pick the type filter.  A `type` of 0 or -1 is
//  "any"; otherwise `derived` picks between the exact and the derived-from
//  test.  Both take the two centre coordinates in ecx and edx because
//  0x6F467570 does and hands them straight on.
//----------------------------------------------------------------------------
void __fastcall EnumerateAgentsInCircle(const int* x, const int* y,
                                        const int* radius, unsigned int mask,
                                        unsigned int type, int derived,
                                        UnitQueryVisitFn fn, void* ctx);
void __fastcall EnumerateAgentsTouchingCircle(const int* x, const int* y,
                                              const int* radius,
                                              unsigned int mask,
                                              unsigned int type, int derived,
                                              UnitQueryVisitFn fn,
                                              void* ctx);

#endif
