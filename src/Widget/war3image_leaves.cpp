//============================================================================
//  CWar3Image's constant leaves.
//
//  Two of them sit in the 0x6F26Bxxx cluster that holds the rest of this
//  class's own body - the nine sprite and animation slots war3image.h
//  describes - and the third (slot 38) is a bare `retn` over in 0x6F03Axxx.
//  None of the three calls anything, so grouping them costs nothing the
//  shipped build kept out of line.
//
//  CWar3Image's slot 1 is *not* here: it inlines ~CAgentWar3 and therefore
//  has to live in agentwar3_dtor.cpp.  See that file.
//============================================================================
#include "war3image.h"

//----------------------------------------------------------------------------
//  0x6F26B760 - slot 22.
//----------------------------------------------------------------------------
const char* CWar3Image::GetClassName()
{
    return "CWar3Image";
}

//----------------------------------------------------------------------------
//  0x6F26B7B0 - slot 7.  '+','w','3','i' low byte first, between
//  CAgentWar3's '+w3a' (agentwar3_slots.cpp) and CWidget's '+w3w'
//  (widget_vtable_leaves.cpp) in the same family.
//----------------------------------------------------------------------------
unsigned int CWar3Image::GetAgileTypeId() const
{
    return 0x2B773369;
}

//----------------------------------------------------------------------------
//  0x6F03A9A0 - slot 38, IDA's nullsub_198.  `retn`, no arguments.  This is
//  the slot CWidget overrides with real work (widget_visibleflags.cpp); the
//  class that owns the sprite has nothing to do when visibility changes
//  because the sprite is not yet the thing being shown.
//----------------------------------------------------------------------------
void CWar3Image::NotifyWidgetVisibilityChanged() {}
