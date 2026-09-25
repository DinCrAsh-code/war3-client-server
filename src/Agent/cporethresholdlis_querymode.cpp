//============================================================================
//  0x6F4A98B0 - `this`-implicit, `ecx`-only, `retn 0` leaf: given `this`
//  (opaque here), returns its own +0x10 field, or 0 when `this` is null.
//  Own translation unit, kept out of line to match the real dump.
//============================================================================
#include "cporethresholdlis.h"

namespace NIpse {

struct SPoReThresholdLisSource
{
    int QueryMode();
};

int SPoReThresholdLisSource::QueryMode()
{
    if (this == 0)
        return 0;
    return *(int*)((char*)this + 0x10);
}

}  // namespace NIpse
