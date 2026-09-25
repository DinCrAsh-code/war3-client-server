//============================================================================
//  0x6F399740 - CWorldFrameWar3::CreateRallyIndicators: the sixteen
//  rally-point destination indicators and the one source indicator.
//
//  The same sprite set-up as CreateWaypointIndicators
//  (GameUI/worldframewaypoints.cpp) run seventeen times, with two
//  differences worth reading:
//
//  * the skin key is "RallyIndicatorDst" for the sixteen and
//    "RallyIndicatorSrc" for the one, and the source indicator is not a
//    record at all - it hangs straight off the frame at +0x588;
//  * the detail-flags argument is a literal 0 here where the waypoints pass
//    GetModelDetailFlags(1).
//
//  Alongside the records there is a sixteen-int draw order at +0x53C,
//  filled 15, 14, ... 0 - one entry per iteration, written from the *front*
//  while the record it belongs to is built.  Its own resize is inlined in
//  the shipped body and is the shorter of this binary's two SetCount shapes
//  (see gameui.h); the record array's is a real call.
//
//  Both arrays are re-read through the frame every iteration rather than
//  hoisted - `mov ecx, [ebx+544h]` and `mov esi, [ebx+3B8h]` are inside the
//  loop - because the sprite construction in between can reallocate
//  neither, but the shipped code does not know that and neither does this.
//
//  Own translation unit: every call in it is real.
//============================================================================
#include "gameui.h"
#include "sprite.h"
#include "modeltokens.h"
#include "framethunks.h"

//  0x6F314CE0 - Widget/spritemodel.cpp, declared exactly as
//  GameUI/worldframewaypoints.cpp declares it.
void __fastcall LoadModelWithTokens(CSprite* self, unsigned int modelHandle,
                                    CTokenizer* animTokens,
                                    CTokenizer* attachmentTokens,
                                    CTokenizer* boneTokens, int detailFlags,
                                    unsigned int player, void* notify);
//  0x6F4D7E50 - Widget/widget_spriteplay.cpp.
float __fastcall SpriteStartAnimation(CSprite* sprite,
                                      const unsigned int* name,
                                      int flags, int extra);
//  0x6F01F450 - Misc/unreconstructed_thunks.cpp.
void __fastcall ModelReleaseNotify();

void CWorldFrameWar3::CreateRallyIndicators()
{
    m_rallyOrder.SetCount(16);
    m_rallyIndicators.SetCount(16);

    //  Two induction variables and not one, which is what the shipped loop
    //  carries: the index for the four-byte order array and a byte offset
    //  for the 0x18-byte records.  Indexing both off `i` makes MSVC keep
    //  i * 4 in a register and spill i * 0x18 to the frame instead.
    unsigned int offset = 0;
    for (int i = 0; i < 16; i++)
    {
        m_rallyOrder.m_data[i] = 15 - i;

        SRallyIndicator* record =
            (SRallyIndicator*)((char*)m_rallyIndicators.m_data + offset);

        record->m_pSprite = CreateSpriteForDetailLevel(1);

        LoadModelWithTokens(record->m_pSprite,
                            (unsigned int)ResolveSkinValue(
                                "RallyIndicatorDst", 0),
                            GetAnimationTokenizer(), 0, 0, 0,
                            m_localPlayer, (void*)&ModelReleaseNotify);

        unsigned int animation = 4;
        SpriteStartAnimation(record->m_pSprite, &animation, 1, 0);

        offset += 0x18;
    }

    m_pRallySourceSprite = CreateSpriteForDetailLevel(1);

    LoadModelWithTokens(m_pRallySourceSprite,
                        (unsigned int)ResolveSkinValue("RallyIndicatorSrc", 0),
                        GetAnimationTokenizer(), 0, 0, 0,
                        m_localPlayer, (void*)&ModelReleaseNotify);

    unsigned int animation = 4;
    SpriteStartAnimation(m_pRallySourceSprite, &animation, 1, 0);
}
