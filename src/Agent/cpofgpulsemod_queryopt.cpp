//============================================================================
//  0x6F4A8AF0 - same shape again, reading +0x18 with a 0 default.
//============================================================================
#include "cpofgpulsemod.h"

namespace NIpse {

struct SPoFgPulseModSource
{
    int QueryOpt();
};

int SPoFgPulseModSource::QueryOpt()
{
    if (this == 0)
        return 0;
    return *(int*)((char*)this + 0x18);
}

}  // namespace NIpse
