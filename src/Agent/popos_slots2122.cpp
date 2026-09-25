//============================================================================
//  NIpse::CPoPos - vtable slots 21 and 22, both genuine no-ops at their
//  own addresses (not the shared `SVtableNoOp`/`SVtableNoOpArg` this
//  whole chain otherwise reuses - these are CPoPos's own, distinct
//  addresses, one instruction each).
//============================================================================
#include "popos.h"

namespace NIpse {

//  0x6F4879B0 - vtable slot 21. `retn 4`, no `this` read, one stack arg
//  consumed and discarded.
void CPoPos::Slot21(unsigned int)
{
}

//  0x6F4879C0 - vtable slot 22. `retn 0`, no `this` read, no arguments.
void CPoPos::Slot22()
{
}

}  // namespace NIpse
