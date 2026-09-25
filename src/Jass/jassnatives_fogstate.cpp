//============================================================================
//  0x6F3C1A30 / 0x6F3C1AB0 / 0x6F3C1B20 - JASS_SetFogStateRect,
//  JASS_SetFogStateRadius, JASS_SetFogStateRadiusLoc.
//
//  All three turn (player, useSharedVision) into a 16-bit fog mask word
//  through FogStatePlayerMaskOrRecordMask (0x6F408110,
//  src/Jass/jassfogstateplayermask.cpp - desync_mechanism_worklist.json
//  batch B's own name/reconstruction; this batch independently
//  reconstructed the same address as SPlayerTableSubmitParts::
//  GetPlayerFogWord and deferred to batch B's once the merge surfaced the
//  collision) and then write that word through the player table's own
//  spatial grid - playertablefog.h/fogstatetablewriter.cpp, the same +0x34
//  sub-object of g_unk6FAB65F4 the widget-submission code
//  (playertablesubmitprepare.cpp) already reads: one grid, two features.
//
//  `whichFogState` is forwarded exactly as the JASS bytecode hands it to the
//  native - nothing here calls JASS_ConvertFogState (0x6F3B2830, already
//  reconstructed elsewhere), so whatever numeric encoding the caller already
//  applied is the one the grid writers' own three-way switch reads.
//
//  Every one of these lands on the same three-word-store pattern before its
//  own writer call: a zeroed `CFloat` (`g_CFloatZero`, `dword_6FAAE470`) is
//  stored into the third slot of the point/bounds block passed by address
//  even though nothing downstream of ApplyFogRadius that this session
//  reconstructed reads it - both float-taking natives build a 3-int point
//  rather than a 2-int one, so the shipped store is kept rather than
//  dropped as dead code.
//============================================================================
#include "jassnatives.h"
#include "playertablefog.h"
#include "CFloat.h"

extern void* g_unk6FAB65F4;   // dword_6FAB65F4 - funcmap.DATA global

//  0x6F408110 - src/Jass/jassfogstateplayermask.cpp.  `ecx`/`edx` are both
//  genuinely dead in the callee; every caller here still passes the
//  player table itself in the first (dead) argument, matching what the
//  shipped code's own callers do.
unsigned short __fastcall FogStatePlayerMaskOrRecordMask(
    void* unused_widgetRecord, int unused, int slot, int useRecordMask);

//----------------------------------------------------------------------------
//  0x6F3C1A30 - JASS_SetFogStateRect "(Hplayer;Hfogstate;Hrect;Bj;)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetFogStateRect(int hPlayer, int whichFogState, int hRect,
                                  int useSharedVision)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);
    if (!player)
        return;

    SPlayerTableSubmitParts* table =
        ((SPlayerTableFogHost*)g_unk6FAB65F4)->m_pParts;

    SJassRect* rect = ResolveRectHandle(hRect);
    if (!rect)
        return;

    unsigned short fogWord = FogStatePlayerMaskOrRecordMask(
        table, 0, player->m_playerIndex, useSharedVision);

    SJassBounds bounds(rect->m_minX, rect->m_minY, rect->m_maxX, rect->m_maxY);
    table->ApplyFogRect(whichFogState, fogWord, &bounds);
}

//----------------------------------------------------------------------------
//  0x6F3C1AB0 - JASS_SetFogStateRadius "(Hplayer;Hfogstate;RRR;Bj;)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetFogStateRadius(int hPlayer, int whichFogState,
                                    const CFloat* x, const CFloat* y,
                                    const CFloat* radius, int useSharedVision)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);
    if (!player)
        return;

    SPlayerTableSubmitParts* table =
        ((SPlayerTableFogHost*)g_unk6FAB65F4)->m_pParts;

    unsigned short fogWord = FogStatePlayerMaskOrRecordMask(
        table, 0, player->m_playerIndex, useSharedVision);

    int point[3] = { *(const int*)x, *(const int*)y, g_CFloatZero.m_bits };
    table->ApplyFogRadius(whichFogState, fogWord, point, radius);
}

//----------------------------------------------------------------------------
//  0x6F3C1B20 - JASS_SetFogStateRadiusLoc "(Hplayer;Hfogstate;Hlocation;R;Bj;)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetFogStateRadiusLoc(int hPlayer, int whichFogState,
                                       int hLocation, const CFloat* radius,
                                       int useSharedVision)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);
    if (!player)
        return;

    SPlayerTableSubmitParts* table =
        ((SPlayerTableFogHost*)g_unk6FAB65F4)->m_pParts;

    unsigned short fogWord = FogStatePlayerMaskOrRecordMask(
        table, 0, player->m_playerIndex, useSharedVision);

    SJassLocation* loc = ResolveLocationHandle(hLocation);
    if (!loc)
        return;

    int point[3] = { loc->m_x, loc->m_y, g_CFloatZero.m_bits };
    table->ApplyFogRadius(whichFogState, fogWord, point, radius);
}
