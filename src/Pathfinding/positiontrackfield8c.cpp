//============================================================================
//  0x6F474810 - PositionTrack's vtable slot 7, GetField8C.  Position's own,
//  a plain accessor: resolve the handle, hand the resolved CPathTrace's
//  own +0x8C (CPathTrace::m_field8C, game.h) back through the out-pointer
//  argument.
//
//  Own translation unit: shipped right after slot 6 in the same 0x6F474xxx
//  module (positiontrackspeed.cpp), but the two make no calls into each
//  other, so keeping them apart costs nothing and mirrors the address
//  grouping either way.
//
//  DIFFERS, not EXACT: the shipped tail keeps LookupHandle's own field read
//  in ecx and the out-pointer in eax; this build keeps them the other way
//  round - every mention of either register is spelled with the other one,
//  same three instructions in the same order otherwise. Tried several
//  equivalent source spellings (a named `CPathTrace*` local, a named
//  `void*` local for the field, folding the cast into one expression, an
//  explicit `void**` alias for `out`) and none changed the allocation;
//  register allocation only, pending angr IDENTICAL verification.
//============================================================================
#include "positiontrack.h"

void PositionTrack::GetField8C(void** out)
{
    *out = ((CPathTrace*)LookupHandle(m_handle, m_typeTag))->m_field8C;
}
