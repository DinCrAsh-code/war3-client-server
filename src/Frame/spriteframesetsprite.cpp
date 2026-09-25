//============================================================================
//  0x6F60EAA0 - CSpriteFrame::SetSprite.  One half of SetSpritePath's own
//  two-part set-up (frame.h's own comment on the pair); this half publishes
//  the scale.  Real reconstruction now - it was a naked redirect in
//  Frame/framethunks.cpp.
//
//  `this+0x114` is a plain float this batch does not have a name for (it
//  belongs to the CFrame base, well above CSpriteFrame's own +0x168 fields -
//  "only touched members get names", so it stays a raw offset rather than a
//  guessed field on a base class this session does not otherwise touch).
//  +0x1A0 is CSpriteFrame's own m_1A0.
//
//  The callee is GameUI/controllerslotsetfloat.cpp's own
//  `SetControllerSlotFloat(host, slot, value)` - slot 2 here, read straight
//  off the shipped `mov edx, 2`.
//============================================================================
#include "frame.h"
#include "controllerslot.h"

void CSpriteFrame::SetSprite(void* sprite, float scale)
{
    if (sprite == 0)
        return;
    if (m_1A0 == 0)
        return;

    float value = *(float*)((char*)this + 0x114) * scale;
    SetControllerSlotFloat(sprite, 2, value);
}
