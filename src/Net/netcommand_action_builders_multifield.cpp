//============================================================================
//  BuildAndFire wrappers for the independent multi-field CNetCommandXxx
//  classes (netcommand_classes_multifield.h) - same shape as every other
//  builder in this module (netcommand_pause.cpp's own exemplar).
//============================================================================
#include "netcommand_classes_multifield.h"
#include "cdatastore.h"

void* __fastcall CNetCommandReviveCancel_Attach(CDataStore* reader, CNetCommandReviveCancel* cmd);
void* __fastcall CNetCommandUnitSelectionEvent_Attach(CDataStore* reader, CNetCommandUnitSelectionEvent* cmd);
void* __fastcall CNetCommandSelectableSelectionModify_Attach(CDataStore* reader, CNetCommandSelectableSelectionModify* cmd);
void* __fastcall CNetCommandUnitSelectControlGroup_Attach(CDataStore* reader, CNetCommandUnitSelectControlGroup* cmd);
void* __fastcall CNetCommandUnitSelectSubGroup_Attach(CDataStore* reader, CNetCommandUnitSelectSubGroup* cmd);
void* __fastcall CNetCommandCheatCritter_Attach(CDataStore* reader, CNetCommandCheatCritter* cmd);
//  sub_6F554BF0 - claimed as NetMsgReadByteDwordAt18_6F554BF0 (netmsgio_554.cpp)
//  by an earlier session; same body (ReadByte@+0x18, ReadDword@+0x1C), so
//  CheatGold's BuildAndFire calls it directly instead of a second wrapper.
CDataStore* __fastcall NetMsgReadByteDwordAt18_6F554BF0(CDataStore* self, unsigned char* dst);
void* __fastcall CNetCommandCheatLumber_Attach(CDataStore* reader, CNetCommandCheatLumber* cmd);
void* __fastcall CNetCommandCheatResources_Attach(CDataStore* reader, CNetCommandCheatResources* cmd);
void* __fastcall CNetCommandCheatSetTOD_Attach(CDataStore* reader, CNetCommandCheatSetTOD* cmd);
void* __fastcall CNetCommandTeamChangeAlliance_Attach(CDataStore* reader, CNetCommandTeamChangeAlliance* cmd);
void* __fastcall CNetCommandTeamTradeResources_Attach(CDataStore* reader, CNetCommandTeamTradeResources* cmd);
void* __fastcall CNetCommandPlayerChatMatch_Attach(CDataStore* reader, CNetCommandPlayerChatMatch* cmd);
//  0x6F53C210 - netcommand_fire.cpp's own second copy of FireCommand,
//  called only from here (see that file's comment on it).
void __fastcall CNetCommandPlayerChatMatch_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                         const char* name, int verbose, unsigned int turn);
void* __fastcall CNetCommandResumeTriggerExec_Attach(CDataStore* reader, CNetCommandResumeTriggerExec* cmd);
void* __fastcall CNetCommandTriggerSyncReady_Attach(CDataStore* reader, CNetCommandTriggerSyncReady* cmd);
void* __fastcall CNetCommandTrackableHit_Attach(CDataStore* reader, CNetCommandTrackableHit* cmd);
void* __fastcall CNetCommandTrackableTrack_Attach(CDataStore* reader, CNetCommandTrackableTrack* cmd);
void* __fastcall CNetCommandDialogButtonClick_Attach(CDataStore* reader, CNetCommandDialogButtonClick* cmd);
void* __fastcall CNetCommandDialogClick_Attach(CDataStore* reader, CNetCommandDialogClick* cmd);
void* __fastcall CNetCommandAllyPing_Attach(CDataStore* reader, CNetCommandAllyPing* cmd);

//----------------------------------------------------------------------------
//  0x6F541170 - CNetCommandReviveCancel (wire type 0xA001D).
//----------------------------------------------------------------------------
void __fastcall CNetCommandReviveCancel_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandReviveCancel cmd(ctx, sender);
    CNetCommandReviveCancel_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F540FD0 - CNetCommandUnitSelectionEvent (wire type 0xA001B).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitSelectionEvent_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitSelectionEvent cmd(ctx, sender);
    CNetCommandUnitSelectionEvent_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5410A0 - CNetCommandSelectableSelectionModify (wire type 0xA001C).
