//============================================================================
//  0x6F4A9680 - `this`-implicit, `ecx`-only, `retn 0` leaf: given `this`
//  (opaque here), returns its own +0x14 field, or 0 when `this` is null.
//  Own translation unit, kept out of line to match the real dump.
//============================================================================
#include "cporepulsemod.h"

namespace NIpse {

struct SPoRePulseModSource
{
    int QueryOpt();
};

int SPoRePulseModSource::QueryOpt()
{
    if (this == 0)
        return 0;
    return *(int*)((char*)this + 0x14);
}

}  // namespace NIpse
