//============================================================================
//  The two things the agile type registry's own per-type instance walk
//  (0x6F46D0B0) reaches that nothing else in this repo has named: the
//  object a type record lazily builds at its +0x6C, and the list of child
//  types hanging off the record itself at +0x48.
//
//  Both are overlays on AGILE_TYPE_DATA rather than members added to it
//  (agiletype.h): that class is reached by pointer everywhere and its
//  +0x44..+0x6B block is one reserved run today, but TSExplicitList has a
//  user-declared constructor and a private operator=, and giving
//  AGILE_TYPE_DATA one would change what the compiler is allowed to do
//  with the class in the dozen translation units that already include it.
//  The offsets are exact and each is established by the instruction that
//  reads it - the list by 0x6F46D0B0's `mov eax,[edx+48h]` /
//  `mov eax,[edx+50h]` pair, which is a TSExplicitList's link offset and
//  its terminator's prevlink, four bytes apart the way that template lays
//  them out.
//============================================================================
#ifndef AGILETYPEINSTANCES_H
#define AGILETYPEINSTANCES_H

#include "agiletype.h"
#include "agentquery.h"

//----------------------------------------------------------------------------
//  One child type of a type record: the child's own four-character id, and
//  the record for it once somebody has looked it up.  The cache is filled
//  in on first use and never cleared, exactly like AGILE_TYPE_DATA's own
//  +0x6C.
//
//  Where its list link sits is not recoverable: TSExplicitList carries the
//  offset at run time and the walk uses it, so only the two fields the walk
//  reads are named.
//----------------------------------------------------------------------------
struct SAgileChildType
{
    char             m_reserved00[0x18];
    unsigned int     m_typeId;      // +0x18
    AGILE_TYPE_DATA* m_cached;      // +0x1C
};

//----------------------------------------------------------------------------
//  What AGILE_TYPE_DATA::m_field6C points at, seen from the one angle this
//  walk gives: it holds the head of the type's live-instance list.  The
//  nodes are SAgileAgent (agentquery.h) threaded through their own +0x08,
//  which is the same class the spatial enumerators reach through a
//  CPathTrace's m_owner - and the same +0x20 / +0x54 pair they test and
//  hand on.
//----------------------------------------------------------------------------
struct SAgileTypeInstances
{
    char         m_reserved00[0x58];
    SAgileAgent* m_head;            // +0x58
};

//----------------------------------------------------------------------------
//  AGILE_TYPE_DATA's child-type list, by offset.
//----------------------------------------------------------------------------
struct SAgileTypeChildren
{
    char                           m_reserved00[0x48];
    TSExplicitList<SAgileChildType> m_children;   // +0x48
};

//  0x6F46D0B0 - visit every live instance of `type`, and then (when
//  `recurse` is set and nothing has asked to stop) every live instance of
//  each of its child types.  See src/Agent/agiletypeinstances.cpp.
void __fastcall EnumerateTypeInstances(AGILE_TYPE_DATA* type,
                                       UnitQueryVisitFn fn, void* ctx,
                                       CGameData* game, int recurse);

#endif
