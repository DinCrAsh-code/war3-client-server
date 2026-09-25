//============================================================================
//  0x6F26B8C0 - give the widget an owning player.
//
//  Own translation unit: both calls are real.  Resolve the player's colour
//  slot first, then hand player and slot together to StoreOwningPlayer -
//  the slot is not recomputed on the sprite side, it is passed down.
//============================================================================
#include "widget.h"

#include "playercolor.h"

void CWar3Image::SetOwningPlayer(unsigned int player)
{
    StoreOwningPlayer(player, ResolvePlayerColorSlot(player));
}
