//============================================================================
//  0x6F4A8C80 - SPoFgLisMaskSource::QueryMask, own translation unit (see
//  cpofglis.h's own declaration comment for why it is not folded into
//  CPoFgLis::Slot3's own file).
//============================================================================
#include "cpofglis.h"

int SPoFgLisMaskSource::QueryMask()
{
    if (this == 0)
        return -1;
    return *(int*)((char*)this + 0x0C);
}
