//============================================================================
//  0x6F3A6AA0 - the body ReloadGame and RestartGame share.  `retn 10h`.
//
//  Four things, in this order:
//
//  1. refuse outright if the game data carries a tag;
//  2. when `reResolveSaveGame` is set, look at the path in the world's map
//     slot: if it ends in ".w3z" it is a saved game, so open its header and
//     put the *map* it was saved from back in the slot, and remember whether
//     the save's own slot record still matches the session's.  Anything else
//     - a short path, a different extension, a header that would not read -
//     leaves the slot alone and clears the session's own slot record;
//  3. when `endNow` is set, latch the score-screen flag and a reason of 1 and
//     ask the session to end;
//  4. otherwise decide whether a load is already pending and start the load
//     itself.
//
//  The two `[esi+328h]` stores in arm 2 both write 1 and are both reached,
//  which is why they are two statements rather than one after the branch.
//============================================================================
#include "gamerestart.h"
#include "savegame.h"
#include "os.h"
#include "handleobject.h"

int __stdcall Storm_508(const char* a, const char* b, unsigned int max);

void __stdcall RequestSessionGameOver(int index);   // 0x6F39F240

//  0x6F3409B0 - CSaveGame::MapPath, Game/savegamemappath.cpp.
const char* __fastcall SaveGameMapPath(CSaveGame* save);

//  0x6F5BD1F0 - Game/gamesetupslotrecord.cpp.  A bare `jmp` onto the slot
//  table encoder: it writes the save game's own slot table into the
//  session's record and says whether it fitted.  (It was called
//  "SlotRecordMatches" while it was a redirect, on the reading that a
//  non-zero answer meant the two agreed; the body says it *overwrites*.)
int __fastcall SlotRecordEncode(Net::GAMESETUP_SLOTRECORD* out,
                                Net::GAMESETUP_SLOTS* in);

//  0x6F545A80 (`retn 0`, both arguments in registers) - clear the session's
//  own slot record.  Not in this tree; redirected.
int __fastcall SlotRecordCapture(void* record, int index);

//  0x6F3A4C70 - `retn 0Ch` against a receiver in ecx and three pushes, so a
//  member.  Not in this tree; redirected.

void SWorldRestart::RequestRestart(int doScoreScreen, int reResolveSaveGame,
                                   int endNow, int endReason)
{
    SWorldRestart* self = this;

    if (IsGameDataTagSet_6F53E670())
        return;

    self->m_reloadFailed = 0;

    if (reResolveSaveGame != 0)
    {
        STStringField* slot = (STStringField*)self->m_mapPath;
        const char* path =
            (const char*)GetHandleOrZeroAlias((const SHandleHolder*)slot);
        unsigned int length = Storm_506(path);

        const char* extension = 0;
        if (length > 4)
            extension = path + length - 4;

        if (extension != 0 && Storm_508(extension, ".w3z", 0x7FFFFFFF) == 0)
        {
            CSaveGame save;
            SaveGameConstruct(&save);

            if (!save.ReadHeader(path))
            {
                SaveGameDestruct(&save);
                return;
            }

            slot->Assign(SaveGameMapPath(&save));

            if (SlotRecordEncode((Net::GAMESETUP_SLOTRECORD*)self->m_slotRecord, &save.m_slots))
                self->m_reloadFailed = 1;

            SaveGameDestruct(&save);
        }
        else
        {
            SlotRecordCapture(&self->m_slotRecord, 0);
            self->m_reloadFailed = 1;
        }
    }

    if (endNow != 0)
    {
        self->m_showScoreScreen = doScoreScreen;
        self->m_endReason = 1;
        RequestSessionGameOver(endReason);
        return;
    }

    int pending = 0;
    if (self->m_endReason != 0 && WorldSuppressReload(self) == 0)
        pending = 1;

    self->m_endReason = pending;

    STStringField* slot = (STStringField*)self->m_mapPath;
    self->RequestLoad((const char*)GetHandleOrZeroAlias(
                          (const SHandleHolder*)slot),
                      doScoreScreen, 0);
}
