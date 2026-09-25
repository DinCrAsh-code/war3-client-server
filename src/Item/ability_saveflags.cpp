//============================================================================
//  0x6F02E410 - CAbility's vtable slot 18 (+0x48), SaveFlags.  See
//  ability.h.
//
//  Writes the same ten fields DumpState (ability_dump.cpp) reads, in a
//  different order and through CDataStoreScratch's own three
//  not-folded-by-the-linker four-byte writers (cdatastorescratch.h) - one
//  of them (WriteDwordAlt2, for m_field2C) is a genuinely different
//  function from the other nine calls, matching the shipped stream's own
//  mix of `call sub_6F4C2360` and one `call sub_6F4C2310`.  The embedded
//  m_timer is saved through its own vtable slot 8 (+0x20) - not named
//  anywhere in this build (agenttimer.h only establishes TRefCnt's own two
//  slots), so it is reached by raw offset rather than a guessed call.
//============================================================================
#include "ability.h"
#include "cdatastorescratch.h"
#include "agenttimer.h"

typedef CDataStoreScratch* (__thiscall *AgentTimerVtableSlot8Fn)(
    void*, CDataStoreScratch*);

void CAbility::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);
    store->WriteDword(m_field24);
    store->WriteDword(m_field28);
    store->WriteDwordAlt2(m_field2C);
    store->WriteDword(m_field3C);
    store->WriteDword(m_field40);
    store->WriteDword(m_field44);
    store->WriteDword(m_field48);
    store->WriteDword(m_field4C);
    store->WriteDword(m_typeId);
    store->WriteDword(m_field50);
    ((AgentTimerVtableSlot8Fn)(*(void***)&m_timer)[0x20 / 4])(&m_timer,
                                                               store);
    store->WriteDword(m_field38);
}
