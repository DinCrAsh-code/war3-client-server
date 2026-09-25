//============================================================================
//  0x6F35AA20 - resolve this object's own handle (+0x18) through the
//  global handle table, confirm the object it names still carries the
//  expected `m_kindTag` (Game/game.h's own CHandleObject, +0x0C - the
//  fixed FourCC 0x2B61676C check that file already documents from
//  jasssetitemposition_native.cpp), and hand back `SItemHandleObject`'s own
//  +0x54 field.  `retn 0`, thiscall, no other argument, no push/pop - a
//  pure leaf.
//
//  LookupHandle (Agent/handle.cpp) already does its own type-tag check
//  against whatever `typeTag` it is handed (+0x1C here); the `m_kindTag`
//  check is not redundant from the compiler's own point of view - it
//  cannot prove the two agree - so it survives as a second, real
//  comparison.
//
//  Reached from CItem-batch-3's own sub_6F35AAE0
//  (GameUI/selectionidentitynotify.cpp) and from many addresses outside
//  this batch's own dump; the receiver's own real class is not
//  established by anything this dump reaches, so it stays a minimal local
//  view, the same convention GameUI/gameuinotifyleaf.cpp already uses for
//  the same reason.
//============================================================================
#include "game.h"

//  Agent/handle.cpp - no header of its own; declared here to match its
//  mangled signature exactly.
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

struct SHandleTaggedFieldHost
{
    char         m_reserved00[0x18];
    unsigned int m_handle;      // +0x18
    int          m_typeTag;     // +0x1C
};

void* __fastcall GetSelectionHandleTaggedField(SHandleTaggedFieldHost* self)
{
    CHandleObject* object = LookupHandle(self->m_handle, self->m_typeTag);
    if (object && object->m_kindTag == 0x2B61676C)
        return ((SItemHandleObject*)object)->m_field54;
    return 0;
}
