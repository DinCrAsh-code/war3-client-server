//============================================================================
//  0x6F4332A0 - CScriptEventData::GetClickedButtonAbility.  Byte-identical
//  to CScriptEventData::GetAbility (0x6F26FDB0, scripteventpoint.cpp) -
//  same +0x2C/+0x30 ref, same inlined guard, same tail jump into
//  QueryHandleField0x54 - but its own distinct shipped address (no
//  /OPT:ICF in this image), reached only from GetClickedButton
//  (jassnativesdialog.cpp).
//============================================================================
#include "jassevents.h"

void* CScriptEventData::GetClickedButtonAbility()
{
    if ((m_abilityTypeTag & m_abilityHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_abilityHandle);
}
