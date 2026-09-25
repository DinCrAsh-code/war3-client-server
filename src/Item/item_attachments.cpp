//============================================================================
//  0x6F2B6450 and 0x6F2B63D0 - CItem's vtable slots 108 and 109
//  (+0x1B0/+0x1B4).  See item.h.
//
//  The same fifty instructions twice over with one flag inverted and one
//  different helper - two functions rather than one with a parameter, and
//  the linker left both standing because this image has no /OPT:ICF
//  (agentdefaults.cpp) and they are not identical anyway.
//
//  Each walks m_position's own twelve-byte attachment-ref array
//  *backwards*: the shipped code forms `this + (count*3 + 0x27)*4`, which
//  is `&m_position.m_reserved9C + count*12` - one past the end - and then
//  steps down by twelve at the top of the loop.  Written forwards it comes
//  out as an indexed walk with a scaled address at every iteration; written
//  backwards from a one-past-the-end pointer it is the shipped
//  `sub edi,0Ch` / `sub ebx,1` pair.  The count is read once, up front, and
//  the whole loop is skipped when it is zero.
//
//  An entry is live when `m_handle & m_typeTag` is not all-ones - the same
//  "-1 in either means unset" test itemhandleresolve.h documents and
//  position.h's own m_placementRef comment predicted.
//
//  Own translation unit: four real calls per loop body.
//============================================================================
#include "item.h"
#include "itemhandleresolve.h"

//  The array element: an SOptionalHandleRef with a third word after it that
//  nothing in these three vtables reads - the stride is twelve bytes, not
//  eight, which is what `sub edi,0Ch` fixes.
struct SItemAttachmentRef
{
    SOptionalHandleRef m_ref;        // +0x00
    unsigned int       m_reserved08; // +0x08
};

//  0x6F052BC0 / 0x6F052B60 - the two halves of the attachment reference
//  counter, thiscall members of whatever QueryHandleField0x54 hands back.
//  Reconstructed in attachmentcounters.cpp; declared, not defined, here so
//  both call sites reach them with real calls.
//
//  Both arguments are `int`, not the `void*` pair this file declared
//  before the bodies were read: the first is only ever tested against zero
//  and the second is added to and subtracted from a counter.  The two
//  casts at the call sites below are the seam - this function's own
//  parameters are still the guessed `void*` pair, because they also travel
//  into two vtable slots whose signatures nothing here has read.
struct SAttachmentTarget
{
    void Show(int alsoTotal, int delta);
    void Hide(int alsoTotal, int delta);
};

//  vtable slots +0xAC and +0xB0 on that same object, each taking the flag
//  plus the caller's two arguments.
typedef void (__thiscall *AttachmentSlotFn)(void* self, int on, void* context,
                                            void* arg);

void CItem::SetAttachmentsOn(void* context, void* arg)
{
    unsigned int remaining = m_position.m_reserved98;
    if (remaining == 0)
        return;

    SItemAttachmentRef* entry =
        (SItemAttachmentRef*)&m_position.m_reserved9C + remaining;
    do
    {
        entry--;
        remaining--;
        if ((entry->m_ref.m_handle & (unsigned int)entry->m_ref.m_typeTag)
                != 0xFFFFFFFFu)
        {
            SAttachmentTarget* target =
                (SAttachmentTarget*)QueryHandleField0x54(&entry->m_ref);
            if (target != 0)
            {
                target->Show((int)(size_t)context, (int)(size_t)arg);
                ((AttachmentSlotFn)(*(void***)target)[0xAC / 4])(
                    target, 1, context, arg);
                ((AttachmentSlotFn)(*(void***)target)[0xB0 / 4])(
                    target, 1, context, arg);
            }
        }
    }
    while (remaining != 0);
}

void CItem::SetAttachmentsOff(void* context, void* arg)
{
    unsigned int remaining = m_position.m_reserved98;
    if (remaining == 0)
        return;

    SItemAttachmentRef* entry =
        (SItemAttachmentRef*)&m_position.m_reserved9C + remaining;
    do
    {
        entry--;
        remaining--;
        if ((entry->m_ref.m_handle & (unsigned int)entry->m_ref.m_typeTag)
                != 0xFFFFFFFFu)
        {
            SAttachmentTarget* target =
                (SAttachmentTarget*)QueryHandleField0x54(&entry->m_ref);
            if (target != 0)
            {
                target->Hide((int)(size_t)context, (int)(size_t)arg);
                ((AttachmentSlotFn)(*(void***)target)[0xAC / 4])(
                    target, 0, context, arg);
                ((AttachmentSlotFn)(*(void***)target)[0xB0 / 4])(
                    target, 0, context, arg);
            }
        }
    }
    while (remaining != 0);
}
