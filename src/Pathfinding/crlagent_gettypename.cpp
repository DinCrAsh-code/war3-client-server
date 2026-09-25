//============================================================================
//  0x6F480320 - NIpse::CRlAgent's vtable slot 0.  A trivial accessor with no
//  `this` read at all: returns the literal string "RlAgent" (aRlagent,
//  0x6F9520C8, the string immediately following this class's own vtable
//  data in the image).  `retn` with zero stack bytes.
//============================================================================
#include "crlagent.h"

namespace NIpse {

const char* CRlAgent_GetTypeName()
{
    return "RlAgent";
}

}  // namespace NIpse
