//============================================================================
//  CNetCommandPause - the dispatcher's action-byte case 1
//  (PauseGame_LAN_path_analysis.md frame [7]).  See netcommand.h for the
//  base layout and the constructor-inlining/SEH-frame note.
//============================================================================
#include "netcommand.h"
#include "cdatastore.h"

class CNetCommandPause : public CNetCommandBase
{
public:
    CNetCommandPause(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0001, 1)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};

//  0x6F5546E0 - defined in its own TU (netcommand_pause_attach.cpp) so it
//  is called for real rather than inlined away - see that file.
void* __fastcall CNetCommandPause_Attach(CDataStore* reader, CNetCommandBase* cmd);

//----------------------------------------------------------------------------
//  0x6F5402B0 - build a CNetCommandPause (wire type 0xA0001) and fire it,
//  unless `suppressFire` is set (return immediately) or the reader's
//  position has already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------
void __fastcall CNetCommandPause_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandPause cmd(ctx, sender);
    CNetCommandPause_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}
