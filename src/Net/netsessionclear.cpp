//============================================================================
//  0x6F54C380 - CNetData::CNetSession::Reset, the body that puts a record
//  back the way it was created.
//
//  Three Storm allocations are handed back, and each one's tag is the
//  shipped binary's own RTTI descriptor name for the element type - which is
//  where three class names in this target came from:
//
//      ".PAUCPlayer@CNetData@@"      struct CNetData::CPlayer *
//      ".?AUSUSPENDPLAYER@Net@@"     struct Net::SUSPENDPLAYER
//      ".?AUGAMERESULT@Net@@"        struct Net::GAMERESULT
//
//  Each is spelled as a literal here rather than given a funcmap.DATA name:
//  an RTTI descriptor is shared with every other instantiation that passes
//  it, and naming a shared string constant can only break matches elsewhere
//  (docs/msvc-vc8-idioms.md).
//
//  The two `memset`s at the top clear blocks nothing in this closure reads;
//  their sizes are the shipped ones and that is all that is claimed about
//  them.  MSVC batches both calls' argument cleanup into one `add esp, 18h`,
//  which is what the shipped code does too.
//
//  Own translation unit: SetGameState (0x6F53E0B0) and the slot table's own
//  ReleaseAll (0x6F54B1A0) are both real calls here.
//============================================================================
#include "netdata.h"
#include "storm.h"

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

void CNetPlayerRecord::Reset()
{
    memset(m_block134, 0, sizeof(m_block134));
    memset(m_block1EC, 0, sizeof(m_block1EC));

    m_priorGameState = m_gameState;
    SetGameState(kGameStateNone);

    CNetData::SLOTTABLE* slots = (CNetData::SLOTTABLE*)SlotTable();

    m_zero27C = 0;
    m_zero280 = 0;
    m_zero284 = 0;

    slots->ReleaseAll(0);

    if (slots->m_players != 0)
        SMemFree(slots->m_players, ".PAUCPlayer@CNetData@@", -2, 0);

    slots->m_playerAlloc = 0;
    slots->m_playerCount = 0;
    slots->m_players = 0;
    slots->m_field48 = 0;

    //  One `mov al, 0FFh` serves both bytes in the shipped stream.
    m_localSlot = 0xFF;
    m_pausingSlot = 0xFF;
    m_resumeBlocked = 0;
    m_zero2DC = 0;

    if (m_pSuspended != 0)
        SMemFree(m_pSuspended, ".?AUSUSPENDPLAYER@Net@@", -2, 0);

    m_suspendAlloc = 0;
    m_suspendCount = 0;
    m_pSuspended = 0;
    m_suspendChunk = 0;
    m_zero2F0 = 0;

    if (m_pResults != 0)
        SMemFree(m_pResults, ".?AUGAMERESULT@Net@@", -2, 0);

    m_resultAlloc = 0;
    m_resultCount = 0;
    m_pResults = 0;
}
