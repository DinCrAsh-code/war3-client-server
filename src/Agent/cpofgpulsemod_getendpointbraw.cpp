//============================================================================
//  0x6F4A8B10 - NIpse::CPoFgPulseMod::GetEndpointBRaw, own translation
//  unit (called from NotifyEndpointAIfTagged, cpofgpulsemod_notify.cpp -
//  kept out of line so /Ob2 does not fold it in). One-instruction leaf:
//  `this`'s own +0x40 (CAgentRelation::m_endpointB) - the identical shape
//  Agent/cpofglis.h's own GetEndpointBRaw already reconstructs at a
//  different address (docs/msvc-vc8-idioms.md's own "Four identical
//  functions at four addresses" idiom).
//============================================================================
#include "cpofgpulsemod.h"

namespace NIpse {

void* CPoFgPulseMod::GetEndpointBRaw()
{
    return *(void**)((char*)this + 0x40);
}

}  // namespace NIpse
