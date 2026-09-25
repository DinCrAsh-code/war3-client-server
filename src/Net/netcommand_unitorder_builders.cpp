//============================================================================
//  BuildAndFire wrappers for the unit-order CNetCommandXxx family
//  (netcommand_unitorder.h).  Each is the dispatcher's own case body,
//  identical in shape to CNetCommandPause_BuildAndFire (netcommand_pause.cpp)
//  - construct the stack object, call its own (chained) Attach hook, then
//  fire unless suppressed or the reader has already overrun.
//============================================================================
#include "netcommand_unitorder.h"
#include "cdatastore.h"

void* __fastcall CNetCommandUnitOrderBasic_Attach(CDataStore* reader, CNetCommandUnitOrderBasic* cmd);
void* __fastcall CNetCommandUnitOrderTargetPoint_Attach(CDataStore* reader, CNetCommandUnitOrderTargetPoint* cmd);
void* __fastcall CNetCommandUnitOrderTargetImage_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImage* cmd);
void* __fastcall CNetCommandUnitOrderTargetImage2_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImage2* cmd);
void* __fastcall CNetCommandUnitOrderTargetImageFogged_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImageFogged* cmd);
void* __fastcall CNetCommandUnitOrderTargetImageFogged2_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImageFogged2* cmd);

//----------------------------------------------------------------------------
//  0x6F540790 - CNetCommandUnitOrderBasic (wire type 0xA0010).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitOrderBasic_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitOrderBasic cmd(ctx, sender);
    CNetCommandUnitOrderBasic_Attach(reader, &cmd);

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
//  0x6F540870 - CNetCommandUnitOrderTargetPoint (wire type 0xA0011).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitOrderTargetPoint_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitOrderTargetPoint cmd(ctx, sender);
    CNetCommandUnitOrderTargetPoint_Attach(reader, &cmd);

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
//  0x6F540960 - CNetCommandUnitOrderTargetImage (wire type 0xA0012).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitOrderTargetImage_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitOrderTargetImage cmd(ctx, sender);
    CNetCommandUnitOrderTargetImage_Attach(reader, &cmd);

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
//  0x6F540A50 - CNetCommandUnitOrderTargetImage2 (wire type 0xA0013).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitOrderTargetImage2_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitOrderTargetImage2 cmd(ctx, sender);
    CNetCommandUnitOrderTargetImage2_Attach(reader, &cmd);

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
//  0x6F540B50 - CNetCommandUnitOrderTargetImageFogged (wire type 0xA0014).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitOrderTargetImageFogged_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitOrderTargetImageFogged cmd(ctx, sender);
    CNetCommandUnitOrderTargetImageFogged_Attach(reader, &cmd);

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
//  0x6F540C50 - CNetCommandUnitOrderTargetImageFogged2 (wire type 0xA0015).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitOrderTargetImageFogged2_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitOrderTargetImageFogged2 cmd(ctx, sender);
    CNetCommandUnitOrderTargetImageFogged2_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}
