//============================================================================
//  CDataStoreCache<128> - the same class cdatastorecache.cpp reconstructs at
//  1460, instantiated at 128 and at its own two addresses.  `$0IA@` is
//  MSVC's encoding of 128 in the vtable symbol IDA shows
//  (`??_7?$CDataStoreCache@$0IA@@@6B@`, 0x6F970EE0), and the object is 0x98
//  bytes: 0x18 of CDataStore plus the 0x80-byte inline buffer.
//
//  Reached from the save game's slot-record encode/decode pair
//  (gamesetupslotrecord.cpp) as a short-lived local, which is also where the
//  0x80 bound is tested - a record that would not fit in the buffer is
//  refused rather than grown.
//
//  Where the 1460 instantiation reaches its buffer-release through the
//  cross-module pointer off_6F932D30, this one reaches its own through
//  off_6F970EE4 - which is the *second slot of its own vtable*, loaded by
//  absolute address rather than through `this` because inside a destructor
//  the vptr's value is a constant.  Modelled as a function pointer for the
//  same reason cdatastorecache.cpp models off_6F932D30 that way: the slot's
//  own body (0x6F654C10) is in none of these dumps.
//============================================================================
#include "cdatastorecache128.h"

extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);

void CDataStoreCache128::OnConstruct(void** pField4, int* pField8, int* pField0xC)
{
    (void)pField8;
    *pField4 = m_inlineBuffer;
    *pField0xC = 128;
}

typedef void(__thiscall* CacheReleaseFn)(CDataStoreCache128* self, void** pField4,
                                         int* pField8, int* pField0xC);
extern "C" CacheReleaseFn off_6F970EE4;

//----------------------------------------------------------------------------
//  0x6F654B70
//----------------------------------------------------------------------------
CDataStoreCache128::CDataStoreCache128()
{
    m_field4 = 0;
    m_field8 = 0;
    m_field0xC = 0;
    m_field10 = 0;
    m_readPos = -1;
    m_reservedSlot0();
}

//----------------------------------------------------------------------------
//  0x6F654DD0 - the two-stage cleanup CDataStoreScratch::~CDataStoreScratch
//  and CDataStoreCache<1460>::~CDataStoreCache<1460> both have: this class's
//  own release, then CDataStore::~CDataStore's body inlined, re-testing the
//  same field because the first call may have changed it.
//----------------------------------------------------------------------------
CDataStoreCache128::~CDataStoreCache128()
{
    if (m_field0xC != -1)
        off_6F970EE4(this, &m_field4, &m_field8, &m_field0xC);

    if (m_field0xC != -1)
        ((RecycleBufferFn)g_pfnRecycleBuffer)(this, &m_field4, &m_field8, &m_field0xC);
}
