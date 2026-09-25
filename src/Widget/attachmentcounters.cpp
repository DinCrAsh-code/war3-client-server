//============================================================================
//  0x6F052BC0 and 0x6F052B60 - the "one more"/"one fewer" halves of the
//  attachment reference counters CItem's slots 108 and 109 drive
//  (item_attachments.cpp).
//
//  Both were `mov eax,<address> / jmp eax` redirects.  Neither is
//  rendering-side bookkeeping in the sense that comment claimed: they are a
//  pair of counters at +0x3C and +0x40 with an edge test on the first one,
//  and everything they do on the edge goes out through the object's own
//  vtable.
//
//  Read them as a matched pair and the shape is obvious:
//
//    Hide  adds `delta` to +0x3C and fires when the count crosses *up*
//          through zero  (was <= 0, now > 0);
//    Show  subtracts it and fires when the count crosses *down* through
//          zero (was > 0, now <= 0).
//
//  The names are the ones item_attachments.cpp already reaches them under
//  and are kept for that reason; the direction each actually moves the
//  counter is in the code.
//
//  Two differences beyond the sign, both in the shipped code:
//
//   * Hide touches the total at +0x40 *after* the edge work and only when
//     its flag argument is set; Show touches it *before*, under the same
//     flag.  The two are not mirror images.
//   * Show's edge path consults an owner object - `this+0x30` if it is
//     there, otherwise whatever 0x6F472890 hands back - and only proceeds
//     when bit 2 of that owner's +0x5C is set.  Hide has no such test.
//
//  Both parameters are `int`, not the `void*` pair the redirect declared:
//  the first is only ever tested against zero and the second is added to
//  and subtracted from a counter.
//
//  Own translation unit: neither calls the other, both are in the same
//  0x6F052xxx module, and item_attachments.cpp must reach both with real
//  calls.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F021270 - the shared "I changed, re-evaluate me" notify both halves
//  end on.  Fifteen instructions in the 0x6F021xxx module reaching two more
//  unreconstructed helpers; out of scope.  `retn` with nothing on the
//  stack, so a thiscall member with no arguments pushes what it cleans.
//----------------------------------------------------------------------------
struct SAttachmentCounterNotify
{
    void __thiscall OnCountChanged();
};

__declspec(naked) void __thiscall SAttachmentCounterNotify::OnCountChanged()
{
    __asm
    {
        mov     eax, 06F021270h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F472890 - the owning object, resolved the slow way when +0x30 is
//  empty.  A real body now, in attachmentownerquery.cpp; declared here, on
//  the same class name, so this call site reaches it with a real `call`.
//----------------------------------------------------------------------------
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();
};

//  Every slot below takes nothing but `this`; the shipped call sites push
//  nothing and none of them adjusts esp afterwards.
typedef int  (__thiscall* SlotQueryFn)(void* self);
typedef void (__thiscall* SlotNotifyFn)(void* self);

struct SAttachmentTarget
{
    void Show(int alsoTotal, int delta);
    void Hide(int alsoTotal, int delta);

    char         m_reserved00[0x20];
    unsigned int m_flags;       // +0x20 - bit 1 tracks the edge state
    char         m_reserved24[0x30 - 0x24];
    void*        m_pOwner;      // +0x30 - may be null; see QueryOwner
    char         m_reserved34[0x3C - 0x34];
    int          m_count;       // +0x3C
    int          m_total;       // +0x40
};

//----------------------------------------------------------------------------
//  0x6F052B60 - add `delta`; fire on the upward crossing of zero.
//----------------------------------------------------------------------------
void SAttachmentTarget::Hide(int alsoTotal, int delta)
{
    if (((SlotQueryFn)(*(void***)this)[0xCC / 4])(this) == 0)
        return;

    //  The old value is read *before* the update, and the new one is read
    //  back out of the field afterwards rather than computed - that is the
    //  shipped `mov eax,[esi+3Ch]` / `add [esi+3Ch],edi` /
    //  `cmp [esi+3Ch],0` pair, not one register compared twice.
    int before = m_count;
    m_count += delta;

    if (before <= 0 && m_count > 0)
    {
        ((SlotNotifyFn)(*(void***)this)[0xE0 / 4])(this);
        m_flags &= ~2u;
        ((SlotNotifyFn)(*(void***)this)[0xC4 / 4])(this);
    }

    if (alsoTotal != 0)
        m_total += delta;

    ((SAttachmentCounterNotify*)this)->OnCountChanged();
}

//----------------------------------------------------------------------------
//  0x6F052BC0 - subtract `delta`; fire on the downward crossing of zero.
//----------------------------------------------------------------------------
void SAttachmentTarget::Show(int alsoTotal, int delta)
{
    if (((SlotQueryFn)(*(void***)this)[0xCC / 4])(this) == 0)
        return;

    if (alsoTotal != 0)
        m_total -= delta;

    int before = m_count;
    m_count -= delta;

    if (before > 0 && m_count <= 0)
    {
        ((SlotNotifyFn)(*(void***)this)[0xE4 / 4])(this);

        void* owner = m_pOwner;
        if (owner == 0)
            owner = ((SAttachmentOwnerQuery*)this)->QueryOwner();

        if ((((const char*)owner)[0x5C] & 4) != 0)
        {
            m_flags |= 2;
            ((SlotNotifyFn)(*(void***)this)[0xC8 / 4])(this);
        }
    }

    ((SAttachmentCounterNotify*)this)->OnCountChanged();
}
