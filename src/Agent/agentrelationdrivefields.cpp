//============================================================================
//  0x6F4A9A50 / 0x6F4A9FF0 / 0x6F4AA010 - the driven relation's own three
//  field accessors.
//
//  The two setters are the same function twice over one field each, and
//  both carry the same second parameter: a flag saying whether to re-sample
//  the drive straight away.  Their one caller sets the mode with it clear
//  and the target with it set (FloatListener::SetBoundary,
//  src/Unit/floatlistenerboundary.cpp), so a boundary change costs one
//  sample and not two - which is what the flag is for.
//
//  Neither setter tail-calls Resample(): the shipped `call` / `retn 8` is
//  what a callee-cleanup function with two stack arguments has to do, since
//  the tail callee cleans none of them.
//
//  Own translation unit: Resample() calls GetTrace() for real and both
//  setters call Resample() for real, so this file must hold neither
//  Resample() nor anything Resample() reaches.
//============================================================================
#include "agentrelation.h"

//----------------------------------------------------------------------------
//  0x6F4A9A50
//----------------------------------------------------------------------------
CPathTrace* CDrivenRelation::GetTrace()
{
    return (CPathTrace*)m_endpointB;
}

//----------------------------------------------------------------------------
//  0x6F4A9FF0
//----------------------------------------------------------------------------
void CDrivenRelation::SetTarget(const CFloat* target, int resample)
{
    m_target = *target;

    if (resample)
        Resample();
}

//----------------------------------------------------------------------------
//  0x6F4AA010
//----------------------------------------------------------------------------
void CDrivenRelation::SetMode(int mode, int resample)
{
    m_mode = mode;

    if (resample)
        Resample();
}
