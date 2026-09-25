//============================================================================
//  0x6F26B920 - CWidget's vtable slot 31 (+0x7C).  See widget.h.
//
//  The same shape as CWar3Image::CreateSpriteFromDescriptor (widgetsprite.cpp)
//  with the descriptor stripped out: make sure there is a sprite, and - if
//  the caller actually named a model - load it and put the sprite into the
//  same state that function does.
//
//  Two things differ from the descriptor version and are worth naming.  The
//  model arrives as a *string* in edx (LoadModelWithTokens' second
//  parameter, which widgetsprite.cpp reaches with a handle instead - the
//  callee takes whichever, since both are one dword), and the emptiness
//  test is on the string: null pointer *or* empty string means "no model".
//  The owning player passed on is a literal -1, not anything the caller
//  supplied; only the colour slot comes from the argument.
//
//  Own translation unit: every call here is real.
//============================================================================
#include "widget.h"
#include "sprite.h"
#include "itemplacementdispatch.h"
#include "modeltokens.h"

//  From widgetsprite.cpp's own declarations - same addresses, same shapes,
//  re-declared here rather than re-invented.  The three tokenizer getters
//  come from modeltokens.h, which already declares them.
void __fastcall ModelReleaseNotify();
CSprite* __fastcall CreateSpriteForDetailLevel(int detailLevel);
int __fastcall GetModelDetailFlags(int wantsFullDetail);
void __fastcall LoadModelWithTokens(CSprite* self, unsigned int modelHandle,
                                    CTokenizer* animTokens,
                                    CTokenizer* attachmentTokens,
                                    CTokenizer* boneTokens, int detailFlags,
                                    unsigned int player, void* notify);

//  vtable slots 35/36 (+0x8C/+0x90) - CWar3Image::PlayAnimation and
//  PlayAnimationLooping (widget_spriteanim.cpp).  Their answers are
//  discarded on the spot (`fstp st`), so they are called for the side
//  effect, exactly as widgetsprite.cpp already documents.
typedef float (__thiscall *QueryFloatFn)(void*, int, int, int);

void CWar3Image::SetModelByName(const char* modelName, unsigned int colourSlot)
{
    if (m_pSprite == 0)
        m_pSprite = CreateSpriteForDetailLevel(1);

    if (modelName == 0 || *modelName == 0)
        return;

    LoadModelWithTokens(m_pSprite, (unsigned int)modelName,
                        GetAnimationTokenizer(),
                        GetAttachmentTokenizer(),
                        0,
                        GetModelDetailFlags(0),
                        (unsigned int)-1,
                        (void*)&ModelReleaseNotify);

    StoreOwningPlayer((unsigned int)-1, colourSlot);

    SetItemPlacementFlagBits(m_pSprite, 0x3000000, 1);
    SetSpriteStateBits(m_pSprite, 7, 1);
    SetSpritePausedBit(m_pSprite, 1);

    ((QueryFloatFn)(*(void***)this)[0x8C / 4])(this, 0, 0, 0);
    ((QueryFloatFn)(*(void***)this)[0x90 / 4])(this, 4, 0, 0);
}
