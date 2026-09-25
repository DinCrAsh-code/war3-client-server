//============================================================================
//  0x6F602F80 - CSimpleButton::AttachSlotTexture: park a loaded texture in
//  one of the button's three slots (m_slots, +0x158), dropping whatever was
//  there first (through its own deleting destructor, slot 2, with
//  deleteFlags=1 - the same pattern Destroy2 uses everywhere else in this
//  module), and re-parenting the new one onto this button's own layer 2.
//
//  `CSimpleTexture` really is a `CSimpleRegion` (frame.h), which is what
//  the shipped call into 0x6F60E800 - already reconstructed as
//  `CSimpleRegion::SetParentLayer`, Frame/simpleregionparent.cpp - is
//  doing: `texture->SetParentLayer(this, 2, 0)`, cast through the base.
//
//  The new texture is stored into the slot unconditionally (even null, to
//  clear it); only a *non-null* texture landing in the currently active
//  slot (m_enabled doubles as the active-state index - frame.h's own
//  comment on it) also mirrors into m_164 and calls the already-reconstructed
//  `CSimpleRegion::AttachToLayer` (0x6F60E7C0, Frame/simpleregionattach.cpp)
//  on it, not `ShowSlotTexture` - the two names turned out to be the same
//  shipped address; verified against the diff (0.969 without this fix,
//  1.000 with it).
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleButton::AttachSlotTexture(int slot, CSimpleTexture* texture)
{
    CSimpleTexture* old = m_slots[slot];
    if (old)
    {
        typedef void* (__thiscall *DestroyFn)(void*, int);
        ((DestroyFn)(*(void***)old)[0x08 / 4])(old, 1);
    }

    if (texture)
        ((CSimpleRegion*)texture)->SetParentLayer(this, 2, 0);

    m_slots[slot] = texture;

    if (texture && slot == m_enabled)
    {
        m_164 = texture;
        ((CSimpleRegion*)texture)->AttachToLayer();
    }
}
