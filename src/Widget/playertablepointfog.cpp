//============================================================================
//  0x6F276240 / 0x6F276290 - the fogged and masked halves of the same query
//  playertablepointvisible.cpp answers for "visible": both are
//  IsPointVisible line for line with a different constant compared against
//  Submit()'s result.  4 is visible, 2 is fogged, 1 is masked - the three
//  values playertablesubmitparts.cpp's answer table can produce, and the
//  three JASS natives that reach these (IsVisibleToPlayer, IsFoggedToPlayer,
//  IsMaskedToPlayer) name them one for one.
//
//  The two are adjacent in the image (0x50 apart) and neither calls the
//  other, so they share a translation unit; the visible one does not,
//  because it lives in a different module entirely.
//
//  The masked tail is spelled `!(state - 1)` rather than `state == 1`: the
//  shipped code is `sub eax,1` / `neg eax` / `sbb eax,eax` / `add eax,1`,
//  which is MSVC's zero-test-as-a-value, where `== 1` compiles to the
//  `xor`/`cmp`/`setz` its two siblings have.  Same answer, and the two
//  spellings are what tell the two shipped tails apart.
//============================================================================
#include "playertablefog.h"
#include "gamebounds.h"

int SPlayerTableSubmitParts::IsPointFogged(CWorldVec3 pos,
                                           unsigned int playerMask)
{
    ClampToGameBounds((CGridVec2*)&pos);

    unsigned int codeA;
    unsigned int codeB;
    Prepare(&pos, (unsigned short*)&codeA, (unsigned short*)&codeB);

    return Submit(codeA, codeB, playerMask) == 2;
}

int SPlayerTableSubmitParts::IsPointMasked(CWorldVec3 pos,
                                           unsigned int playerMask)
{
    ClampToGameBounds((CGridVec2*)&pos);

    unsigned int codeA;
    unsigned int codeB;
    Prepare(&pos, (unsigned short*)&codeA, (unsigned short*)&codeB);

    return !(Submit(codeA, codeB, playerMask) - 1);
}
