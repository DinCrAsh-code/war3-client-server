//============================================================================
//  0x6F302950 - SSpriteHost::GetAttachmentOrigin.
//
//  Zero the caller's three-float destination, then let the sprite at +0x34
//  overwrite it with the world position of its attachment point 7.  The
//  three stores come off one `fldz` - `fst`/`fst`/`fstp` - which is what
//  assigning one value to three members gives and what three separate zero
//  literals would not.
//
//  The destination is pushed once, before the stores, and handed straight
//  back in eax: an out-pointer that is also the return value.
//
//  Own translation unit: 0x6F4E8260 is a real call into the sprite engine.
//============================================================================
#include "gameuithunks.h"

struct SSpriteHost
{
    char  m_reserved00[0x34];
    void* m_pSprite;                // +0x34

    float* __thiscall GetAttachmentOrigin(float* out);
};

enum { kSpriteAttachmentOrigin = 7 };

float* SSpriteHost::GetAttachmentOrigin(float* out)
{
    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 0.0f;

    QuerySlotVec3(m_pSprite, kSpriteAttachmentOrigin, out);

    return out;
}
