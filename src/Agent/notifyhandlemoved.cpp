//============================================================================
//  0x6F474830 - resolve a handle to its live object and tell it where its
//  owner now is.
//
//  Own translation unit: the LookupHandle call (0x6F03FA30, handletable.cpp)
//  is real.
//
//  `this` is the small handle-bearing sub-object widget.cpp already reaches
//  through CWidget's vtable[0xB8] - handle at +0x08, type at +0x0C, the
//  same SHandleWithType shape timesyncbounds.h describes.  Both remaining
//  calls go through the *resolved* object's own vtable, not the widget's:
//  slot 0x4C takes the new facing by address, and slot 0x58 is a
//  no-argument "and now republish yourself" that only runs when the caller
//  asks for it.
//
//  LookupHandle returning null is not checked here - the shipped code
//  dereferences the result unconditionally.  That is faithful, not an
//  oversight in the transcription: its one caller only reaches this after
//  CWidget::InitSprite has already established the object.
//============================================================================
#include "handlemoved.h"
#include "game.h"   // LookupHandle, declared once - a local re-declaration
                     // with a different return type or parameter type mangles
                     // differently and silently stops resolving (see
                     // docs/notes/verifier-gate-link-failures.md, cause 2).

typedef void (__thiscall *SetFacingFn)(void*, const CFloat*);
typedef void (__thiscall *RepublishFn)(void*);

void SHandleWithType::NotifyHandleObjectMoved(const CFloat* facing,
                                              int republish)
{
    CHandleObject* object = LookupHandle(m_handle, m_typeTag);

    ((SetFacingFn)(*(void***)object)[0x4C / 4])(object, facing);
    if (republish)
        ((RepublishFn)(*(void***)object)[0x58 / 4])(object);
}
