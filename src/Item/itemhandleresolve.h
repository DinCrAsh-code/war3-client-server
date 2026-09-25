//============================================================================
//  JASS_SetItemPosition's own item-handle resolution chain - see
//  jasssetitemposition_native.cpp for the root and the two functions
//  declared here.
//============================================================================
#ifndef ITEMHANDLERESOLVE_H
#define ITEMHANDLERESOLVE_H

#include "game.h"

//----------------------------------------------------------------------------
//  A nullable {handle, typeTag} pair - the "not set" sentinel is both
//  fields all-ones, tested as (handle & typeTag) == -1 rather than two
//  separate comparisons (both fields must individually be all-ones for an
//  AND of them to equal all-ones).  Reached both as a standalone struct
//  (QueryHandleField0x54's own `this`) and embedded at a fixed offset
//  inside the item-position root's own resolved object.
//----------------------------------------------------------------------------
struct SOptionalHandleRef
{
    //  Trivial - purely so Position's own forced-inline constructor
    //  (position.h) can initialise its embedded instance the same way it
    //  initialises every other field, C++03-legal under this compiler.
    SOptionalHandleRef(unsigned int handle, int typeTag)
        : m_handle(handle), m_typeTag(typeTag) {}

    unsigned int    m_handle;   // +0x00
    int             m_typeTag;  // +0x04
};

//  0x6F4786B0 - LookupHandle(ref), then the same m_pendingFlag guard every
//  other reader in this chain uses, then SItemHandleObject::m_field54.
void* __fastcall QueryHandleField0x54(SOptionalHandleRef* ref);

#endif
