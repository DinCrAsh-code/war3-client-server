//============================================================================
//  SImpactContext - the 0x24-byte block CMissile::OnArrival
//  builds on its stack and hands to the last impact gate
//  (missile_arrival.cpp).
//
//  Its size is the gap the shipped frame leaves between that local and the
//  exception-handling slots above it.  What its constructor writes is now
//  known (0x6F2556B0, impactcontext_ctor.cpp) and the fields it writes are
//  named for their offsets; nothing in this repo *reads* one, so nothing
//  here claims what any of them mean.
//
//  The header exists because the constructor and its caller are in two
//  translation units - the call has to stay a real call - and a per-file
//  copy of a layout is how offsets drift.
//============================================================================
#ifndef IMPACTCONTEXT_H
#define IMPACTCONTEXT_H

#include "CFloat.h"

struct SImpactContext
{
    //  0x6F2556B0 - `retn 0`, no arguments.
    __thiscall SImpactContext();

    int  m_zero00;          // +0x00
    //  +0x04 / +0x08 - never written by the constructor.
    char m_reserved04[8];
    int  m_zero0C;          // +0x0C
    CFloat m_float10;       // +0x10 - g_CFloatZero
    int  m_zero14;          // +0x14
    CFloat m_float18;       // +0x18 - g_CFloatZero, loaded a second time
    int  m_zero1C;          // +0x1C
    int  m_zero20;          // +0x20
};

#endif
