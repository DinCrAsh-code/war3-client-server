//============================================================================
//  0x6F3A1460 - SPlayerTableFogHost::RebuildPlayerMasks: recompute the two
//  16-bit visibility masks a point's fog state produces, one player at a
//  time.
//
//  Reached from SItemMaskNotify::OnMaskChanged (0x6F38EA30,
//  itemsinkmaskchanged.cpp) - that function is a thunk for reasons of its
//  own (six other unreached leaf helpers), but this call inside it is a
//  self-contained leaf: both its callees, Prepare and Submit, are already
//  reconstructed (playertablesubmitprepare.cpp / playertablesubmitparts.cpp).
//
//  `players` is a caller-owned array whose first word is skipped (the walk
//  starts at `players + 1`, i.e. `players`'s own +4 in bytes) and whose
//  next twelve words are one per player slot, each either 0 (this slot's
//  own player-mask *bit*, `1 << i`, is not being asked about at all - the
//  loop does nothing for it) or a per-player Storm-key mask Submit() is
//  asked about directly.  Both outputs are rebuilt from scratch: the
//  caller-owned words at *outVisibleMask and *outFoggedMask are zeroed up
//  front and only ever OR'd into afterwards, and what gets OR'd in is
//  always the slot's own `1 << i` - the per-player mask from `players[]`
//  is Submit()'s argument and the short-circuit test's operand, never the
//  value written back.
//
//  Per player i, in order:
//
//    * if this player's own Storm-key mask already overlaps a bit already
//      set in *outVisibleMask, `1 << i` is folded into *outVisibleMask
//      immediately without calling Submit() at all - the same short-
//      circuit a player already counted through an earlier, wider mask
//      does not need asking about again;
//    * otherwise Submit(codeA, codeB, playerMask) is asked, and a
//      "visible" (bit 2) answer folds `1 << i` into *outVisibleMask the
//      same way;
//    * an answer that is not visible only ever touches *outFoggedMask, and
//      only when it is "fogged" (bit 1) - unless m_field3C0 is set, in
//      which case that fold happens unconditionally for every player who
//      was not visible, fogged or not.
//
//  Own translation unit: two real calls (Prepare, Submit).
//============================================================================
#include "playertablefog.h"

void SPlayerTableFogHost::RebuildPlayerMasks(const CWorldVec3* pos,
                                             unsigned short* outVisibleMask,
                                             unsigned short* outFoggedMask,
                                             const unsigned short* players)
{
    *outVisibleMask = 0;
    *outFoggedMask = 0;

    unsigned int codeA;
    unsigned int codeB;
    m_pParts->Prepare((CWorldVec3*)pos, (unsigned short*)&codeA,
                      (unsigned short*)&codeB);

    const unsigned short* slot = players + 1;
    unsigned short bit = 1;

    for (unsigned int i = 0; i < 12; i++, slot++, bit = (unsigned short)(bit + bit))
    {
        unsigned short playerMask = *slot;
        if (playerMask == 0)
            continue;

        if ((*outVisibleMask & playerMask) != 0)
        {
            *outVisibleMask |= bit;
            continue;
        }

        int result = m_pParts->Submit(codeA, codeB, playerMask);

        if ((result & 4) != 0)
        {
            *outVisibleMask |= bit;
            continue;
        }

        if (m_field3C0 != 0 || (result & 2) != 0)
            *outFoggedMask |= bit;
    }
}
