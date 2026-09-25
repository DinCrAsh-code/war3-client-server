//============================================================================
//  0x6F3C3310 - the ability the current spell event is about.  Shared by
//  GetSpellAbility, GetSpellAbilityUnit, GetSpellTargetX/Y/Unit/Item/
//  Destructable and GetSpellTargetLoc.
//
//  Own translation unit: it issues genuine calls to GetSpellEventOwner and
//  GetSpellEventData (jasseventaccessors.cpp), which are its own module's
//  neighbours and exactly the size /Ob2 would inline.
//
//  A switch and not two range tests: the shipped code indexes a 22-entry
//  byte map at 0x6F3C3364 and jumps through jpt_6F3C332B, which is MSVC's
//  dense-switch lowering and not something a pair of `if`s produces.  The
//  two id ranges are the same ones jasseventaccessors.cpp's own pair uses.
//============================================================================
#include "jassevents.h"

void* GetSpellEventAbility()
{
    CScriptEventData* data;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80310: case 0x80311: case 0x80312: case 0x80313: case 0x80314:
    {
        SSpellEventOwner* owner = (SSpellEventOwner*)GetSpellEventOwner();
        if (!owner)
            return 0;
        data = (CScriptEventData*)owner->GetEventData();
        break;
    }

    case 0x80321: case 0x80322: case 0x80323: case 0x80324: case 0x80325:
        data = (CScriptEventData*)GetSpellEventData();
        break;

    default:
        return 0;
    }

    if (!data)
        return 0;

    return data->GetAbility();
}
