//============================================================================
//  0x6F4A8A90 - same shape as cpofgpulsemod_queryfrequency.cpp's own leaf,
//  reading +0x10 with a 0 default instead of -1.
//============================================================================
#include "cpofgpulsemod.h"

namespace NIpse {

struct SPoFgPulseModSource
{
    int QueryDuration();
};

int SPoFgPulseModSource::QueryDuration()
{
    if (this == 0)
        return 0;
    return *(int*)((char*)this + 0x10);
}

}  // namespace NIpse
