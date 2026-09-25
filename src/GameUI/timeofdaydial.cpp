//============================================================================
//  0x6F33C6F0 - CTimeOfDayIndicator::RefreshDialAnimation.
//
//  The dial's sprite hangs off the indirect head of the frame itself, which
//  is what 0x6F60EDE0 hands back; with no sprite there is nothing to do.
//
//  Both arms build the animation-name token array in the same two stack
//  words and the first token is 2 either way - the shipped code stores it
//  before the branch - so the flag at +0x1B4 changes only the *length*: two
//  tokens (2, 4) when it is set, one (2) when it is not.  The second token
//  is written only on the two-token arm, which is why the store into the
//  second word is inside the `if` and not next to the first.
//
//  SpriteStartAnimation returns a float, which the shipped code pops off the
//  x87 stack and discards; nothing here reads it.
//============================================================================
#include "timeofdayindicator.h"

class CSprite;

//  0x6F4D7E50 - Widget/widget_spriteplay.cpp, under the same name and
//  signature as GameUI/gameuicursorstate.cpp declares it so both reach the
//  one symbol.
float __fastcall SpriteStartAnimation(CSprite* sprite,
                                      const unsigned int* name,
                                      int flags, int extra);
//  0x6F60EDE0 - Misc/trivialpredicates.cpp.
int __fastcall GetIndirectHead_6F60EDE0(const void* self);

void CTimeOfDayIndicator::RefreshDialAnimation()
{
    CSprite* sprite = (CSprite*)GetIndirectHead_6F60EDE0(this);
    if (sprite == 0)
        return;

    unsigned int tokens[2];
    tokens[0] = 2;

    if (m_extendedDialAnim != 0)
    {
        tokens[1] = 4;
        SpriteStartAnimation(sprite, tokens, 2, 0);
    }
    else
    {
        SpriteStartAnimation(sprite, tokens, 1, 0);
    }
}
