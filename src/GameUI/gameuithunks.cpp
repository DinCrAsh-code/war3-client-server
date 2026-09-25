//============================================================================
//  See gameuithunks.h.  One naked redirect per shipped address; nothing in
//  this file is a reconstruction, and every one of them goes away as its own
//  batch of this closure lands.
//============================================================================
#include "gameuithunks.h"

//  Spelled as a *trailing* macro, the way Jass/jassnatives_thunks.cpp does
//  it, so that tools/thunk_inventory.py counts every one of these: its
//  CALL_THUNK pattern wants `NAME_THUNK(0x<addr>)` after the signature, and
//  a body-shaped macro with a MASM-style `0...h` literal is invisible to it.
//  Forty-one uncounted redirects is exactly how the placeholder count got
//  to 300 in the first place.
#define GAMEUI_THUNK(addr)       \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  --- the frame framework ---------------------------------------------------
//  Everything this block redirected is reconstructed in src/Frame/ now.  The
//  last two - CTextFrame::SetString (0x6F611D40) and the backdrop
//  generator's constructor (0x6F61D5A0) - went with batch D; both classes
//  are frame.h's, which gameuithunks.h includes.

//  0x6F62B170 and 0x6F5C9650 are reconstructed for real now - as
//  IsEventKeyRegistered in Frame/eventkeyquery.cpp and as GetGameString in
//  Config/gamestrings.cpp - so the redirects that stood here are gone.  A
//  redirect left behind after its body lands is not harmless: it is a second
//  C++ name for one shipped address, and verify.py scores the two-instruction
//  redirect rather than the body (GetGameString went 44/44 to 0/44 exactly
//  that way).

//  --- sprites and textures --------------------------------------------------
//  0x6F4D3100, 0x6F4D32E0 and 0x6F4E82B0 are real bodies now, in
//  Render/agentworldposition.cpp, Render/spritesettexture.cpp and
//  Render/spritesetchannelcolor.cpp; Render/sprite.h declares all three.

//  0x6F732360 is a real body now, in Render/loadtexturebypath.cpp;
//  Render/texture.h declares it.

//  0x6F4B5450 and 0x6F4C6480 are real bodies now, in
//  Math/matrix44transform.cpp and Math/uispan.cpp.

//  0x6F526A40 and 0x6F5266D0 are real bodies now, in
//  Render/viewprojectionmatrix.cpp and Render/viewportrect.cpp, and
//  Render/renderdevice.h declares both.  Leaving the redirects here would be
//  a second definition of each: the .mix links with /FORCE:MULTIPLE, the
//  first object on the line wins, and it is not necessarily the real body.