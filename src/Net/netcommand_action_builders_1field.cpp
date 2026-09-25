//============================================================================
//  CNetCommandXxx builders carrying exactly one extra payload field beyond
//  CNetCommandBase's own header, read from the wire by their own Attach
//  hook (netcommand_attach_all.cpp) instead of that hook being a no-op -
//  GameSpeed's requested level, SaveDone's save-slot dword, TrainCancel's
//  byte+dword pair, ArrowKey's key byte.  Natural class-size alignment (the
//  base class rounds up to 0x18 for having a vtable) is what lands each
//  extra field at +0x18 without explicit padding.  Classes declared in
//  netcommand_classes_1field.h, shared with the Attach hooks that offset
//  into them.
//============================================================================
#include "netcommand.h"
#include "cdatastore.h"
#include "netcommand_classes_1field.h"

void* __fastcall CNetCommandGameSpeed_Attach(CDataStore* reader, CNetCommandGameSpeed* cmd);
void* __fastcall CNetCommandSaveDone_Attach(CDataStore* reader, CNetCommandSaveDone* cmd);
void* __fastcall CNetCommandTrainCancel_Attach(CDataStore* reader, CNetCommandTrainCancel* cmd);
void* __fastcall CNetCommandArrowKey_Attach(CDataStore* reader, CNetCommandArrowKey* cmd);

//----------------------------------------------------------------------------
//  0x6F540450 - CNetCommandGameSpeed (wire type 0xA0003).  The dispatcher's
//  base GameSpeed case (siblings CNetCommandGameSpeedIncrement/Decrement,
//  netcommand_action_builders.cpp, carry no payload of their own).
//----------------------------------------------------------------------------
void __fastcall CNetCommandGameSpeed_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandGameSpeed cmd(ctx, sender);
    CNetCommandGameSpeed_Attach(reader, &cmd);

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
//  0x6F5406C0 - CNetCommandSaveDone (wire type 0xA0007).
//----------------------------------------------------------------------------
void __fastcall CNetCommandSaveDone_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSaveDone cmd(ctx, sender);
    CNetCommandSaveDone_Attach(reader, &cmd);

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
//  0x6F541250 - CNetCommandTrainCancel (wire type 0xA001E).
//----------------------------------------------------------------------------
void __fastcall CNetCommandTrainCancel_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandTrainCancel cmd(ctx, sender);
    CNetCommandTrainCancel_Attach(reader, &cmd);

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
//  0x6F5431E0 - CNetCommandArrowKey (wire type 0xA0075).
//----------------------------------------------------------------------------
void __fastcall CNetCommandArrowKey_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandArrowKey cmd(ctx, sender);
    CNetCommandArrowKey_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}
