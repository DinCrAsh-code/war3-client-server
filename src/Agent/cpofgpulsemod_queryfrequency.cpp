//============================================================================
//  0x6F4A8A70 - `this`-implicit, `ecx`-only, `retn 0` leaf: given `this`
//  (a `SHandleRegistrationRequest*`, opaque here), returns its own +0x0C
//  field, or -1 when `this` is null. Own translation unit, kept out of
//  line to match the real dump.
//============================================================================
#include "cpofgpulsemod.h"

namespace NIpse {

struct SPoFgPulseModSource
{
    int QueryFrequency();
};

int SPoFgPulseModSource::QueryFrequency()
{
    if (this == 0)
        return -1;
    return *(int*)((char*)this + 0x0C);
}

}  // namespace NIpse
