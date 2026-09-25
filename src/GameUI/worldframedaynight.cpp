//============================================================================
//  0x6F390460 - CWorldFrameWar3::SetDayNightModels: load the day and the
//  night model of the day/night cycle, each into its own sprite.
//
//  Two identical blocks, one per sprite (+0x338 and +0x33C), each guarded by
//  "was a model asked for at all" and then by "does the sprite exist yet".
//  0x6F39A910 calls it with the two
//  "Environment\\DNC\\DNCLordaeron\\DNCLord..." paths, which is what says
//  what the two models are.
//
//  Each sprite gets two controller-slot bindings with the world frame as
//  their context - the ramp binder on slot 0 and the direct binder on slot 1
//  - and is then started on animation 4.
//
//  Own translation unit: every call in it is real.
//============================================================================
#include "gameui.h"
#include "sprite.h"
#include "modeltokens.h"
#include "framethunks.h"
#include "cameracontrollerbind.h"

//  0x6F4D8D60 - Misc/unreconstructed_thunks.cpp owns the redirect and the
//  name.
void __fastcall SetSpriteModel(CSprite* self, unsigned int modelHandle,
                               CTokenizer* animTokens,
                               CTokenizer* attachmentTokens,
                               CTokenizer* boneTokens, int detailFlags);
//  0x6F4D7E50 - Widget/widget_spriteplay.cpp.
float __fastcall SpriteStartAnimation(CSprite* sprite,
                                      const unsigned int* name,
                                      int flags, int extra);
//  The two callbacks these bindings register, both address-taken only and
//  both `retn 4` (read with ida_query; neither has a dump).  Redirected in
//  GameUI/gameui_thunks.cpp.
void __stdcall WorldFrameOnDayNightRampSlot(void* context);
void __stdcall WorldFrameOnDayNightDirectSlot(void* context);

void CWorldFrameWar3::SetDayNightModels(unsigned int dayModel,
                                        unsigned int nightModel)
{
    if (dayModel != 0)
    {
        if (m_pDayModel == 0)
            m_pDayModel = CreateSpriteForDetailLevel(1);

        SetSpriteModel(m_pDayModel, dayModel, GetAnimationTokenizer(), 0, 0, 0);
        BindControllerSlotRamp(m_pDayModel, 0,
                               (void*)&WorldFrameOnDayNightRampSlot, this,
                               0.0f);
        BindControllerSlotDirect(m_pDayModel, 1,
                                 (void*)&WorldFrameOnDayNightDirectSlot, this,
                                 0.0f);

        unsigned int animation = 4;
        SpriteStartAnimation(m_pDayModel, &animation, 1, 0);
    }

    if (nightModel != 0)
    {
        if (m_pNightModel == 0)
            m_pNightModel = CreateSpriteForDetailLevel(1);

        SetSpriteModel(m_pNightModel, nightModel, GetAnimationTokenizer(),
                       0, 0, 0);
        BindControllerSlotRamp(m_pNightModel, 0,
                               (void*)&WorldFrameOnDayNightRampSlot, this,
                               0.0f);
        BindControllerSlotDirect(m_pNightModel, 1,
                                 (void*)&WorldFrameOnDayNightDirectSlot, this,
                                 0.0f);

        unsigned int animation = 4;
        SpriteStartAnimation(m_pNightModel, &animation, 1, 0);
    }
}
