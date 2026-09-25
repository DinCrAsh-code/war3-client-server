//============================================================================
//  0x6F4937B0 - CPathTrace::SetFieldB4.
//
//  A plain raw-dword copy: `*(int*)&m_fieldB4 = *(const int*)value`, not
//  through CFloat's own copy assignment (which would emit the same single
//  `mov` here regardless - a CFloat is one dword - so there is nothing to
//  distinguish from the instruction stream alone; declared as an
//  `unsigned int` field per CLAUDE.md's "do not invent fields the call
//  tree does not read", since nothing in this call tree ever reads it
//  back as a float or otherwise).
//
//  The one caller in this repo is SmartPositionTrack::SetMaxSpeed
//  (Pathfinding/smartpositiontrackspeed.cpp), called immediately after its
//  own CPathTrace::SetMaxSpeed call with the identical grid-unit CFloat
//  pointer - see that file's own comment for why a second call exists here
//  at all.
//
//  Own translation unit: 0x6F4937B0 is a different module from both
//  0x6F476xxx (smartpositiontrackspeed.cpp) and 0x6F4A7xxx
//  (pathtracesetmaxspeed.cpp), and this makes no call of its own.
//============================================================================
#include "game.h"

void CPathTrace::SetFieldB4(const CFloat* value)
{
    m_fieldB4 = *(const unsigned int*)value;
}
