//============================================================================
//  0x6F38BD80 - CWorldFrameWar3::CreateTargetConfirmSprite: build the little
//  sprite that flashes where a move or attack order was issued.
//
//  The model is not a path in this module - it is whatever the active skin
//  has under the key "TargetPointConfirm", which is what ResolveSkinValue
//  answers, and it rides into CSprite::SetModel in edx as that function's
//  second __fastcall argument.
//
//  Own translation unit: all four calls are real.
//============================================================================
#include "gameui.h"
#include "sprite.h"
#include "modeltokens.h"
#include "framethunks.h"
#include "gameui.h"

//  0x6F4D8D60 - Misc/unreconstructed_thunks.cpp owns the redirect and the
//  name; re-declared rather than re-thunked so one address keeps one name.
void __fastcall SetSpriteModel(CSprite* self, unsigned int modelHandle,
                               CTokenizer* animTokens,
                               CTokenizer* attachmentTokens,
                               CTokenizer* boneTokens, int detailFlags);

void CWorldFrameWar3::CreateTargetConfirmSprite()
{
    m_pTargetConfirmSprite = CreateSpriteForDetailLevel(1);
    SetSpriteModel(m_pTargetConfirmSprite,
                   (unsigned int)ResolveSkinValue("TargetPointConfirm", 0),
                   GetAnimationTokenizer(), 0, 0, 0);
    SetSpriteStateBits(m_pTargetConfirmSprite, 7, 1);
}
