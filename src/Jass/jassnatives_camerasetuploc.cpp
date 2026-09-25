//============================================================================
//  0x6F3D3E40 - `CameraSetupGetDestPositionLoc`, "(Hcamerasetup;)Hlocation;".
//
//  Resolve the handle, copy the setup's destination position out of
//  +0xA4/+0xA8/+0xAC, and hand the x/y pair to the `Location` native
//  (jassnatives_locationmake.cpp), whose fresh location handle is the
//  result.  The z is copied and never used - the shipped code stores it
//  into the third slot of the local and never reads it again, which is a
//  whole-object copy of the three-component position and not two field
//  reads.
//
//  Own translation unit: two real calls out of it, one of them the
//  resolver.
//============================================================================
#include "jassnatives.h"
#include "game.h"

//  jassresolvecamerasetuphandle.cpp.  Declared here rather than in
//  jassnatives.h because this is its only caller.
struct SJassCameraSetup
{
    char       m_reserved00[0xA4];
    CWorldVec3 m_destination;   // +0xA4 .. +0xAC
};

SJassCameraSetup* __fastcall ResolveCameraSetupHandle(int hCameraSetup);

//  0x6F3D03B0 - jassnatives_locationmake.cpp.
int __cdecl JASS_Location(const int* x, const int* y);

int __cdecl JASS_CameraSetupGetDestPositionLoc(int hCameraSetup)
{
    SJassCameraSetup* setup = ResolveCameraSetupHandle(hCameraSetup);

    if (!setup)
        return 0;

    CWorldVec3 dest = setup->m_destination;

    return JASS_Location((const int*)&dest.m_x, (const int*)&dest.m_y);
}
