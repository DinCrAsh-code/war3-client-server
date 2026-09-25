//============================================================================
//  0x6F38DBE0 - CWorldFrameWar3::CreateWaypointIndicators: build the sprite
//  for every one of the 256 waypoint-indicator records at +0x598.
//
//  The record stride and the count are the loop's own: `add edi, 1Ch` /
//  `cmp edi, 1C00h`, so 0x1C bytes each and 0x100 of them, and the sprite
//  lives at +0x08 of each.  The loop is written as a byte offset rather
//  than as an index because that is what the shipped stream advances - one
//  register stepping by 0x1C against a constant limit, with the base
//  re-read from +0x598 every iteration.
//
//  The model is "WaypointIndicator" resolved through the active skin, and
//  it is loaded with the full eight-argument LoadModelWithTokens: the two
//  trailing arguments (the owning player at +0x198 and the release
//  callback) are pushed first, which is where the shipped stream's two
//  early pushes come from.
//
//  Own translation unit: every call in it is real.
//============================================================================
#include "gameui.h"
#include "sprite.h"
#include "modeltokens.h"
#include "framethunks.h"

//  0x6F314CE0 - Widget/spritemodel.cpp.
void __fastcall LoadModelWithTokens(CSprite* self, unsigned int modelHandle,
                                    CTokenizer* animTokens,
                                    CTokenizer* attachmentTokens,
                                    CTokenizer* boneTokens, int detailFlags,
                                    unsigned int player, void* notify);
//  0x6F333760 - Widget/widget_setmodelhandle.cpp declares it the same way.
int __fastcall GetModelDetailFlags(int wantsFullDetail);
//  0x6F4D7E50 - Widget/widget_spriteplay.cpp.
float __fastcall SpriteStartAnimation(CSprite* sprite,
                                      const unsigned int* name,
                                      int flags, int extra);
//  0x6F01F450 - Misc/unreconstructed_thunks.cpp.
void __fastcall ModelReleaseNotify();

void CWorldFrameWar3::CreateWaypointIndicators()
{
    unsigned int offset = 0;
    do
    {
        SWaypointIndicator* record =
            (SWaypointIndicator*)((char*)m_pWaypoints + offset);

        record->m_pSprite = CreateSpriteForDetailLevel(1);

        LoadModelWithTokens(record->m_pSprite,
                            (unsigned int)ResolveSkinValue("WaypointIndicator",
                                                           0),
                            GetAnimationTokenizer(), 0, 0,
                            GetModelDetailFlags(1),
                            m_localPlayer, (void*)&ModelReleaseNotify);

        unsigned int animation = 4;
        SpriteStartAnimation(record->m_pSprite, &animation, 1, 0);
        SetSpriteStateBits(record->m_pSprite, 7, 1);

        offset += 0x1C;
    }
    while (offset < 0x1C00);
}
