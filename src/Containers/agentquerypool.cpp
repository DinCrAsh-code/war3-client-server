//============================================================================
//  TSFixedArray for CGameData::m_queryPool - the stack of AgileAgentQuery
//  scratch objects every agent enumerator borrows one of.
//
//  0x6F469880 SetAlloc, 0x6F46A950 SetCount.  Both in one translation unit
//  for the reason tsarray.cpp's three are: SetCount's call into SetAlloc
//  stays a real call at this size, which is what the shipped stream shows.
//============================================================================
#include "agentquery.h"
#include "tsfixedarray.inl"

template void TSFixedArray<AgileAgentQuery*>::SetAlloc(unsigned int);
template void TSFixedArray<AgileAgentQuery*>::SetCount(unsigned int);
