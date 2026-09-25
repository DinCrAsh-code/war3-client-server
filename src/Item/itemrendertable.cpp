//============================================================================
//  0x6F38DD50 - "this item is going away: drop every render record that
//  names it".
//
//  Was a thunk in itemrendergate.cpp, declared there so
//  SItemRenderGate::OnPending could reach the real code.  Thirty-four
//  instructions over a fixed eight-entry table, and both things it calls are
//  already here: AttachmentRelease (0x6F00D970, stormattachmentgates.cpp)
//  and the inline TRefCnt release every counted object in this binary uses.
//
//  The table is eight 24-byte entries at +0x378, walked by *byte* offset -
//  the shipped loop counts `edi` from 0 to 0xC0 in steps of 0x18 and adds it
//  to a freshly reloaded base each iteration, so the base is a member access
//  inside the loop body and not a hoisted local.  An index-and-scale loop
//  (`for (int i = 0; i < 8; i++)`) reproduces neither.
//
//  Each matching entry is torn down in two halves, and the second half
//  re-reads +0x04 after the first has run: the AttachmentRelease call in
//  between reaches the whole visual-attachment subsystem, so the compiler
//  cannot carry the value across it - and neither should the source.
//
//  The reference drop is written out inline rather than as a
//  `TRefCnt::Release()` call because the shipped code has it inline: an
//  `add [ecx+4],-1` / `jnz` pair and a direct slot-0 dispatch, with nothing
//  pushed.
//
//  Own translation unit: OnPending reaches it with a real call and it makes
//  a real call of its own.
//============================================================================

void __fastcall AttachmentRelease(unsigned int slot);

//  Slot 0 - the same "you are at zero, destroy yourself" slot
//  TRefCnt::Release (refcnt.h) tail-jumps to, invoked with nothing pushed.
typedef void (__thiscall* Slot0Fn)(void* self);

//----------------------------------------------------------------------------
//  One entry.  Only the two fields the sweep touches are named; the twelve
//  bytes after them are whatever else the renderer keeps per record.
//----------------------------------------------------------------------------
struct SItemRenderEntry
{
    int   m_attachment;   // +0x00 - a visual-attachment slot, or -1
    void* m_pItem;        // +0x04 - counted; +0x00 vtable, +0x04 refcount
    char  m_reserved08[0x18 - 0x08];
};

struct SItemRenderTable
{
    void __thiscall DropItemRecords(void* item);

    char              m_reserved00[0x378];
    SItemRenderEntry* m_entries;      // +0x378
};

void __thiscall SItemRenderTable::DropItemRecords(void* item)
{
    for (unsigned int offset = 0; offset < 0xC0; offset += 0x18)
    {
        SItemRenderEntry* entry =
            (SItemRenderEntry*)((char*)m_entries + offset);

        if (entry->m_pItem != item)
            continue;

        if (entry->m_attachment != -1)
            AttachmentRelease((unsigned int)entry->m_attachment);
        entry->m_attachment = -1;

        //  Re-read, not the value compared above - see the header comment.
        void* held = entry->m_pItem;
        if (held == 0)
            continue;

        if (--((int*)held)[1] == 0)
            ((Slot0Fn)(*(void***)held)[0])(held);
        entry->m_pItem = 0;
    }
}
