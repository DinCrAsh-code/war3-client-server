//============================================================================
//  0x6F3B47D0 - JASS_PanCameraToTimedWithZ, signature "(RRRR)V".
//
//  Twenty-two instructions: fetch the CGameUI singleton, take the camera out
//  of it, and hand the pan four values and a flag.
//
//  Two things separate this native from the rest of the family in
//  jassnatives.h.
//
//   * Its four `real` arguments are read as **hardware** floats (`fld dword
//     ptr [ecx]`), not carried on as encoded CFloat bit patterns.  The
//     camera works in hardware floats throughout - see gameui.h - so the
//     values cross into it as `float`, which is what makes the four
//     dereferences `fld`/`fstp` pairs instead of `mov`s.
//
//   * The pan's argument order is not the native's: JASS passes
//     (x, y, z, duration) and the pan takes (x, y, duration, z), so the last
//     two are swapped on the way in.  That swap is in the shipped
//     instruction order (`*z` is stored to the *fourth* outgoing slot, and
//     `*duration` to the third), not a reading of it.
//
//  The trailing `1` is the pan's own "also drive the height ramp" flag; the
//  timed-with-Z form is the one that sets it.
//
//  GetGameUI is called as GetGameUI(1, 0) - `lea ecx,[edx+1]` off a zeroed
//  edx, which is one instruction shorter than two immediates and is what
//  every other call site of it in this repo already compiles to.
//
//  Own translation unit with the other natives' conventions: see
//  jassnatives.h.
//============================================================================
#include "gameui.h"

void JASS_PanCameraToTimedWithZ(const float* x, const float* y,
                                const float* z, const float* duration)
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    camera->PanTo(*x, *y, *duration, *z, 1);
}

//============================================================================
//  The rest of the 0x6F3B4xxx camera natives - one shipped module with
//  0x6F3B47D0 above, and none of them calls another, so they share this
//  translation unit.
//
//  Every one of them opens the same way: `GetGameUI(1, 0)`, then
//  CGameUI::GetCamera, then one call on the camera.  None of the ones below
//  tests either pointer first, which is in the shipped branch structure and
//  not a reading of it - the family's one exception is SetCameraPosition
//  (0x6F3B45D0), which tests both.
//
//  A JASS `real` arrives as a *pointer* to the encoded value, and the camera
//  works in hardware floats, so each one is `fld dword ptr [reg]` - see the
//  note on 0x6F3B47D0 above.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F3B46E0 - `StopCamera` "()V".  Stop everything and cancel the running
//  modifiers with it (the `1`).
//----------------------------------------------------------------------------
void __cdecl JASS_StopCamera()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    camera->StopAll(1);
}

//----------------------------------------------------------------------------
//  0x6F3B4740 - `PanCameraToTimed` "(RRR)V".  The same pan as
//  0x6F3B47D0 with no height of its own: the z is a literal zero and the
//  "drive the height ramp" flag is off.
//----------------------------------------------------------------------------
void __cdecl JASS_PanCameraToTimed(const float* x, const float* y,
                                   const float* duration)
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    camera->PanTo(*x, *y, *duration, 0.0f, 0);
}

//----------------------------------------------------------------------------
//  0x6F3B4BB0 - `CameraSetSmoothingFactor` "(R)V".
//----------------------------------------------------------------------------
void __cdecl JASS_CameraSetSmoothingFactor(const float* factor)
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    camera->SetSmoothingFactor(*factor);
}

//----------------------------------------------------------------------------
//  0x6F3B4C50 / 0x6F3B4C70 / 0x6F3B4C90 / 0x6F3B4CB0 - the four bound
//  readers, "()R".  These are what name CCameraWar3's four bound fields:
//  the X pair is +0x4D0/+0x4D8 and the Y pair the two between them.
//
//  Each returns a JASS `real`, which is the float's own bit pattern in eax -
//  the shipped `fstp [esp+var]` / `mov eax, [esp+var]` pair, which is what
//  returning a float through an int-returning entry point compiles to.
//----------------------------------------------------------------------------
int __cdecl JASS_GetCameraBoundMinX()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float value = camera->m_boundsMinX;
    return *(const int*)&value;
}

int __cdecl JASS_GetCameraBoundMinY()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float value = camera->m_boundsMinY;
    return *(const int*)&value;
}

int __cdecl JASS_GetCameraBoundMaxX()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float value = camera->m_boundsMaxX;
    return *(const int*)&value;
}

int __cdecl JASS_GetCameraBoundMaxY()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float value = camera->m_boundsMaxY;
    return *(const int*)&value;
}

//----------------------------------------------------------------------------
//  0x6F3B4CD0 - `GetCameraField` "(Hcamerafield;)R".  The `camerafield`
//  handle *is* its integer id, and CCameraWar3::GetProperty
//  (cameragetproperty.cpp) is where that numbering is established.
//----------------------------------------------------------------------------
int __cdecl JASS_GetCameraField(int field)
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float value = camera->GetProperty(field);
    return *(const int*)&value;
}

//----------------------------------------------------------------------------
//  0x6F3B4D00 / 0x6F3B4D30 / 0x6F3B4D60 - where the camera is looking,
//  "()R" each.  All three fill the same three-float buffer through
//  CCameraWar3::GetTargetPosition and return one component of it, which is
//  why each carries a 0x10-byte frame rather than reading a field.
//----------------------------------------------------------------------------
int __cdecl JASS_GetCameraTargetPositionX()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float point[3];
    camera->GetTargetPosition(point);

    float value = point[0];
    return *(const int*)&value;
}

int __cdecl JASS_GetCameraTargetPositionY()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float point[3];
    camera->GetTargetPosition(point);

    float value = point[1];
    return *(const int*)&value;
}

int __cdecl JASS_GetCameraTargetPositionZ()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float point[3];
    camera->GetTargetPosition(point);

    float value = point[2];
    return *(const int*)&value;
}

//----------------------------------------------------------------------------
//  0x6F3B4D90 / 0x6F3B4DC0 / 0x6F3B4DF0 - and where it is looking *from*,
//  "()R" each.  The same shape one slot over: 0x6F302950 is
//  GetTargetPosition's twin on the controller's slot 7 instead of slot 8.
//
//  That address carries an earlier slice's name - SSpriteHost::
//  GetAttachmentOrigin, GameUI/gameuispriteorigin.cpp - and one shipped
//  address carries exactly one C++ name, so these three call it by that
//  name and cast.  The receiver really is the camera: the shipped body's
//  `mov ecx, [ecx+34h]` is CCameraWar3::m_pController.  See the write-up.
//----------------------------------------------------------------------------
struct SSpriteHost { float* __thiscall GetAttachmentOrigin(float* out); };

int __cdecl JASS_GetCameraEyePositionX()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float point[3];
    ((SSpriteHost*)camera)->GetAttachmentOrigin(point);

    float value = point[0];
    return *(const int*)&value;
}

int __cdecl JASS_GetCameraEyePositionY()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float point[3];
    ((SSpriteHost*)camera)->GetAttachmentOrigin(point);

    float value = point[1];
    return *(const int*)&value;
}

int __cdecl JASS_GetCameraEyePositionZ()
{
    CCameraWar3* camera = ((CGameUI*)GetGameUI(1, 0))->GetCamera();

    float point[3];
    ((SSpriteHost*)camera)->GetAttachmentOrigin(point);

    float value = point[2];
    return *(const int*)&value;
}
