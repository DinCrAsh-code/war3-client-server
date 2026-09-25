//============================================================================
//  0x6F36BEB0 - CTimeOfDayIndicator::CTimeOfDayIndicator.
//
//  Everything the day/night dial needs, in the shipped order:
//
//    * the CSpriteFrame base under `parent`;
//    * its own flag word or'd with the mask it already carries plus bit 2;
//    * anchor point 6 of the dial onto point 6 of the parent, at (0, 0);
//    * the sprite the "TimeOfDayIndicator" skin names;
//    * a slot-0 ramp binding on the dial's own sprite whose callback is
//      0x6F3659A0 and whose context is the indicator itself;
//    * the animation started (RefreshDialAnimation, which is why the flag at
//      +0x1B4 has to be zeroed before it rather than after);
//    * the CTimeCover, allocated at 0x774 bytes - CFrame's 0x168 plus the
//      two tooltip buffers - sized to 0.09f square and anchored point 1 to
//      point 1 of the same parent;
//    * and the two [Misc] configuration floats.
//
//  Three pointer conversions are worth reading carefully, because MSVC
//  treats two of them differently and the shipped stream shows both:
//
//    * `parent` handed to SetPointTo as a CLayoutFrame* is a *conversion*,
//      so it gets the null-checked `lea eax,[edi+0B4h]` / `xor eax,eax`
//      pair - twice, once per anchor;
//    * `m_pCover->SetWidth(...)` is a *member call* on a CFrame subclass, so
//      the same +0xB4 adjustment goes in unconditionally with no test.
//
//  The two configuration values arrive as CFloats and land in plain float
//  fields.  The shipped code copies them through the x87 (`fld` / `fstp`),
//  which is a float assignment; a four-byte class copy would be a `mov`
//  pair.  So the read is punned rather than assigned, which is what
//  AsFloat below spells.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  a constructor with a base and an owned allocation to unwind; this repo's
//  fixed /GS- /EHs-c- emits none of it.
//============================================================================
#include "timeofdayindicator.h"
#include "gameui.h"
#include "gamecontext.h"
#include "storm.h"

//  0x6F4E8720 - GameUI/cameracontrollerbind.h.
#include "cameracontrollerbind.h"

//  0x6F60EDE0 - Misc/trivialpredicates.cpp.
int __fastcall GetIndirectHead_6F60EDE0(const void* self);

//  0x6F3659A0 - the ramp callback the dial binds on slot 0.  Never called
//  from anywhere in this tree, only had its address taken; `retn 4`, which
//  is what this __stdcall declaration puts there.
void __stdcall OnTimeOfDayRampSlot(void*);

//  The CFloat/float pun the two configuration reads need; the two have the
//  same representation (Math/CFloat.h says so in its first paragraph).
static float AsFloat(const CFloat& v) { return *(const float*)&v; }

static const char kTimeOfDayFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\CTimeOfDayIndicator.cpp";

CTimeOfDayIndicator::CTimeOfDayIndicator(CFrame* parent)
    : CSpriteFrame((int)parent, 0, 0)
{
    m_extendedDialAnim = 0;
    m_flags |= m_flagMask | 2;

    SetPointTo(6, parent, 6, 0.0f, 0.0f, 1);

    SetSpritePath(ResolveSkinValue("TimeOfDayIndicator", 0), -1, 0);

    BindControllerSlotRamp((void*)GetIndirectHead_6F60EDE0(this), 0,
                           (void*)&OnTimeOfDayRampSlot, this, 0.0f);

    RefreshDialAnimation();

    void* memory = SMemAlloc(0x774, kTimeOfDayFile, 0x95, 0);
    m_pCover = memory ? new (memory) CTimeCover(this) : 0;

    m_pCover->SetWidth(0.09f);
    m_pCover->SetHeight(0.09f);
    m_pCover->SetPointTo(1, parent, 1, 0.0f, 0.0f, 1);

    m_dayHours = AsFloat(GetConfigFloat("Misc", "DayHours", 0));
    m_scaledAnimTime = AsFloat(GetConfigFloat("Misc", "ScaledAnimTime", 0));
}
