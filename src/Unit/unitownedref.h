//============================================================================
//  0x6F02C960 / 0x6F03ED80 - two byte-identical "release this reference and
//  clear it" bodies at two addresses, reached by CUnit::Deactivate
//  (unit_deactivate.cpp) on three of its own fields (+0x128 and +0x224 go
//  to the first, +0x12C to the second).
//
//  Two addresses and one body is this image's usual state, not a puzzle:
//  it was linked without /OPT:ICF (agentdefaults.cpp), so two classes'
//  identically-shaped members both survive.  They are two types here for
//  the same reason - the call sites pick one or the other by field, which
//  a single shared type could not express.
//
//  Each is a one-pointer wrapper reached by the *field's* address
//  (`lea ecx, [esi+128h]`), so `this` is the field, not the unit.  The
//  body: if the pointer is set, run its own vtable slot 2 (+0x08), re-read
//  the pointer - the shipped code reloads it, so slot 2 is allowed to have
//  changed it - drop a reference and destroy through vtable slot 0 if that
//  was the last, then null the field.
//============================================================================
#ifndef UNITOWNEDREF_H
#define UNITOWNEDREF_H

#include "refcnt.h"

//  0x6F02C960
struct SOwnedAgentRef
{
    void Release();

    TRefCnt* m_p;   // +0x00
};

//  0x6F03ED80 - the same body, a different owner.
struct SOwnedQueueRef
{
    void Release();

    TRefCnt* m_p;   // +0x00
};

#endif
