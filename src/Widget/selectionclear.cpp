//============================================================================
//  0x6F41FA20 - the selection holder's own "let go of what I am holding".
//
//  Reached with a real `call` from SSelectionSet::Contains
//  (selectionholder.cpp), which is why it is not in that file.
//
//  Two passes over the same +0x1E4 field, not one: the shipped code reloads
//  it after the notify, because the notify is a virtual call that could
//  have cleared it.  Writing this as a single cached local costs the second
//  `mov ecx,[esi+1E4h]` and is a real behavioural difference, not a
//  cosmetic one - so the member access is repeated on purpose.
//============================================================================

struct CAgent;

//----------------------------------------------------------------------------
//  Only the two words this function reaches through: the vtable pointer and
//  the reference count immediately behind it.  The same +0x00 / +0x04 pair
//  every counted object in this binary has (refcnt.h).
//----------------------------------------------------------------------------
struct SSelectedAgent
{
    void** m_vtable;        // +0x00
    int    m_refCount;      // +0x04
};

//  Slot 0x194 - "your selection state changed".  Five stack arguments, all
//  of them constants at this one call site (`push -1 / push 1 / push 1 /
//  push 0 / push 0`, i.e. left to right 0, 0, 1, 1, -1).
typedef void (__thiscall* Slot0x194Fn)(void* self, int, int, int, int, int);
//  Slot 0 - the scalar deleting destructor every counted object here has.
typedef void (__thiscall* Slot0Fn)(void* self);

struct SSelectionSet
{
    void ClearSelection();

    char    m_reserved00[0x1E4];
    CAgent* m_pSelected;        // +0x1E4
};

void SSelectionSet::ClearSelection()
{
    if (m_pSelected != 0)
    {
        SSelectedAgent* held = (SSelectedAgent*)m_pSelected;
        ((Slot0x194Fn)held->m_vtable[0x194 / 4])(held, 0, 0, 1, 1, -1);
    }

    //  Re-read, not the local above - see the header comment.
    if (m_pSelected != 0)
    {
        SSelectedAgent* held = (SSelectedAgent*)m_pSelected;
        if (--held->m_refCount == 0)
            ((Slot0Fn)held->m_vtable[0])(held);
        m_pSelected = 0;
    }
}
