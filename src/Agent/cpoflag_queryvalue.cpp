//============================================================================
//  0x6F4A8960 - NIpse::SPoFlagValueSource::QueryValue, own translation unit
//  (see Agent/cpoflag_apply.cpp's own struct declaration) - deliberately
//  separate from its one caller (CPoFlag::Apply) so /Ob2 cannot inline it
//  away; the real dump shows the call staying out of line, matching
//  Agent/cpofglis_querymask.cpp's own precedent for this exact shape.
//============================================================================
#include "cpoflag.h"

namespace NIpse {

//  Declared here (rather than in cpoflag.h) since it is not part of
//  CPoFlag's own public interface - see cpoflag_apply.cpp for its use.
struct SPoFlagValueSource
{
    int QueryValue();
};

int SPoFlagValueSource::QueryValue()
{
    if (this == 0)
        return 0;
    return *(int*)((char*)this + 0x0C);
}

}  // namespace NIpse
