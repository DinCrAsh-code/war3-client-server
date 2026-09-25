//============================================================================
//  0x6F333500 - SItemPlaceGate::OnPlaced: tell the game UI a new item has
//  appeared, if there is a game UI.
//
//  The CGameUI lookup is made with `create` clear, so this never brings the
//  UI into existence - a headless or not-yet-started game simply skips the
//  notification, which is the null check.
//
//  Own translation unit: two real calls out of it.
//============================================================================
#include "item.h"

//  0x6F300710 - selectable_attachcreate.cpp's own redirect; re-declared
//  here, never re-thunked.
struct SGameUI
{
    char   m_reserved00[0x3BC];
    void*  m_pItemSink;     // +0x3BC
};
SGameUI* __fastcall GetGameUI(int create, int reset);

//  0x6F39A3C0 - the UI-side sink's own "an item appeared" method.  A
//  thiscall member with one stack argument; out of scope, redirected.
struct SItemSink
{
    void OnItemPlaced(void* item);
};

__declspec(naked) void SItemSink::OnItemPlaced(void*)
{
    __asm
    {
        mov     eax, 06F39A3C0h
        jmp     eax
    }
}

//  0x6F39A3F0 - the same sink's "this widget's player mask changed" method,
//  reached from 0x6F333520 the same way.  Reconstructed in
//  itemsinkmaskchanged.cpp; declared, not defined, here so this call site
//  reaches it with a real call.
struct SItemSink2
{
    void OnMaskChanged(void* agent);
};

struct SItemPlaceGate
{
    void OnPlaced();
    void OnMaskChanged();
};

void SItemPlaceGate::OnPlaced()
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SItemSink*)ui->m_pItemSink)->OnItemPlaced(this);
}

//----------------------------------------------------------------------------
//  0x6F333520 - the twin of OnPlaced above, one sink method along.  Shares
//  this file because neither calls the other.
//----------------------------------------------------------------------------
void SItemPlaceGate::OnMaskChanged()
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SItemSink2*)ui->m_pItemSink)->OnMaskChanged(this);
}
