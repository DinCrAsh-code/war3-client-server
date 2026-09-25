//============================================================================
//  0x6F4C4630 / 0x6F4C4660 - see handleobject.cpp.  Declared in a header so
//  the definition and CWidget::CreateSpriteFromDescriptor's call site cannot
//  drift apart into two mangled names.
//============================================================================
#ifndef HANDLEOBJECT_H
#define HANDLEOBJECT_H

struct SHandleHolder
{
    char m_reserved00[0x08];
    struct SHandleOwner
    {
        char         m_reserved00[0x1C];
        unsigned int m_handle;     // +0x1C
    }* m_pOwner;                   // +0x08
};

unsigned int __fastcall GetHandleOrZero(const SHandleHolder* self);
unsigned int __fastcall GetHandleOrZeroAlias(const SHandleHolder* self);

#endif
