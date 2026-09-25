//============================================================================
//  CDataStoreCache<1460> - a CDataStore-shaped read view whose vtable slot 0
//  override points m_field4 at its own inline 1460-byte buffer (this+0x18)
//  instead of externally pool-owned storage, and whose destructor releases
//  that buffer through a second cross-module function pointer
//  (off_6F932D30, the same convention as CDataStoreScratch's own
//  off_6F970E54 - see cdatastorescratch.cpp) before falling into
//  CDataStore::~CDataStore's own shared tail.
//
//  1460 = 0x5B4, read directly off this class's own OnConstruct body
//  (sub_6F2C93D0) rather than off the mangled template name IDA shows
//  (`??_7?$CDataStoreCache@$0FLE@@@6B@` - `$0FLE@` is MSVC's encoding for
//  the same int template argument).  Reached from
//  ServerTock_0x27_PacketHandler's own SEH-protected Tock/GameAction-relay
//  builders (sub_6F6690F0/sub_6F6691F0) as a short-lived local.
//============================================================================
#include "cdatastore.h"
#include "storm.h"   // placement new

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);

class CDataStoreCache1460 : public CDataStore
{
public:
    //  0x6F2C9290
    CDataStoreCache1460();
    //  0x6F2C95B0
    ~CDataStoreCache1460();

    //  vtable slot 0 override (called from the constructor, same
    //  not-yet-fully-constructed-vtable shape as CDataStoreScratch's own
    //  OnConstruct - see that class's own header comment).
    virtual void m_reservedSlot0() { OnConstruct(&m_field4, &m_field8, &m_field0xC); }

    //  0x6F2C93D0 - points m_field4 at this object's own inline buffer and
    //  sets m_field0xC to the buffer's fixed size; m_field8 (the "backing
    //  capacity" CDataStore's own header keeps unresolved) is left
    //  untouched, matching the target's own body exactly (only two of the
    //  three out-params are ever written here). Defined out-of-line, like
    //  ReleaseBuffer below, purely so both get their own compiled body -
    //  neither is ODR-used from anywhere reachable in this call tree
    //  (OnConstruct only through this class's own vtable slot 0, which
    //  m_reservedSlot0's own inline body already calls directly; ReleaseBuffer
    //  only through the unassigned off_6F932D30 pointer), and an in-class
    //  inline definition with no real call site is never emitted at all.
    void OnConstruct(void** pField4, int* pField8, int* pField0xC);

    //  0x6F2C93F0 - the off_6F932D30 target: releases m_field4 back through
    //  SMemFree unless it still points at this object's own inline buffer
    //  (nothing to free) or is already null, then always clears
    //  *pField4/*pField0xC. Same shape as sub_6F4C1CD0's own cleanup, one
    //  field fewer (no separate "owns it" flag - the inline-buffer address
    //  comparison serves that purpose here) and a different trailing
    //  condition (this class's own inline buffer vs a plain non-null check).
    void ReleaseBuffer(void** pField4, void* pField8, int* pField0xC);

    char m_inlineBuffer[1460];   // +0x18
};

void CDataStoreCache1460::OnConstruct(void** pField4, int* pField8, int* pField0xC)
{
    (void)pField8;
    *pField4 = m_inlineBuffer;
    *pField0xC = 1460;
}

void CDataStoreCache1460::ReleaseBuffer(void** pField4, void* /*pField8, unused*/, int* pField0xC)
{
    void* buffer = *pField4;
    if (buffer != m_inlineBuffer && buffer != 0)
        SMemFree(buffer, "e:\\drive1\\temp\\buildwar3x\\engine\\source\\netgame.cpp", 0x108, 0);
    *pField4 = 0;
    *pField0xC = 0;
}

typedef void(__thiscall* CDataStoreCacheCleanupFn)(CDataStoreCache1460* self, void** pField4, void* pField8, int* pField0xC);
extern "C" CDataStoreCacheCleanupFn off_6F932D30;

//----------------------------------------------------------------------------
//  0x6F2C9290
//----------------------------------------------------------------------------
CDataStoreCache1460::CDataStoreCache1460()
{
    m_field4 = 0;
    m_field8 = 0;
    m_field0xC = 0;
    m_field10 = 0;
    m_readPos = -1;
    m_reservedSlot0();   // OnConstruct overwrites m_field0xC to 1460
}

//----------------------------------------------------------------------------
//  0x6F2C95B0 - m_readPos is never read again after the constructor sets it;
//  both this destructor and CDataStore::~CDataStore's own shared tail gate
//  on m_field0xC instead, exactly as CDataStore::~CDataStore's own entry
//  documents. OnConstruct always leaves m_field0xC at 1460, never -1, so in
//  practice both release calls below always fire.
//----------------------------------------------------------------------------
CDataStoreCache1460::~CDataStoreCache1460()
{
    if (m_field0xC != -1)
        off_6F932D30(this, &m_field4, 0, &m_field0xC);

    //  CDataStore::~CDataStore's own body, inlined - same re-checked
    //  condition (off_6F932D30 may have changed m_field0xC).
    if (m_field0xC != -1)
        g_pfnRecycleBuffer(&m_field4, &m_field8, &m_field0xC);
}

//----------------------------------------------------------------------------
//  Not a separate shipped address of its own - a thin cross-TU exposer so a
//  caller outside this file (CNetData::DispatchQueuedTurnRecord's own
//  applied-turn-delta reporter, netcommand_dispatch.cpp - see
//  networking_desync_closure_worklist.json batch-D) can destroy a
//  CDataStoreCache1460-shaped object in place through the real,
//  already-reconstructed 0x6F2C95B0 body above rather than a second naked
//  `mov eax, 06F2C95B0h` / `jmp eax` thunk to the same address under a
//  different local name - CLAUDE.md's own "grep src/ for its address"
//  rule, applied: a hit means call the reconstruction that is already
//  there instead of re-thunking it.  This wrapper's own compiled body is
//  a genuine out-of-line call (no dump heading of its own to score
//  against; the callee's own dump/score is unaffected).
//----------------------------------------------------------------------------
void __fastcall CDataStoreCache1460_DestroyInPlace(void* self)
{
    ((CDataStoreCache1460*)self)->~CDataStoreCache1460();
}

//----------------------------------------------------------------------------
//  The construct-in-place counterpart the comment above already anticipated:
//  CNetData_AppliedDeltaReporterCtor (netcommand_dispatch.cpp) builds a
//  CDataStoreCache1460 base subobject through the real 0x6F2C9290 body
//  above, then overwrites the vtable with its own derived
//  `Net::CTurnsSyncStore` one - exactly the "vtable stamped twice, base
//  first" shape a real derived-class constructor produces, so placement-new
//  through the already-reconstructed constructor is the honest way to
//  reach it rather than a second naked thunk to the same address.
//----------------------------------------------------------------------------
void __fastcall CDataStoreCache1460_ConstructInPlace(void* self)
{
    new (self) CDataStoreCache1460();
}
