//============================================================================
//  0x6F302920 - CCameraWar3::GetTargetPosition: zero the caller's
//  three-float point and let the live controller overwrite it with slot 8,
//  which is where the camera is looking.
//
//  Byte for byte the same body as CCameraWar3::QueryControllerVec3
//  (0x6F3026E0, cameraquerycontrollervec3.cpp) - same controller, same slot
//  8 - and a separate shipped function all the same.  The two are kept
//  apart because one shipped address carries exactly one C++ name; what
//  distinguishes this one is its callers, the three JASS natives
//  GetCameraTargetPositionX/Y/Z, which read x, y and z out of the buffer it
//  fills.
//
//  Its eye-position twin is 0x6F302950, the identical body on slot 7, which
//  this repo already carries under an earlier slice's name
//  (SSpriteHost::GetAttachmentOrigin, gameuispriteorigin.cpp) - see the
//  target write-up.
//
//  Own translation unit: the controller query is a real call.
//============================================================================
#include "gameui.h"
#include "spriteattach.h"

float* CCameraWar3::GetTargetPosition(float* out)
{
    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 0.0f;

    QuerySlotVec3(m_pController, 8, out);
    return out;
}