//----------------------------------------------------------------------------
void __fastcall CNetCommandSelectableSelectionModify_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSelectableSelectionModify cmd(ctx, sender);
    CNetCommandSelectableSelectionModify_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F540D60 - CNetCommandUnitSelectControlGroup (wire type 0xA0018).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitSelectControlGroup_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitSelectControlGroup cmd(ctx, sender);
    CNetCommandUnitSelectControlGroup_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F540E30 - CNetCommandUnitSelectSubGroup (wire type 0xA0019).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitSelectSubGroup_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitSelectSubGroup cmd(ctx, sender);
    CNetCommandUnitSelectSubGroup_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5414C0 - CNetCommandCheatCritter (wire type 0xA0021).
//----------------------------------------------------------------------------
void __fastcall CNetCommandCheatCritter_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatCritter cmd(ctx, sender);
    CNetCommandCheatCritter_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5418E0 - CNetCommandCheatGold (wire type 0xA0027).
//----------------------------------------------------------------------------
void __fastcall CNetCommandCheatGold_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatGold cmd(ctx, sender);
    NetMsgReadByteDwordAt18_6F554BF0(reader, (unsigned char*)&cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5419B0 - CNetCommandCheatLumber (wire type 0xA0028).
//----------------------------------------------------------------------------
void __fastcall CNetCommandCheatLumber_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatLumber cmd(ctx, sender);
    CNetCommandCheatLumber_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541DC0 - CNetCommandCheatResources (wire type 0xA002D).
//----------------------------------------------------------------------------
void __fastcall CNetCommandCheatResources_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatResources cmd(ctx, sender);
    CNetCommandCheatResources_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541E90 - CNetCommandCheatSetTOD (wire type 0xA002E).
//----------------------------------------------------------------------------
void __fastcall CNetCommandCheatSetTOD_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatSetTOD cmd(ctx, sender);
    CNetCommandCheatSetTOD_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5422B0 - CNetCommandTeamChangeAlliance (wire type 0xA0050).
//----------------------------------------------------------------------------
void __fastcall CNetCommandTeamChangeAlliance_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandTeamChangeAlliance cmd(ctx, sender);
    CNetCommandTeamChangeAlliance_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542380 - CNetCommandTeamTradeResources (wire type 0xA0051).
//----------------------------------------------------------------------------
void __fastcall CNetCommandTeamTradeResources_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandTeamTradeResources cmd(ctx, sender);
    CNetCommandTeamTradeResources_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542450 - CNetCommandPlayerChatMatch (wire type 0xA0060).  Fires
//  through its own duplicate FireCommand (0x6F53C210, netcommand_fire.cpp)
//  rather than the canonical one every other builder in this file uses.
//----------------------------------------------------------------------------
void __fastcall CNetCommandPlayerChatMatch_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandPlayerChatMatch cmd(ctx, sender);
    CNetCommandPlayerChatMatch_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetCommandPlayerChatMatch_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542620 - CNetCommandResumeTriggerExec (wire type 0xA0062).
//----------------------------------------------------------------------------
void __fastcall CNetCommandResumeTriggerExec_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandResumeTriggerExec cmd(ctx, sender);
    CNetCommandResumeTriggerExec_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542700 - CNetCommandTriggerSyncReady (wire type 0xA0063).
//----------------------------------------------------------------------------
void __fastcall CNetCommandTriggerSyncReady_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandTriggerSyncReady cmd(ctx, sender);
    CNetCommandTriggerSyncReady_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5427E0 - CNetCommandTrackableHit (wire type 0xA0064).
//----------------------------------------------------------------------------
void __fastcall CNetCommandTrackableHit_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandTrackableHit cmd(ctx, sender);
    CNetCommandTrackableHit_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5428C0 - CNetCommandTrackableTrack (wire type 0xA0065).
//----------------------------------------------------------------------------
void __fastcall CNetCommandTrackableTrack_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandTrackableTrack cmd(ctx, sender);
    CNetCommandTrackableTrack_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5429A0 - CNetCommandDialogButtonClick (wire type 0xA0069).
//----------------------------------------------------------------------------
void __fastcall CNetCommandDialogButtonClick_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandDialogButtonClick cmd(ctx, sender);
    CNetCommandDialogButtonClick_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542A80 - CNetCommandDialogClick (wire type 0xA006A).
//----------------------------------------------------------------------------
void __fastcall CNetCommandDialogClick_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandDialogClick cmd(ctx, sender);
    CNetCommandDialogClick_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F543100 - CNetCommandAllyPing (wire type 0xA0068).
//----------------------------------------------------------------------------
void __fastcall CNetCommandAllyPing_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandAllyPing cmd(ctx, sender);
    CNetCommandAllyPing_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}
