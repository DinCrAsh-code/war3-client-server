//============================================================================
//  The per-order movement wrapper (0x6F493xxx / 0x6F495320).
//
//  Not CPathClient (pathmove.h) despite the superficially similar offsets:
//  this object *holds* a CPathClient at +0xA8 and hands it to
//  CPathClient::SetTargetNode/SetRadius, where CPathClient's own +0xA8 is a
//  plain handle.  It is the thing one movement order exists as for as long as
//  the order lasts, and it links itself into a list rooted off the time-sync
//  singleton.
//
//  Only the two fields this pair of accessors touches are named; the rest of
//  the object is not recovered yet.  See docs/targets/
//  CPathClient_movement_pathfinder.md.
//============================================================================
#ifndef PATHORDER_H
#define PATHORDER_H

#include "game.h"

//----------------------------------------------------------------------------
//  The list the orders thread themselves onto, rooted off the time-sync
//  singleton (g_pTimeSync+0x438).  The anchor and the members are the same
//  shape, which is why the insert below takes its `this` as one of them.
//----------------------------------------------------------------------------
struct SPathOrderLink
{
    //  0x6F493810 - splice `node` in immediately after this one.
    void InsertAfter(SPathOrderLink* node);

    char            m_reserved00[0x04];
    SPathOrderLink* m_prev;     // +0x04
    SPathOrderLink* m_next;     // +0x08
};

class CPathOrder
{
public:
    //  0x6F493E80 / 0x6F493E90 set and clear bit 0x10000 of m_flags, one
    //  instruction each.  They are reconstructed as the free functions
    //  SetFlag0x10000_6F493E80 / ClearFlag0x10000_6F493E90 in
    //  trivialaccessors_04.cpp rather than as members here - which is what
    //  the object they operate on turns out to be is recorded for, not a
    //  second definition of them.
    char         m_reserved00[0xD8];
    unsigned int m_flags;       // +0xD8
};

#endif
