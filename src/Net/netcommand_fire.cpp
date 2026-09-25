//============================================================================
//  CNetData::FireCommand / FireToObserverLists / ParseError - the shared
//  tail every CNetCommandXxx builder in this module calls into once its
//  stack object is fully stamped.  See netcommand.h for the field layout
//  these operate on and PauseGame_LAN_path_analysis.md frames 6/5 for the
//  call-tree context (observer-fire guard, fire to two observer lists).
//============================================================================
#include "netcommand.h"

//  0x6F20BC60 - a debug-log hook that never actually logs anything in this
//  build (nullsub.cpp/nullsub1.cpp); declared locally the same way every
//  other caller in this repo does, there being no shared header for it.
//  Plain C++ linkage (not extern "C") to match nullsub1.cpp's own
//  definition - an extern "C" re-declaration here mangles to a different
//  symbol and the call site cannot be resolved against it.
void __cdecl nullsub_1(const char*, ...);

//----------------------------------------------------------------------------
//  0x6F5378A0 - CNetData::ParseError
//----------------------------------------------------------------------------
void CNetData_ParseError(const char* tag)
{
    nullsub_1("CNetObserver::ParseError (%s)\n", tag);
}

//----------------------------------------------------------------------------
//  0x6F53B790 - CNetData::FireCommand
//----------------------------------------------------------------------------
void __fastcall CNetData_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                      const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

//----------------------------------------------------------------------------
//  0x6F53C210 - a second, byte-identical copy of FireCommand above, called
//  only by CNetCommandPlayerChatMatch_BuildAndFire
//  (netcommand_action_builders_multifield.cpp).  Not a wrapper or an alias:
//  this image was linked without /OPT:ICF (docs/msvc-vc8-idioms.md,
//  "agentdefaults.cpp"), so identical bodies at separate addresses are
//  ordinary here, and CNetData::DispatchActionByte's own dispatch table
//  really does call this address rather than 0x6F53B790 for this one case.
//  A sibling family of these duplicates (0x6F53BA90/0x6F53BAD0, now real
//  below, and the eight 0x6F53C4xx/0x6F53C5xx addresses) is called by the
//  Sync*/UnitSelectionModify/UnitDefineControlGroup builders in
//  netcommand_dispatch.cpp; the eight Sync* addresses are still thunked
//  (their own builders still are - see that file).
//----------------------------------------------------------------------------
void __fastcall CNetCommandPlayerChatMatch_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                         const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

//----------------------------------------------------------------------------
//  0x6F53BA90 / 0x6F53BAD0 - two more byte-identical copies of FireCommand,
//  called only by CNetCommandUnitSelectionModify_BuildAndFire and
//  CNetCommandUnitDefineControlGroup_BuildAndFire respectively
//  (netcommand_dispatch.cpp, now real reconstructions).  Same
//  /OPT:ICF-less duplication as CNetCommandPlayerChatMatch_FireCommand
//  above; the remaining eight 0x6F53C4xx/0x6F53C5xx duplicates the header
//  comment there mentions still have no real caller (their own
//  Sync*_BuildAndFire builders are still thunked).
//----------------------------------------------------------------------------
void __fastcall CNetCommandUnitSelectionModify_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                             const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

void __fastcall CNetCommandUnitDefineControlGroup_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                                const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

//----------------------------------------------------------------------------
//  0x6F53C410/450/490/510/550/590/5D0 - seven more byte-identical copies,
//  one per CNetCommandSyncStoreInteger/Real/Boolean and
//  CNetCommandSyncClearInteger/Real/Boolean/Unit's own BuildAndFire
//  (netcommand_dispatch.cpp, now real reconstructions).  Same
//  /OPT:ICF-less duplication as every other copy in this file; the eighth
//  address in this same family (0x6F53C4D0, CNetCommandSyncStoreUnit's
//  own) has no caller here yet - that builder is still a naked thunk.
//----------------------------------------------------------------------------
void __fastcall CNetCommandSyncStoreInteger_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                          const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

void __fastcall CNetCommandSyncStoreReal_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                       const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

void __fastcall CNetCommandSyncStoreBoolean_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                          const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

void __fastcall CNetCommandSyncClearInteger_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                          const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

void __fastcall CNetCommandSyncClearReal_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                       const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

void __fastcall CNetCommandSyncClearBoolean_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                          const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

void __fastcall CNetCommandSyncClearUnit_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                       const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}
