//============================================================================
//  LookupAbilityFieldRecord's real machinery: a second, allocating
//  per-type-id cache CAbility's field accessors (ability_fieldart.cpp)
//  fall back to when the fast instance cache (CAbility::m_field54,
//  refreshed through abilitylevelfield.cpp's own
//  LookupAbilityTypeRecord/0x6F265ED0) comes up empty.
//
//  The table/node shape (g_abilityFieldRecordTable, dword_6FAB58FC) is
//  shared with a sibling CAbility depth<=5 closure batch and lives in
//  abilityfieldrecordtable.h - both batches reached the same real
//  singleton from different call chains and it is one object, not two.
//============================================================================
#ifndef ABILITY_TYPEFIELDRECORD_H
#define ABILITY_TYPEFIELDRECORD_H

#include "abilityfieldrecordtable.h"

//----------------------------------------------------------------------------
//  0x6F32C8E0 - find or create the record for typeId.  ability_slots.cpp's
//  own naked redirect used to stand in for this; the real body is
//  ability_typefieldrecord.cpp.
//
//  debugTag is not a field-name selector - every caller in
//  ability_fieldart.cpp gets back the *same* per-typeId record and reads
//  its own hard-coded offset out of it, so the record is not keyed on
//  this string at all.  It rides through unread by this function itself:
//  it stays in edx from the caller all the way into AllocNode's own
//  vtable slot (Storm's SMemAlloc logfilename convention, Storm/storm.h)
//  on the not-found path, which is why the walk in
//  ability_typefieldrecord.cpp never touches it directly.
//  __fastcall(typeId in ecx, debugTag in edx), `retn 0`.
//----------------------------------------------------------------------------
void* __fastcall LookupAbilityFieldRecord(unsigned int typeId, const char* debugTag);

#endif
