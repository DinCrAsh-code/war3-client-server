//============================================================================
//  0x6F2556B0 - SImpactContext::SImpactContext, the constructor of the
//  0x24-byte block CMissile::OnArrival builds on its own
//  stack (missile_arrival.cpp).  A naked redirect there until now.
//
//  Seven stores and no reads.  Two of them are g_CFloatZero, and the
//  shipped code **loads that global twice** rather than keeping it in a
//  register across the five stores between them - which is what two
//  separate CFloat copy-initialisations look like and what one register
//  reused would not.  Both loads are kept here for that reason.
//
//  +0x04 and +0x08 are never written.  They are left in the reserved block
//  rather than guessed at, per CLAUDE.md's rule about untouched layout.
//
//  Own translation unit: OnArrival constructs the block with a real call,
//  which is exactly what a naked body was standing in for.
//============================================================================
#include "impactcontext.h"
#include "CFloat.h"

__thiscall SImpactContext::SImpactContext()
    //  Only +0x10 is initialised from the list.  +0x18 is left uninitialised
    //  here and assigned in the body, because the shipped store order -
    //  +0x10, +0x14, +0x00, +0x0C, +0x18, +0x1C, +0x20 - puts four other
    //  stores between the two, and a member-initialiser list runs in
    //  *declaration* order, which no declaration of these fields can make
    //  match.  kCFloatNoInit compiles to nothing, so the split costs no
    //  instruction.
    : m_float10(g_CFloatZero)
    , m_float18(kCFloatNoInit)
{
    m_zero14 = 0;
    m_zero00 = 0;
    m_zero0C = 0;
    m_float18 = g_CFloatZero;
    m_zero1C = 0;
    m_zero20 = 0;
}
