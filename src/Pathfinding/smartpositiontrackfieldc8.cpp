//============================================================================
//  0x6F476370 - SmartPositionTrack's vtable slot 7.
//
//  NOT the same field PositionTrack::GetField8C reads (0x6F474810,
//  positiontrackfield8c.cpp): the raw disassembly reads
//  `mov ecx, [eax+0C8h]`, offset +0xC8, not +0x8C - a different, new field
//  (CPathTrace::m_fieldC8, game.h) this override alone reaches. Same
//  three-instruction shape otherwise (resolve the handle, hand the
//  resolved CPathTrace's own field back through the out-pointer argument).
//
//  DIFFERS, not EXACT: register-swapped only (the resolved-handle result
//  and the out-pointer are kept in the opposite registers from the
//  shipped code, same three instructions in the same order) - pending
//  angr IDENTICAL verification.
//
//  Own translation unit: 0x6F476370 is a different module from 0x6F474810
//  (positiontrackfield8c.cpp, PositionTrack's own slot 7) and from
//  0x6F476470 (smartpositiontrackspeed.cpp, this class's own slot 6) - the
//  two make no calls into each other.
//============================================================================
#include "positiontrack.h"

void SmartPositionTrack::GetFieldC8(void** out)
{
    *out = ((CPathTrace*)LookupHandle(m_handle, m_typeTag))->m_fieldC8;
}
