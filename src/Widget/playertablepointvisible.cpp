//============================================================================
//  0x6F3BA430 - SPlayerTableSubmitParts::IsPointVisible: is this world point
//  visible to any of the players in `playerMask`?
//
//  Three statements: clamp the point into the map's playable rectangle,
//  turn it into Prepare()'s pair of cell codes, and ask Submit() for the fog
//  state.  4 is "visible" - the same value playertablesubmitparts.cpp's own
//  answer table hands back from column 0 of every row - so the whole test is
//  one `== 4` on Submit's result, which the shipped `xor ecx,ecx` /
//  `cmp eax,4` / `setz cl` tail is MSVC's branchless spelling of.
//
//  The point arrives **by value**: `retn 10h` is three dwords of CWorldVec3
//  plus the mask, and ClampToGameBounds rewrites the copy in place before
//  Prepare ever sees it, which a by-pointer parameter would have made
//  visible to the caller.  Its callers (jassnatives_xyvisibility.cpp) build
//  that copy straight into the argument area with `sub esp,0Ch`.
//
//  ClampToGameBounds takes a CGridVec2* and is handed the address of a
//  CWorldVec3 here: it touches only the leading x/y pair, which the two
//  types share, and the shipped code really does pass this vector to that
//  function.
//
//  Own translation unit: it is 0x9000 bytes away from its two fogged/masked
//  twins in the image, it issues two real calls, and nothing may inline.
//============================================================================
#include "playertablefog.h"
#include "gamebounds.h"

int SPlayerTableSubmitParts::IsPointVisible(CWorldVec3 pos,
                                            unsigned int playerMask)
{
    ClampToGameBounds((CGridVec2*)&pos);

    //  Two dword locals whose *low words* Prepare fills, exactly as
    //  SPlayerTableSubmit::SubmitMasked has them (playertablesubmit.cpp):
    //  the shipped code reads both back with a plain `mov`, not a `movzx`.
    unsigned int codeA;
    unsigned int codeB;
    Prepare(&pos, (unsigned short*)&codeA, (unsigned short*)&codeB);

    return Submit(codeA, codeB, playerMask) == 4;
}
