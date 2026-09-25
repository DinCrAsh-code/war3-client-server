//============================================================================
//  0x6F38ECD0 - CWorldFrameWar3::SetCamera: park the camera in the world
//  frame's own checked '+cam' slot at +0x19C.
//
//  Two instructions in the shipped image - `add ecx, 19Ch` and a jump into
//  SCheckedCameraSlot::Assign (0x6F2F49E0) - which is exactly what a
//  tail-call to a member of a member at a non-zero offset compiles to.  The
//  slot's return value is dropped.
//
//  This was a redirect in GameUI/gameui_thunks.cpp until now; the redirect
//  is deleted with this file landing, because two C++ names for one shipped
//  address is what verify.py scores instead of the body.
//============================================================================
#include "gameui.h"
#include "agenttypedslots.h"

void CWorldFrameWar3::SetCamera(CCameraWar3* camera)
{
    m_camera.Assign((CAgent*)camera);
}
