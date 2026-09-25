//============================================================================
//  0x6F3C0830 - ResolveCameraSetupHandle: turn a JASS `camerasetup` handle
//  into the object behind it, or null.
//
//  The camerasetup twin of ResolveLocationHandle (0x6F3BD3B0,
//  jassresolvelocationhandle.cpp): the same 115-instruction body with
//  SCheckedCameraSetupSlot in place of SCheckedLocationSlot.  Both of its
//  constructor calls are 0x6F3B9C00, which is what settles which slot type
//  it stages through rather than inferring it.
//
//  Its only caller in this batch is CameraSetupGetDestPositionLoc
//  (jassnatives_camerasetuploc.cpp), which is also what names it: the
//  native's registered signature is "(Hcamerasetup;)Hlocation;".
//
//  Own translation unit, next to the other resolvers, all of which are in
//  the JASS-resolver module and none of which calls another.
//
//  The two load-bearing shapes are inherited from those twins: the
//  singleton is fetched *before* the null test on the handle, and the
//  failure path constructs a second slot from 0 and returns its value
//  rather than a bare `return 0;`.
//============================================================================
#include "jassnatives.h"
#include "agenttypedslots.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"
#include "game.h"

//  Only the fields this native reaches: the destination position the
//  `camerasetup` object carries as three consecutive encoded reals.
//  jassnatives_camerasetuploc.cpp copies all three out.
struct SJassCameraSetup
{
    char      m_reserved00[0xA4];
    CWorldVec3 m_destination;   // +0xA4 .. +0xAC
};

SJassCameraSetup* __fastcall ResolveCameraSetupHandle(int hCameraSetup)
{
    SItemSlotTableHolder* singleton = g_pItemSlotHost->GetSlotTable();

    if (!hCameraSetup)
        return 0;

    CAgent* candidate = singleton->QueryItemSlot(hCameraSetup);
    SCheckedCameraSetupSlot checked(candidate);

    CAgent* setup = checked.m_value;
    bool validated = false;
    if (setup)
    {
        SItemHandleObject* handleObj =
            (SItemHandleObject*)LookupHandle(setup->m_handle,
                                             setup->m_typeTag);
        if (handleObj)
        {
            SItemHandleObject* tagOk =
                (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
            if (tagOk)
                validated = (tagOk->m_pendingFlag == 0);
        }
    }

    if (validated)
        return (SJassCameraSetup*)checked.m_value;

    SCheckedCameraSetupSlot fallback(0);
    return (SJassCameraSetup*)fallback.m_value;
}
