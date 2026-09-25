//============================================================================
//  0x6F00D9A0 / 0x6F00D9C0 - the two one-line front ends CSelectable's
//  vtable slot 104 (selectable_attachrefresh.cpp) uses to push a visual
//  attachment's Z offset and world position at the Storm singleton.
//
//  Same shape, and the same translation-unit reasoning, as
//  stormsingleton_terrain.cpp and stormnotifysingleton.cpp: the argument
//  pushes come before the GetStormSingletonA call because the whole thing
//  is one expression.
//============================================================================
#include "stormsingletona.h"

//  0x6F01F5A0 - lazysingleton.cpp.
SStormSingletonA* __fastcall GetStormSingletonA();

//----------------------------------------------------------------------------
//  0x6F00D9A0
//----------------------------------------------------------------------------
void __fastcall SetVisualAttachmentZ(int index, int onWalkable, float z)
{
    GetStormSingletonA()->SetAttachmentWalkableZ(index, onWalkable, z);
}

//----------------------------------------------------------------------------
//  0x6F00D9C0
//----------------------------------------------------------------------------
void __fastcall SetVisualAttachmentPosition(int index, const FVec3* pos)
{
    GetStormSingletonA()->SetAttachmentPosition(index, pos);
}

//----------------------------------------------------------------------------
//  0x6F00D9D0
//----------------------------------------------------------------------------
void __fastcall SetVisualAttachmentColor(int index, const unsigned int* color)
{
    GetStormSingletonA()->SetAttachmentColor(index, color);
}

//----------------------------------------------------------------------------
//  0x6F00DA00 - CUnit's own slot 104 (unit_attachmentplacement.cpp).  Two
//  calls, not one shared local: the shipped code re-fetches
//  GetStormSingletonA() for each, exactly like its three siblings above
//  re-fetch it per call rather than caching the pointer once.
//----------------------------------------------------------------------------
void __fastcall SetVisualAttachmentFlags(int index, int bit9On, int bit10On)
{
    GetStormSingletonA()->SetAttachmentFlagBit9(index, bit9On);
    GetStormSingletonA()->SetAttachmentFlagBit10(index, bit10On);
}
