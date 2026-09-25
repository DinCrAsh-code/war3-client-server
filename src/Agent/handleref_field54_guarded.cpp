//============================================================================
//  0x6F021A00 - QueryHandleField0x54 (itemhandleresolve.cpp), but with the
//  "not set" sentinel test (SOptionalHandleRef's own header comment) hoisted
//  in front of the call so this call site never even resolves the handle for
//  an unset ref.  Own TU: this address is nowhere near itemhandleresolve.cpp's
//  module, it just happens to tail-call into it (a normal cross-module call,
//  not something to inline).  Reached with a missile's own {handle,typeTag}
//  pair at +0x2C as `ref` - it is a shared utility, not missile-specific.
//============================================================================
#include "itemhandleresolve.h"

void* __fastcall QueryHandleField0x54IfSet(SOptionalHandleRef* ref)
{
    if ((ref->m_typeTag & ref->m_handle) == -1)
        return 0;
    return QueryHandleField0x54(ref);
}
