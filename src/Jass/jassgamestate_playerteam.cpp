//============================================================================
//  CJassGameState::GetPlayerTeam (0x6F3A1660) and ::SetPlayerTeam
//  (0x6F3A82B0) - the two members jassnatives_player.cpp's JASS_GetPlayerTeam
//  / JASS_SetPlayerTeam natives call.
//
//  Both index the same per-player record array
//  (IndexedArrayHolder_6F3A1650::At, already reconstructed in
//  Misc/misc_field_getters.cpp and re-declared here the way every other user
//  of it does) and read/write a team field at +0x278 of that record.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  sub_6F3A1650 - see Misc/misc_field_getters.cpp's own comment.  Redeclared
//  locally (never redefined) so this TU's call mangles to the same symbol.
//----------------------------------------------------------------------------
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  One player's record, named only for the one field this pair touches.
struct SPlayerTeamRecord
{
    char m_reserved000[0x278];
    int  m_team;   // +0x278
};

//----------------------------------------------------------------------------
//  0x6F3A81D0 / 0x6F3A8150 - remove/add a player record from/to a team's own
//  roster list.  `this` is the roster entry (m_teamRosters + team*0x10), the
//  one stack argument is the player record; both `retn 4`.  Neither has a
//  call-tree dump of its own (they are TSGrowableArray-of-refcounted-object
//  membership operations, complete with an AddRef/Release pair through the
//  held object's own vtable slot 7) and reach well outside the alliance/
//  team-state closure this batch is scoped to, so both stay naked redirects
//  to the real, unhooked bodies.
//----------------------------------------------------------------------------
struct CJassTeamRoster
{
    void Remove(void* playerRecord);
    void Add(void* playerRecord);
};

__declspec(naked) void CJassTeamRoster::Remove(void*)
{
    __asm { mov eax, 06F3A81D0h }
    __asm { jmp eax }
}

__declspec(naked) void CJassTeamRoster::Add(void*)
{
    __asm { mov eax, 06F3A8150h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F3A1660 - `retn 4`.  `this` (dword_6FAB65F4) is passed straight
//  through to the array holder unmodified.
//----------------------------------------------------------------------------
int CJassGameState::GetPlayerTeam(int playerIndex)
{
    IndexedArrayHolder_6F3A1650* holder = (IndexedArrayHolder_6F3A1650*)this;
    SPlayerTeamRecord* record = (SPlayerTeamRecord*)holder->At(playerIndex);

    return record->m_team;
}

//----------------------------------------------------------------------------
//  0x6F3A82B0 - `retn 8`.  The old team (if any, -1 means none) has its
//  roster entry removed first; the record is always linked into the new
//  team's roster and its own field updated last.
//----------------------------------------------------------------------------
void CJassGameState::SetPlayerTeam(int team, int playerIndex)
{
    IndexedArrayHolder_6F3A1650* holder = (IndexedArrayHolder_6F3A1650*)this;
    SPlayerTeamRecord* record = (SPlayerTeamRecord*)holder->At(playerIndex);

    if (record->m_team != -1)
    {
        CJassTeamRoster* oldRoster =
            (CJassTeamRoster*)((char*)m_teamRosters + record->m_team * 0x10);
        oldRoster->Remove(record);
    }

    CJassTeamRoster* newRoster =
        (CJassTeamRoster*)((char*)m_teamRosters + team * 0x10);
    newRoster->Add(record);

    record->m_team = team;
}
