//============================================================================
//  0x6F4A6A40 - RebuildSyncRegistration.  Read one SSyncRegistration record
//  back (the counterpart of WriteSyncRegistrationRecord) and, if it was
//  actually present, re-request it on `channel` for `owner` rather than
//  reconstructing it by value.
//
//  Own translation unit, deliberately: CAgentRelation::Load
//  (agentrelationsyncload.cpp) calls this out of line, twice.
//============================================================================
#include "agentrelation.h"
#include "cdatastore.h"

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

SSyncRegistration* __fastcall RebuildSyncRegistration(CDataStore* store, CSyncChannel* channel, void* owner)
{
    unsigned int hasData;
    store->ReadDword(&hasData);
    if (hasData != 0)
    {
        CFloat deadline(kCFloatNoInit);
        CFloat value(kCFloatNoInit);
        unsigned int sequence;
        int time;
        unsigned int sampleBit;

        CDataStore* chain = ReadCFloat(store, &deadline);
        chain = ReadCFloat(chain, &value);
        chain = chain->ReadDword(&sequence);
        chain = chain->ReadDwordAlt3((unsigned int*)&time);
        chain->ReadDword(&sampleBit);

        SSyncRegistration* reg = channel->MakeRequest(owner, &time, value, deadline, (int)sequence);
        if (sampleBit != 0)
            reg->m_flags |= 1;
        return reg;
    }
    return 0;
}
