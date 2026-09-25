//============================================================================
//  0x6F4B38F0 - NTempest::CMemBlock::`scalar deleting destructor'(unsigned int).
//  See cmemblock.h.  Calls the already-reconstructed
//  ReleaseCMemBlockBase(this) (Pathfinding/cellbuffer.h/.cpp - a naked
//  redirect to 0x6F4B3770, which stamps CMemBlock's own vtable itself, so
//  this function does not stamp it a second time), then frees `this` when
//  bit 0 of `flags` is set, guarded by `this != 0` - the raw bytes carry a
//  `test esi,esi` this file's sibling centity_dtor.cpp's own trivial
//  version does not.
//============================================================================
#include "cmemblock.h"
#include "cellbuffer.h"
#include "storm.h"

namespace NTempest {

void* CMemBlock::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
