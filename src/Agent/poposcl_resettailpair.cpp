//============================================================================
//  0x6F493790 - NIpse::CPoPosCl_ResetTailPair. Own translation unit,
//  deliberately: Apply (poposcl_apply.cpp) calls this out of line in the
//  real dump, and keeping caller and callee in the same TU lets /Ob2
//  inline this two-store leaf away, which the dump does not do.
//============================================================================
#include "poposcl.h"

namespace NIpse {

void __fastcall CPoPosCl_ResetTailPair(CPoPosCl* self)
{
    char* base = (char*)self;
    *(unsigned int*)(base + 0xD4) = 0xFFFFFFFF;
    *(unsigned int*)(base + 0xD0) = 0xFFFFFFFF;
}

}  // namespace NIpse
