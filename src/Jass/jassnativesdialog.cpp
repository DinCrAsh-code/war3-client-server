//============================================================================
//  0x6F3C1DE0 / 0x6F3C1E30 - GetClickedButton / GetClickedDialog, right
//  after the trigger property module.  Neither calls the other.
//============================================================================
#include "jassevents.h"
#include "itemhandlemain.h"
#include "agentregistry.h"

//----------------------------------------------------------------------------
//  0x6F3C1DE0 - `GetClickedButton` "()Hbutton;".  Live for two adjacent id
//  offsets (0 and 2 past 0x8025A - the shipped two-branch shape, not a
//  contiguous range), then resolves the current event data's own
//  clicked-button ref and registers it as an agent.
//----------------------------------------------------------------------------
void* GetClickedButton()
{
    unsigned int rel = (unsigned int)(GetCurrentEventId() + kEventIdBias) - 0x8025A;

    //  Two nested checks, not one range test - the shipped code really is
    //  `jz ok; sub eax,2; jnz null`, not a collapsed `<=2` (see
    //  jasseventaccessors.cpp's own note on this family's bias shape).
    if (rel != 0)
    {
        if (rel - 2 != 0)
            return 0;
    }

    CScriptEventData* data = GetCurrentScriptEventData();
    if (!data)
        return 0;

    void* button = data->GetClickedButtonAbility();

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (void*)registry->Register((CUnitAgent*)button, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C1E30 - `GetClickedDialog` "()Hdialog;".  Same two-offset id check
//  as GetClickedButton, then the event data's own +0x20 ref (shared with
//  GetExpiredTimer/GetTournamentFinishNowRule/Player - see jassevents.h)
//  resolved the ordinary way and registered as an agent.
//----------------------------------------------------------------------------
void* GetClickedDialog()
{
    unsigned int rel = (unsigned int)(GetCurrentEventId() + kEventIdBias) - 0x8025A;

    if (rel != 0)
    {
        if (rel - 2 != 0)
            return 0;
    }

    CScriptEventData* data = GetCurrentScriptEventData();
    if (!data)
        return 0;

    void* dialog = 0;
    if ((data->m_ownerRef.m_typeTag & data->m_ownerRef.m_handle) != -1)
        dialog = QueryHandleField0x54(&data->m_ownerRef);

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (void*)registry->Register((CUnitAgent*)dialog, 0);
}
