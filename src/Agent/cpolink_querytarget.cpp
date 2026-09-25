//============================================================================
//  0x6F4AA0A0 - NIpse::SPoLinkTargetSource::QueryTarget, own translation
//  unit - deliberately separate from its one caller (CPoLink::Apply) so
//  /Ob2 cannot inline it away, matching Agent/cpoflag_queryvalue.cpp's own
//  precedent (that attempt scored 0.894 with the call folded away before
//  the split).
//============================================================================
#include "cpolink.h"

namespace NIpse {

struct SPoLinkTargetSource
{
    void* QueryTarget();
};

void* SPoLinkTargetSource::QueryTarget()
{
    if (this == 0)
        return 0;
    return *(void**)((char*)this + 0x0C);
}

}  // namespace NIpse
