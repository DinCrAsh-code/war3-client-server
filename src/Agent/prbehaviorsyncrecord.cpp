//============================================================================
//  0x6F4AA3A0 - a second, byte-identical (apart from its own call
//  displacements) copy of Agent/agentrelationwritesyncrecord.cpp's own
//  WriteSyncRegistrationRecord: write one SSyncRegistration (game.h) - a
//  presence flag, then, only if present, m_deadline, m_value, m_sequence,
//  m_time and the "sample me" bit of m_flags, in that order. A null
//  registration writes just the presence flag as 0.
//
//  Not the SAME function under a new name: this is a genuinely separate
//  address in the shipped image, reached only from CPrBehavior::Save
//  (prbehavior_save.cpp) - the same "one C++ definition, several shipped
//  addresses" duplicate-compiled-body idiom the base chain's own
//  scalar-deleting-destructor quadruplet already documents
//  (Agent/presence.h's file header), just for an ordinary free function
//  this time instead of a special member. Kept in its own translation
//  unit for the same reason the original is: CPrBehavior::Save calls it
//  out of line, twice.
//============================================================================
#include "game.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);

void __fastcall WriteSyncRegistrationRecordForPrBehavior(CDataStoreScratch* store, const SSyncRegistration* reg)
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
