//============================================================================
//  0x6F02E130 - CAbility's own vtable slot 111.  Formats this ability's
//  own m_typeId (ability.h, +0x34 - the rawcode, confirmed independently
//  by unitinventory.h's CAbilityInventory::m_typeId at the same offset)
//  into the caller's buffer through UnpackFourCCString
//  (Game/objectdatatable.h, 0x6F4C3F80) - the same shared unpacker
//  CObjectDataTable::HasRow/AcquireRow already re-key through.
//
//  `this` arrives in ecx and is read once (m_typeId) before being
//  discarded - the dump reloads it into ecx as UnpackFourCCString's own
//  first (`__fastcall`) argument rather than keeping `this` live, so the
//  source reads the field into a local instead of calling through `this`
//  a second time; declaring the parameter order fourCC-then-buffer here
//  (matching UnpackFourCCString's own signature) reproduces exactly that
//  reload.  `size` is passed on but never read on either side - matching
//  UnpackFourCCString's own unused third argument - and the return value
//  is the caller's buffer pointer, not UnpackFourCCString's own (unused)
//  int result.
//============================================================================
#include "ability.h"
#include "objectdatatable.h"

char* CAbility::GetTypeIdString(char* buffer, int size)
{
    UnpackFourCCString(m_typeId, buffer, size);
    return buffer;
}
