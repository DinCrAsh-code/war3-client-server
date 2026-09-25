//============================================================================
//  AuFixedString/AuFixedStringArray - pulled out of fontfileversionarray.cpp
//  (which still owns the one explicit TSFixedArray<AuFixedString>
//  instantiation, SetAlloc @ 0x6F553B60 / SetCount @ 0x6F553C50) so a
//  second, unrelated caller can share the same type instead of standing up
//  a second, redundant TSFixedArray<T> instantiation at a *new* address for
//  what is really the same shipped template body.  cnd-wave2-c's own
//  ReadTeamGameListTailField_6F653760 (Net/netevent_dispatch.cpp) reaches
//  0x6F553C50 directly (its dump's own `call sub_6F553C50`), so it needs
//  this exact type, not a lookalike - see storm.h for the TSFixedArray<T>
//  template declaration this header's `typedef` instantiates the
//  *declaration* of (the out-of-line bodies stay solely in
//  fontfileversionarray.cpp; nothing here re-includes tsfixedarray.inl).
//============================================================================
#ifndef AUFIXEDSTRING_H
#define AUFIXEDSTRING_H

#include "storm.h"   // TSFixedArray<T>

//  0x20-byte fixed-size string slot.  Only byte 0 is ever touched at this
//  level (SetCount's grow arm default-constructs a freshly added slot as an
//  empty string).
struct AuFixedString
{
    AuFixedString() { m_data[0] = 0; }

    char m_data[0x20];
};

typedef TSFixedArray<AuFixedString> AuFixedStringArray;

#endif
