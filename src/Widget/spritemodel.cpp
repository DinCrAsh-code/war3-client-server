//============================================================================
//  0x6F314CE0 - load a model into the sprite, drop the old one, and colour
//  the new one for its owner.
//
//  Own translation unit: all three calls are real.
//
//  Six stack arguments plus two in registers.  The model handle rides in
//  edx untouched all the way into CSprite::SetModel - it is never stored,
//  never re-read, and the shipped code simply does not reload it before
//  that call, which is what a __fastcall second parameter forwarded to
//  another __fastcall second parameter produces.
//
//  The `notify` argument is the one oddity: it is loaded into edx
//  immediately before the ReleaseSpriteModel call, whose own body never
//  reads edx - see spriteflags.cpp for why that parameter is nonetheless
//  declared, and where it ends up.
//============================================================================
#include "sprite.h"
#include "tokenizer.h"

void __fastcall SetSpriteModel(CSprite* self, unsigned int modelHandle,
                               CTokenizer* animTokens,
                               CTokenizer* attachmentTokens,
                               CTokenizer* boneTokens, int detailFlags);
void __fastcall ApplyTeamColorForPlayer(CSprite* self, unsigned int player);

void __fastcall LoadModelWithTokens(CSprite* self, unsigned int modelHandle,
                                    CTokenizer* animTokens,
                                    CTokenizer* attachmentTokens,
                                    CTokenizer* boneTokens, int detailFlags,
                                    unsigned int player, void* notify)
{
    SetSpriteModel(self, modelHandle, animTokens, attachmentTokens,
                   boneTokens, detailFlags);
    ReleaseSpriteModel(self, notify);
    ApplyTeamColorForPlayer(self, player);
}
