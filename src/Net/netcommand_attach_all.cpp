//============================================================================
//  Trivial per-class "attach" hooks (see netcommand.h) for every base-shape
//  CNetCommandXxx builder in netcommand_action_builders.cpp.  Kept in their
//  own TU, away from their callers, for the same reason
//  netcommand_pause_attach.cpp is: called for real, not inlined away.
//============================================================================
#include "netcommand.h"
#include "cdatastore.h"
#include "netcommand_classes_1field.h"

//  These four read one (or, for TrainCancel, two) extra payload field(s)
//  from the reader into the command object's own tail, unlike every other
//  Attach hook in this file (a pure identity no-op) - each takes the
//  concrete command pointer itself (not a pre-computed field address) and
//  offsets into it internally, matching the dump's own `add edx, 0x18`
//  ahead of the call.

//  0x6F554770 - reads one byte into cmd->m_speedLevel (+0x18).
void* __fastcall CNetCommandGameSpeed_Attach(CDataStore* reader, CNetCommandGameSpeed* cmd)
{
    reader->ReadByte(&cmd->m_speedLevel);
    return reader;
}

//  0x6F554790 - reads one dword into cmd->m_saveSlot (+0x18) via
//  CDataStore's third four-byte reader (ReadDwordAlt2, 0x6F4C2CA0 -
//  already reconstructed, cdatastore.h).
void* __fastcall CNetCommandSaveDone_Attach(CDataStore* reader, CNetCommandSaveDone* cmd)
{
    reader->ReadDwordAlt2(&cmd->m_saveSlot);
    return reader;
}

//  0x6F554410 - reads a byte into cmd->m_field18 (+0x18), then a dword
//  into cmd->m_field1C (+0x1C).
void* __fastcall CNetCommandTrainCancel_Attach(CDataStore* reader, CNetCommandTrainCancel* cmd)
{
    reader->ReadByte(&cmd->m_field18);
    reader->ReadDword(&cmd->m_field1C);
    return reader;
}

//  0x6F554FD0 - reads one byte into cmd->m_key (+0x18).
void* __fastcall CNetCommandArrowKey_Attach(CDataStore* reader, CNetCommandArrowKey* cmd)
{
    reader->ReadByte(&cmd->m_key);
    return reader;
}

//  0x6F554700
void* __fastcall CNetCommandUnpause_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554720
void* __fastcall CNetCommandGameSpeedIncrement_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554740
void* __fastcall CNetCommandGameSpeedDecrement_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F5542E0
void* __fastcall CNetCommandUnitRefreshSubGroup_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F5549C0
void* __fastcall CNetCommandCheatDefeat_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554AC0
void* __fastcall CNetCommandCheatCooldown_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F5549E0
void* __fastcall CNetCommandCheatFastBuild_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554A00
void* __fastcall CNetCommandCheatFastDeath_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554A20
void* __fastcall CNetCommandCheatFood_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554A40
void* __fastcall CNetCommandCheatGod_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554A60
void* __fastcall CNetCommandCheatMana_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554A80
void* __fastcall CNetCommandCheatNoDefeat_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554AA0
void* __fastcall CNetCommandCheatNoVictory_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554AE0
void* __fastcall CNetCommandCheatResearch_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554B20
void* __fastcall CNetCommandCheatShowMap_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554B40
void* __fastcall CNetCommandCheatTechTree_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554B60
void* __fastcall CNetCommandCheatUpgrade_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554B80
void* __fastcall CNetCommandCheatVictory_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554CE0
void* __fastcall CNetCommandPlayerEndCinematic_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554F20
void* __fastcall CNetCommandShowSkill_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554F40
void* __fastcall CNetCommandShowSubmenu_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F554590
void* __fastcall CNetCommandReplayViewSpeedIncrement_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}

//  0x6F5545B0
void* __fastcall CNetCommandReplayViewSpeedDecrement_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}
