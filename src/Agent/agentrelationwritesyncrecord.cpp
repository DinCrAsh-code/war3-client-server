//============================================================================
//  0x6F4A6390 - WriteSyncRegistrationRecord.  Write one SSyncRegistration
//  (game.h): a presence flag, then - only if present - m_deadline,
//  m_value, m_sequence, m_time and the "sample me" bit of m_flags, in that
//  field order.  A null registration writes just the presence flag as 0.
//
//  Own translation unit, deliberately: CAgentRelation::Save
//  (agentrelationsyncsave.cpp) calls this out of line, twice.
//============================================================================
#include "agentrelation.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);

void __fastcall WriteSyncRegistrationRecord(CDataStoreScratch* store, const SSyncRegistration* reg)
{
    if (reg != 0)
    {
        unsigned int sampleBit = reg->m_flags & 1;
        unsigned int sequence = (unsigned int)reg->m_sequence;
        const CFloat* valuePtr = &reg->m_value;
        CDataStoreScratch* chain = store->WriteDword(1);
        chain = WriteCFloat(chain, &reg->m_deadline);
        chain = WriteCFloat(chain, valuePtr);
        chain = chain->WriteDword(sequence);
        chain = chain->WriteDwordAlt2((unsigned int)reg->m_time);
        chain->WriteDword(sampleBit);
        return;
    }
    store->WriteDword(0);
}
