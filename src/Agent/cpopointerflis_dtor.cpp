//============================================================================
//  0x6F48C950 / 0x6F48C9A0 - NIpse::CPoPoInterfLis::~CPoPoInterfLis() /
//  ScalarDeletingDestructor(unsigned int), vtable slot 2. See
//  cpopointerflis.h's own file header comment for the full shape - the
//  real, SEH-framed destructor teardown this whole sibling family
//  otherwise skips (its own siblings restamp straight to
//  `??_7CPresence@NTempest@@6B@` with no base-teardown call at all).
//
//  Own translation unit, deliberately - kept out of line from
//  ScalarDeletingDestructor so /Ob2 does not inline the whole body into it
//  and lose the real `call` the dump has, the same
//  Agent/poposbh_dtor.cpp's own convention this mirrors.
//
//  ~40 instructions unreproducible: this destructor has an
//  __except_handler4-shaped SEH frame (destructible-local idiom,
//  docs/msvc-vc8-idioms.md's own section) this build's fixed
//  /GS- /EHs-c- cannot reach with the bundled compiler - the same accepted
//  gap poposbh_dtor.cpp's own header already documents for the identical
//  shape. The two real instructions the frame wraps (the +0x58 CMemBlock
//  release, the CPresence vtable restamp) are present and in order.
//============================================================================
#include "cpopointerflis.h"
#include "cellbuffer.h"
#include "storm.h"

namespace NIpse {

CPoPoInterfLis::~CPoPoInterfLis()
{
    ReleaseCMemBlockBase(&m_array.m_storage);
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@
}

void* CPoPoInterfLis::ScalarDeletingDestructor(unsigned int flags)
{
    this->~CPoPoInterfLis();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
