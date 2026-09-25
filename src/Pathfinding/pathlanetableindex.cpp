//============================================================================
//  0x6F49C690 - CPathClient::SetLaneTableIndex.  Drop this client out of
//  whichever lane it currently sits on (ReleaseLane, 0x6F49B5A0, which
//  itself clears kPathFlagSearched - pathwaypointmisc.cpp), then restamp
//  the 4-bit lane-table group index at m_flags bits 16-19: the exact field
//  Lane() itself reads back via `(m_flags >> 16) & 0xF` (pathmove.h).
//
//  Reached from this agent's own CUnit vtable BFS slice.  A pre-existing
//  naked `Callee_6F49C690` redirect in cunit_agent6_naked.cpp forwards a
//  different, already-reconstructed caller straight into this address in
//  Game.dll; that thunk's own comment already anticipated this - "a
//  cunit_agent5_worklist.md worklist entry for a different concurrent
//  agent's slice" - and is left alone here since it is a naked jmp into
//  the real shipped function regardless (identical behaviour either way,
//  and the call site reaches it through a raw `call` inside its own
//  __asm block rather than through this declared C++ signature).
//============================================================================
#include "pathmove.h"

void CPathClient::SetLaneTableIndex(unsigned char index)
{
    ReleaseLane();
    m_flags = (m_flags & ~0x000F0000u) | ((unsigned int)index << 16);
}
