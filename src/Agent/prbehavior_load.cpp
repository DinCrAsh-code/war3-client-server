//============================================================================
//  0x6F4AA7C0 - NIpse::CPrBehavior::Load, vtable slot 10, the Save
//  counterpart (prbehavior_save.cpp). See prbehavior.h's own comment.
//
//  Own translation unit: both CDataStore::ReadDword/ReadDwordAlt3 calls and
//  both LoadSyncRegistrationFromStore calls (prbehaviorloadhelpers.cpp) are
//  real out-of-line calls in the shipped body.
//============================================================================
#include "prbehavior.h"
#include "cdatastore.h"

struct STimeSyncPoolSelector
{
    void* SelectSubsystem(void* self);
};

extern "C" void* __fastcall LoadSyncRegistrationFromStore(void* store, void* subsystem, void* self);

namespace NIpse {

void CPrBehavior::Load(void* reader)
{
    CDataStore* store = (CDataStore*)reader;

    //  The shipped body walks the first three fields with one
    //  incrementally-advanced pointer (`&m_field24`, then `+1`, then `+= 2`
    //  before the third read) and only goes back to fresh `&m_flags`/
    //  `&m_field34` lookups for the last two - the same shape
    //  CPrBehavior::Save's own field order mirrors on the write side
    //  (prbehavior_save.cpp).
    unsigned int* p = &m_field24;
    store->ReadDword(p);
    store->ReadDword(p + 1);
    p += 2;
    store->ReadDwordAlt3(p);
    store->ReadDwordAlt3(&m_flags);
    store->ReadDword(&m_field34);

    void* subsystem = ((STimeSyncPoolSelector*)g_pTimeSync)->SelectSubsystem(this);

    m_registration = (SSyncRegistration*)LoadSyncRegistrationFromStore(store, subsystem, this);
    m_channelReg   = (SSyncRegistration*)LoadSyncRegistrationFromStore(store, subsystem, this);
}

}  // namespace NIpse
