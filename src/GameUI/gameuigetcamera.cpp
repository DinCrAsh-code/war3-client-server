//============================================================================
//  0x6F2F5ED0 - CGameUI::GetCamera.
//
//  Two instructions, and until this target nothing said what the dword at
//  +0x254 was, so it carried the name a small-accessor sweep gave it:
//  GetField0x254_6F2F5ED0, in src/Misc/trivialaccessors_02_03.cpp.  The JASS
//  camera natives settle it - JASS_PanCameraToTimedWithZ reads this field off
//  the CGameUI singleton and hands the result straight to CCameraWar3::PanTo
//  as its `this` - so the field is the camera and the getter is GetCamera.
//  Renamed and moved here, into the module it belongs to (docs/notes/
//  naming-standard.md, rules 1 and 3: no address in a name, one translation
//  unit per shipped module).
//============================================================================

#include "gameui.h"

CCameraWar3* CGameUI::GetCamera() const
{
    return m_pCamera;
}
