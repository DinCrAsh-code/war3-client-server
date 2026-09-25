//============================================================================
//  0x6F333290 - the rendering-side gate CItem::SetPending (item_pending.cpp)
//  pokes on its way down.
//
//  Was a `mov eax,<address> / jmp eax` redirect; it is twelve instructions,
//  and the same shape as RefreshSelectionOverlay
//  (selectionoverlayrefresh.cpp): fetch the CGameUI singleton with
//  (0, 0) - do not create it, do not clear the cached pointer - and, if
//  there is one, hand `this` to one of its sub-objects.
//
//  The sub-object is the one the UI parks at +0x3BC, not the +0x284 word
//  the selection refresh ORs into, so the two gates reach different parts
//  of the same singleton.
//
//  Own translation unit: two real calls out of it.
//============================================================================

//  0x6F300710 - the lazily-constructed CGameUI singleton, behind an
//  __except_handler4-shaped SEH frame this toolchain cannot reproduce; it
//  stays a redirect in selectable_attachcreate.cpp and is only declared
//  here.
struct SGameUI
{
    char          m_reserved00[0x2A0];
    unsigned int  m_uiUp;           // +0x2A0
    unsigned int  m_circlesLit;     // +0x2A4
};
SGameUI* __fastcall GetGameUI(int create, int reset);

//----------------------------------------------------------------------------
//  0x6F38DD50 - "this item is going away: drop every render record that
//  names it".  A 0xC0-byte, 0x18-stride sweep over the sub-object's own
//  eight-entry table; reconstructed in itemrendertable.cpp and declared,
//  not defined, here so this call site reaches it with a real call.
//----------------------------------------------------------------------------
struct SItemRenderTable
{
    void __thiscall DropItemRecords(void* item);
};

struct SItemRenderGate
{
    void OnPending();
};

void SItemRenderGate::OnPending()
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SItemRenderTable**)ui)[0x3BC / 4]->DropItemRecords(this);
}
