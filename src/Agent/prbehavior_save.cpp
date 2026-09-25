//============================================================================
//  0x6F4AA450 - NIpse::CPrBehavior::Save, vtable slot 9. See prbehavior.h's
//  own comment for the full field order and the "Alt2"-duplicate note.
//
//  Own translation unit: WriteSyncRegistrationRecordForPrBehavior
//  (prbehaviorsyncrecord.cpp) is a real call out of line, twice - same
//  reasoning Agent/agentrelationsyncsave.cpp's own header gives for its
//  sibling.
//============================================================================
#include "prbehavior.h"
#include "cdatastorescratch.h"

void __fastcall WriteSyncRegistrationRecordForPrBehavior(CDataStoreScratch* store, const SSyncRegistration* reg);

namespace NIpse {

void CPrBehavior::Save(void* writer)
{
    CDataStoreScratch* store = (CDataStoreScratch*)writer;

    store->WriteDword(m_field24);
    store->WriteDword(m_field28);
    store->WriteDwordAlt2(m_field2C);
    store->WriteDwordAlt2(m_flags);
    store->WriteDword(m_field34);

    WriteSyncRegistrationRecordForPrBehavior(store, m_registration);
    WriteSyncRegistrationRecordForPrBehavior(store, m_channelReg);
}

}  // namespace NIpse
