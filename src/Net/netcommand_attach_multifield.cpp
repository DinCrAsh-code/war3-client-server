//============================================================================
//  Attach hooks for the independent multi-field CNetCommandXxx classes
//  (netcommand_classes_multifield.h).  Each is called for real from its own
//  BuildAndFire in netcommand_action_builders_multifield.cpp - a different
//  TU, so none of these risk being inlined away (the /Ob2 gotcha
//  netcommand_pause_attach.cpp documents), and since none of them call each
//  other either, they can all share this one file safely.
//============================================================================
#include "netcommand_classes_multifield.h"
#include "cdatastore.h"

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  0x6F5543D0
void* __fastcall CNetCommandReviveCancel_Attach(CDataStore* reader, CNetCommandReviveCancel* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    return reader;
}

//  0x6F554320
void* __fastcall CNetCommandUnitSelectionEvent_Attach(CDataStore* reader, CNetCommandUnitSelectionEvent* cmd)
{
    reader->ReadByte(&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    reader->ReadDword((unsigned int*)&cmd->m_field20);
    return reader;
}

//  0x6F554380
void* __fastcall CNetCommandSelectableSelectionModify_Attach(CDataStore* reader, CNetCommandSelectableSelectionModify* cmd)
{
    reader->ReadByte(&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    reader->ReadDword((unsigned int*)&cmd->m_field20);
    return reader;
}

//  0x6F554250 - reads +0x19 before +0x18, the dump's own order.
void* __fastcall CNetCommandUnitSelectControlGroup_Attach(CDataStore* reader, CNetCommandUnitSelectControlGroup* cmd)
{
    reader->ReadByte(&cmd->m_field19);
    reader->ReadByte(&cmd->m_field18);
    return reader;
}

//  0x6F5542A0
void* __fastcall CNetCommandUnitSelectSubGroup_Attach(CDataStore* reader, CNetCommandUnitSelectSubGroup* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    reader->ReadDword((unsigned int*)&cmd->m_field20);
    return reader;
}

//  0x6F554C70
void* __fastcall CNetCommandCheatCritter_Attach(CDataStore* reader, CNetCommandCheatCritter* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    return reader;
}

//  0x6F554C30
void* __fastcall CNetCommandCheatLumber_Attach(CDataStore* reader, CNetCommandCheatLumber* cmd)
{
    reader->ReadByte(&cmd->m_targetPlayer);
    reader->ReadDword((unsigned int*)&cmd->m_amount);
    return reader;
}

//  0x6F554CB0
void* __fastcall CNetCommandCheatResources_Attach(CDataStore* reader, CNetCommandCheatResources* cmd)
{
    reader->ReadByte(&cmd->m_targetPlayer);
    reader->ReadDword((unsigned int*)&cmd->m_amount);
    return reader;
}

//  0x6F554B00
void* __fastcall CNetCommandCheatSetTOD_Attach(CDataStore* reader, CNetCommandCheatSetTOD* cmd)
{
    ReadCFloat(reader, &cmd->m_timeOfDay);
    return reader;
}

//  0x6F554910
void* __fastcall CNetCommandTeamChangeAlliance_Attach(CDataStore* reader, CNetCommandTeamChangeAlliance* cmd)
{
    reader->ReadByte(&cmd->m_targetPlayer);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    return reader;
}

//  0x6F554960
void* __fastcall CNetCommandTeamTradeResources_Attach(CDataStore* reader, CNetCommandTeamTradeResources* cmd)
{
    reader->ReadByte(&cmd->m_targetPlayer);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    reader->ReadDword((unsigned int*)&cmd->m_field20);
    return reader;
}

//  0x6F555010 - reads the two trailing dwords before the string, then
//  guards the same way ReadString's every other caller in this codebase
//  does: if the reader ran past its declared length filling the buffer,
//  force-terminate it at byte 0 rather than trust whatever partial bytes
//  landed there.
void* __fastcall CNetCommandPlayerChatMatch_Attach(CDataStore* reader, CNetCommandPlayerChatMatch* cmd)
{
    //  Written through one base pointer stepped by +4 rather than
    //  &cmd->m_field118 / &cmd->m_field11C separately - the shipped code
    //  computes the first field's address once (`lea ebx,[edi+118h]`) and
    //  reuses it (`add ebx,4`) for the second rather than re-deriving it,
    //  which only happens when the source spells it that way too.
    unsigned int* field = (unsigned int*)&cmd->m_field118;
    CDataStore* chained = reader->ReadDword(field);
    chained->ReadDword(field + 1);
    reader->ReadString(cmd->m_matchText, 0x100);
    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
        cmd->m_matchText[0] = 0;
    return reader;
}

//  0x6F554D20 - the third field (+0x20) has no constructor default; only
//  this hook ever writes it.
void* __fastcall CNetCommandResumeTriggerExec_Attach(CDataStore* reader, CNetCommandResumeTriggerExec* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    reader->ReadDword((unsigned int*)&cmd->m_field20);
    return reader;
}

//  0x6F554D70
void* __fastcall CNetCommandTriggerSyncReady_Attach(CDataStore* reader, CNetCommandTriggerSyncReady* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    return reader;
}

//  0x6F554DB0
void* __fastcall CNetCommandTrackableHit_Attach(CDataStore* reader, CNetCommandTrackableHit* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    return reader;
}

//  0x6F554DF0
void* __fastcall CNetCommandTrackableTrack_Attach(CDataStore* reader, CNetCommandTrackableTrack* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    return reader;
}

//  0x6F554E50
void* __fastcall CNetCommandDialogButtonClick_Attach(CDataStore* reader, CNetCommandDialogButtonClick* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    reader->ReadDword((unsigned int*)&cmd->m_field20);
    reader->ReadDword((unsigned int*)&cmd->m_field24);
    return reader;
}

//  0x6F554ED0
void* __fastcall CNetCommandDialogClick_Attach(CDataStore* reader, CNetCommandDialogClick* cmd)
{
    reader->ReadDword((unsigned int*)&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    reader->ReadDword((unsigned int*)&cmd->m_field20);
    reader->ReadDword((unsigned int*)&cmd->m_field24);
    return reader;
}

//  0x6F554F80
void* __fastcall CNetCommandAllyPing_Attach(CDataStore* reader, CNetCommandAllyPing* cmd)
{
    ReadCFloat(reader, &cmd->m_x);
    ReadCFloat(reader, &cmd->m_y);
    ReadCFloat(reader, &cmd->m_field20);
    return reader;
}
