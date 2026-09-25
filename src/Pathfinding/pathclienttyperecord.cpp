//============================================================================
//  0x6F493E20 - stage a movement type record onto the client, or clear it.
//
//  Both fields are set from the record together and both go to -1 when there
//  is no record - not to 0, because -1 is the "no registration" sentinel the
//  rest of this module tests against (see CPathClient::ScheduleRetry's own
//  note about reading the stamp as unsigned so that -1 reads as huge).
//============================================================================
#include "pathmove.h"

//  The record's two staged fields.  Only these two are named; nothing in this
//  call tree reads anything else in it.
struct SPathTypeRecord
{
    char m_reserved00[0x14];
    int  m_typeMask;        // +0x14
    int  m_ignore;          // +0x18
};

void CPathClient::SetTypeRecord(const void* record)
{
    const SPathTypeRecord* r = (const SPathTypeRecord*)record;

    if (r != 0)
    {
        m_typeMask = r->m_typeMask;
        m_ignore = (CGridRegistration*)r->m_ignore;
        return;
    }

    m_typeMask = -1;
    m_ignore = (CGridRegistration*)-1;
}
