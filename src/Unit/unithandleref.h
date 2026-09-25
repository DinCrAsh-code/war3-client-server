//============================================================================
//  "If this handle pair is set, resolve it and tell whatever it names that
//  it is going away" - the block CUnit::Deactivate (unit_deactivate.cpp)
//  has inlined twice and calls out of line once
//  (0x6F2E5A70, unit_notifyref304.cpp).
//
//  It is in a header rather than a .cpp because the shipped code has both
//  shapes: two inlined copies and one real call.  A .cpp definition could
//  only ever produce calls.
//
//  The "not set" test is the same one itemhandleresolve.h documents - both
//  words all-ones, tested as a single AND against -1 - and the *order* of
//  the two loads is the shipped order: the type tag at +0x04 first, the
//  handle at +0x00 second.
//============================================================================
#ifndef UNITHANDLEREF_H
#define UNITHANDLEREF_H

#include "itemhandleresolve.h"

//  The resolved object's own vtable+0x5C (slot 23), no arguments.
typedef void (__thiscall *NotifyDeactivatedFn)(void* self);

inline void ReleaseHandleRefNotify(SOptionalHandleRef* ref)
{
    if ((ref->m_typeTag & (int)ref->m_handle) != -1)
    {
        void* obj = QueryHandleField0x54(ref);
        if (obj != 0)
            ((NotifyDeactivatedFn)(*(void***)obj)[0x5C / 4])(obj);
    }
}

#endif
