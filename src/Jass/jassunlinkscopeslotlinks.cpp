//============================================================================
//  0x6F44B960 - unlink two embedded `TSLink<T>` members directly (at +0x0C
//  then +0x04), each through the same `TSLink<T>::Unlink()`
//  (Storm/storm.h) every other list in this binary already shares.
//  Reached only from sub_6F45A040 - the per-slot element destructor
//  JassInstance's own 256-entry embedded table array uses (see
//  jassinstance.h's own `m_reserved4C[0x2854 - 0x4C]` note) - so `this`
//  here is one of those 0x28-byte elements, and the two links unlinked are
//  its own all-nodes-list link and its own bucket link; the element's own
//  class is not established past this shape (neither list it threads onto
//  is reachable from this closure), so the two offsets stay raw rather
//  than named struct members - the same choice jassdeclhelpers.cpp's own
//  header comment makes for its own opaque object.
//============================================================================
#include "storm.h"

void __fastcall UnlinkScopeSlotLinks(void* obj)
{
    ((TSLink<void>*)((char*)obj + 0x0C))->Unlink();
    ((TSLink<void>*)((char*)obj + 0x04))->Unlink();
}
