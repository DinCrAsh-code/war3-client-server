//============================================================================
//  0x6F26BCD0 - build the widget's sprite and configure it from the art
//  descriptor.  CWidget::InitSprite's third step, and the only one that
//  touches the model.
//
//  Own translation unit: every call it makes is real.
//
//  It runs in two halves.  The first always happens: copy the descriptor's
//  flag word wholesale into CWidget::m_flags, and create the sprite if this
//  widget has not got one yet - at the detail level the descriptor asks for,
//  which is what picks CSpriteUber over CSpriteMini.  The second half is
//  skipped entirely when the descriptor carries no model handle, and the
//  emptiness test is taken *before* the sprite is created, not after, so a
//  modelless descriptor still gets a sprite.
//
//  The handle is read twice - once for that test and once as the argument to
//  the load - and the shipped code really does call it twice rather than
//  keeping the first result.  What it keeps instead is the 0/1 emptiness
//  flag, materialised with the usual `neg`/`sbb`/`neg` triple.
//
//  Both of those calls go to 0x6F4C4660, the one-instruction `jmp` alias,
//  not to 0x6F4C4630 itself - handleobject.cpp has both.  That is visible
//  only in the diff (the two call targets resolve to different names), it
//  makes no behavioural difference, and it is reproduced because a call to
//  the wrong one of two real symbols is still a call to the wrong symbol.
//============================================================================
#include "widget.h"
#include "sprite.h"
#include "matrix3.h"
#include "modeltokens.h"
#include "handleobject.h"
#include "mdp_demo.h"

//  dword_6F92ED98 - an *integer* compared against the descriptor's time
//  scale with `fild`, not a float constant: the shipped test is
//  `fld [esi+1Ch]` / `fild ds:dword_6F92ED98` / `fucompp`, so the sentinel
//  is stored as an int and widened at every comparison.
extern const int g_animationSpeedSentinel;

//  0x6F01F450 - the callback CSprite::LoadModelWithTokens carries through to
//  the model release.  Its address is taken and never called from here.
void __fastcall ModelReleaseNotify();

CSprite* __fastcall CreateSpriteForDetailLevel(int detailLevel);
int __fastcall GetModelDetailFlags(int wantsFullDetail);
void __fastcall LoadModelWithTokens(CSprite* self, unsigned int modelHandle,
                                    CTokenizer* animTokens,
                                    CTokenizer* attachmentTokens,
                                    CTokenizer* boneTokens, int detailFlags,
                                    unsigned int player, void* notify);

//  CWidget's own vtable, by raw offset.  Both of these hand back a float
//  that is discarded on the spot (`fstp st`), so they are called for their
//  side effect only - re-evaluating some derived quantity now that the
//  sprite exists.
typedef float (__thiscall *QueryFloatFn)(void*, int, int, int);

//  CFloat's storage already is an IEEE single - see cmatrix3_tofloat.cpp.
static inline float AsHardwareFloat(const CFloat& value)
{
    return *(const float*)&value.m_bits;
}

void CWar3Image::CreateSpriteFromDescriptor(const SWidgetArtDescriptor* desc)
{
    void* self = this;

    m_flags = desc->m_widgetFlags;

#ifdef MDP_DEMO_PLAYER0_OWNS_WIDGETS
    //  Deliberate behaviour change - see mdp_demo.h.  Put player 0 into
    //  both of this widget's 16-bit player masks (widget.h) as it is
    //  created, so player 0 is in the "can see"/"has seen" set from the
    //  start rather than only once the fog reveals it.
    m_playerMaskA = (unsigned short)(m_playerMaskA | 1);
    m_playerMaskB = (unsigned short)(m_playerMaskB | 1);
#endif

    int hasModel =
        GetHandleOrZeroAlias((const SHandleHolder*)desc->m_modelHandle) != 0;

    if (m_pSprite == 0)
        m_pSprite = CreateSpriteForDetailLevel(desc->m_detailLevel);

    if (!hasModel)
        return;

    LoadModelWithTokens(m_pSprite,
                        GetHandleOrZeroAlias(
                            (const SHandleHolder*)desc->m_modelHandle),
                        GetAnimationTokenizer(),
                        GetAttachmentTokenizer(),
                        GetBoneTokenizer(),
                        GetModelDetailFlags(desc->m_wantsFullDetail),
                        desc->m_owningPlayer,
                        (void*)&ModelReleaseNotify);

    SetOwningPlayer(desc->m_owningPlayer);

    SetSpritePausedBit(m_pSprite, 1);
    StopSpriteAnimationsOfType(m_pSprite, 5);
    SetSpriteAnimationScale(m_pSprite, desc->m_animationScale);

    if (desc->m_timeScale != (float)g_animationSpeedSentinel)
        SetSpriteTimeScale(m_pSprite, desc->m_timeScale);

    if (desc->m_teamColor != -1)
        SetSpriteTeamColor(m_pSprite, 0, desc->m_teamColor);

    SetItemPlacementFlagBits(m_pSprite, 0x3000000, 1);
    SetSpriteStateBits(m_pSprite, 7, 1);

    ((QueryFloatFn)(*(void***)self)[0x8C / 4])(self, 0, 0, 0);
    ((QueryFloatFn)(*(void***)self)[0x90 / 4])(self, 4, 0, 0);

    //  One vector, written twice.  The shipped frame has room for exactly
    //  one FVec3 here and the rotation axis lands on the position's own
    //  three slots - two *separate* locals with disjoint lifetimes are not
    //  enough to get that, because MSVC will not reuse a slot it has
    //  already handed to a by-value argument copy; reusing the object in
    //  the source is.
    FVec3 vec;
    vec.m_x = AsHardwareFloat(desc->m_x);
    vec.m_y = AsHardwareFloat(desc->m_y);
    vec.m_z = AsHardwareFloat(desc->m_z);
    SetItemPlacementPosition(m_pSprite, vec);

    vec.m_x = 0.0f;
    vec.m_y = 0.0f;
    vec.m_z = 1.0f;

    OrientationMatrix orientation;
    orientation.RotateAxis(AsHardwareFloat(desc->m_facing), vec, 1);
    SetItemPlacementOrientation(m_pSprite,
                                *(const SOrientationMatrix*)&orientation);
}
