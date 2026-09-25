#ifndef PLAYERWAR3_RELEASEHELPER_H
#define PLAYERWAR3_RELEASEHELPER_H

//============================================================================
//  0x6F02CA60 - shared by CPlayerWar3::Deactivate (playerwar3_deactivate.cpp,
//  vtable slot 13) and, per the destructor dump nobody has reconstructed
//  yet, CPlayerWar3's own destructor over five of the same seven fields.
//  Given the address of a pointer-valued field:
//    - if the field is non-null, call the pointee's own vtable slot 2
//      (+0x08) with no arguments beyond `this` (a notify/deactivate-shaped
//      call, *not* CObserver::Dispatch, which needs three stack args - this
//      object is not necessarily a CObserver);
//    - reload the field (the slot-2 call can have cleared it);
//    - if still non-null, decrement its refcount at +0x04 and, once it
//      hits zero, call vtable slot 0 (a ReleaseSelf-shaped call);
//    - unconditionally null the field.
//
//  ecx carries the *address of the field* (a `void**`), not the pointee -
//  a `__fastcall` free function taking one pointer argument, `retn 0`.
//  The pointee's own class could not be pinned down within this session's
//  scope (no dump names its vtable or constructor), so both calls are
//  raw indirect dispatches through the pointee's own vtable rather than a
//  named class, matching CLAUDE.md's own house style for an
//  unidentified-class hand-written slot dispatch.
//============================================================================

void __fastcall CPlayerWar3_ReleaseAndClearSlot(void** field);

#endif
