//============================================================================
//  CNetPlayerRecord::CNetPlayerRecord() / ::~CNetPlayerRecord() - 0x6F54FAB0
//  / 0x6F54FB90.  cnd-ctor-agent-2 session, chasing CNetData::CNetData's own
//  prerequisite member subobjects (docs/notes/checksum-provider-registry.md,
//  "cnd-ctor-agent session").
//
//  The constructor placement-constructs a real `CNetObserver`
//  (netobserver.h) directly over this object's own +0x00..+0x134 span -
//  see that header for why CNetPlayerRecord's own declared fields in that
//  range (`m_reserved00`/`m_pauseObserverList`/`m_uiObserverList`/
//  `m_reserved1C`) are left untouched rather than retyped.
//============================================================================
#include "netdata.h"
#include "netobserver.h"
#include "storm.h"

CNetPlayerRecord::CNetPlayerRecord()
{
    new (this) CNetObserver();

    m_block134[0]  = 0;
    *(unsigned char*)((char*)this + 0x154) = 0;
    *(unsigned char*)((char*)this + 0x164) = 0;
    *(unsigned short*)m_block1EC = 0;

    m_priorGameState = 0;
    m_field278       = 1;

    //  0x6F54F430 - CNetData::SLOTTABLE::SLOTTABLE(), real reconstruction
    //  (netdataslottable_ctor.cpp).  m_slotTable is CNetPlayerRecord's own
    //  +0x288 raw-byte view of CNetData::SLOTTABLE (netdata.h) - cast
    //  rather than retyped, so the declared `char[0x4C]` field already
    //  read elsewhere by name is untouched.
    CNetData::SLOTTABLE* slots = (CNetData::SLOTTABLE*)m_slotTable;
    new (slots) CNetData::SLOTTABLE();

    //  m_playerAlloc..m_field48 - not set by SLOTTABLE's own constructor
    //  (see that constructor's own note); zeroed here instead, matching
    //  the shipped instruction stream's own split between the two
    //  functions.
    slots->m_playerAlloc  = 0;
    slots->m_playerCount  = 0;
    slots->m_players      = 0;
    slots->m_playerChunk  = 0;
    slots->m_refereeAlloc = 0;
    slots->m_refereeCount = 0;
    slots->m_referees     = 0;
    slots->m_refereeChunk = 0;
    slots->m_field48      = 0;

    m_suspendAlloc = 0;
    m_suspendCount = 0;
    m_pSuspended   = 0;
    m_resultAlloc  = 0;
    m_resultCount  = 0;
    m_pResults     = 0;
    m_field300     = 0;

    //  0x6F54C380 - Reset(), already a real reconstruction
    //  (netsessionclear.cpp).
    Reset();
}

CNetPlayerRecord::~CNetPlayerRecord()
{
    //  0x6F54C380 - Reset() again (the shipped destructor really does call
    //  it a second time, not just the constructor).
    Reset();

    //  ".?AUGAMERESULT@Net@@" / ".PAUSUSPENDPLAYER@Net@@" - the two tags
    //  netdata.h's own file header already documents for these arrays.
    if (m_pResults)
        SMemFree(m_pResults, ".?AUGAMERESULT@Net@@", -2, 0);

    if (m_pSuspended)
        SMemFree(m_pSuspended, ".?AUSUSPENDPLAYER@Net@@", -2, 0);

    //  0x6F54FA00 - CNetData::SLOTTABLE::~SLOTTABLE(), real reconstruction
    //  (netdataslottable_ctor.cpp).
    ((CNetData::SLOTTABLE*)m_slotTable)->~SLOTTABLE();

    //  0x6F545040 - CNetObserver::~CNetObserver() over this object's own
    //  +0x00..+0x134 span (naked thunk - see netobserver.h's file header).
    ((CNetObserver*)this)->~CNetObserver();
}
