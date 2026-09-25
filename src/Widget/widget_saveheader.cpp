//============================================================================
//  0x6F26BAF0 - CWar3Image::Save, the out-of-line first half of
//  CWidget::Save (vtable slot 14, widget_save.cpp).  See widget.h.
//
//  The base class's own save, then the sprite's, then m_reserved24 - the
//  colour-slot word CWar3Image::StoreOwningPlayer writes
//  (widgetplayerstore.cpp), which is what makes this the widget's own
//  header rather than more of CAgentWar3's.
//
//  Own translation unit: three real calls, and its one caller reaches it
//  with a fourth.
//============================================================================
#include "widget.h"
#include "cdatastorescratch.h"

//  0x6F4D5F20 - the sprite's own save.  A large function with its own SEH
//  frame in the sprite module; out of this session's scope, redirected.
//  __fastcall with the store in edx, read straight off the shipped
//  `mov ecx,[esi+28h]` / `mov edx,edi` pair.
__declspec(naked) void __fastcall SaveSprite(CSprite* sprite,
                                              CDataStoreScratch* store)
{
    __asm
    {
        mov     eax, 06F4D5F20h
        jmp     eax
    }
}

void CWar3Image::Save(CDataStoreScratch* store)
{
    CAgentWar3::Save(store);
    SaveSprite(m_pSprite, store);
    store->WriteDwordAlt(m_reserved24);
}
