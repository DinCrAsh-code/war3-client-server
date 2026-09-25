//============================================================================
//  0x6F474830 - see notifyhandlemoved.cpp.
//
//  Declared in a header rather than at each of its two sites: the caller
//  (widgetinitsprite.cpp) and the definition have to agree exactly, and a
//  `void*` here against an `SHandleWithType*` there mangles into two
//  different symbols that never meet - the single most common defect in
//  docs/notes/verifier-gate-link-failures.md.
//
//  This used to carry its own copy of `struct SHandleWithType` - the same
//  {m_reserved00[8], m_handle, m_type} shape timesyncbounds.h's own copy
//  already had, just with a different subset of methods declared on it.
//  Two full definitions of one struct compile fine on their own and only
//  fail (C2011, "type redefinition") the moment some translation unit
//  includes both headers - which nothing did until destructable.h started
//  pulling in item.h (timesyncbounds.h) and this header together. Merged
//  into timesyncbounds.h's copy (this file's own `NotifyHandleObjectMoved`
//  moved there) rather than the other way around, since that header is the
//  one already included far more widely.
//============================================================================
#ifndef HANDLEMOVED_H
#define HANDLEMOVED_H

#include "timesyncbounds.h"

#endif
