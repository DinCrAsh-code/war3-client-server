//============================================================================
//  0x6F26BB20 - CWar3Image::Load, the out-of-line first half of
//  CWidget::Load (vtable slot 15, widget_load.cpp).  See widget.h.
//
//  CWar3Image::Save's counterpart (widget_saveheader.cpp) and not quite its
//  mirror: the base-class load, then the sprite's own - which needs the
//  three model tokenizers and the detail flags handed to it, where the save
//  side needed none of that - then m_reserved24 read straight back.
//
//  `GetModelDetailFlags(1)`, not `(0)`: the load path asks for full detail
//  where CWar3Image::SetModelByName (widget_setmodel.cpp) asks for none.
//
//  Own translation unit: seven real calls.
//============================================================================
#include "widget.h"
#include "cdatastore.h"
#include "modeltokens.h"

int __fastcall GetModelDetailFlags(int wantsFullDetail);

//  0x6F4D9940 - the sprite's own load.  A large function with its own SEH
//  frame in the sprite module; out of scope, redirected.  __fastcall with
//  the store in edx and four stack arguments, read off the shipped
//  `lea ecx,[esi+28h]` / `mov edx,edi` pair and its own `retn 10h`.
__declspec(naked) void __fastcall LoadSprite(CSprite* sprite,
                                              CDataStore* store,
                                              CTokenizer* animTokens,
                                              CTokenizer* attachmentTokens,
                                              CTokenizer* boneTokens,
                                              int detailFlags)
{
    __asm
    {
        mov     eax, 06F4D9940h
        jmp     eax
    }
}

void CWar3Image::Load(CDataStore* store)
{
    CAgentWar3::Load(store);

    LoadSprite(m_pSprite, store,
               GetAnimationTokenizer(),
               GetAttachmentTokenizer(),
               GetBoneTokenizer(),
               GetModelDetailFlags(1));

    store->ReadDwordAlt(&m_reserved24);
}
