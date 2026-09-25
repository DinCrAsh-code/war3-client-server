//============================================================================
//  0x6F67DEF0 - Net::NetClient's vtable slot 0 (agent-networking-
//  wave4-2026-08-29).  NOT a destructor - wave 3's own working name for
//  slot 0 was a guess from seeing only method_index 0 of an
//  under-queried vtable.  This is the real per-connection
//  receive-data-append path: given a `[Src,Size)` byte range, it finds the
//  connection record matching `this->m_recordKey` (this->field4) in a
//  global table, appends the bytes either into the tail chunk already
//  queued for that connection (if it still has room under a 0x400-byte
//  cap) or as a freshly acquired chunk threaded onto the connection's own
//  record list, then flushes whatever pending record is now due to send
//  before releasing the global table lock.
//
//  Verb-decode search, wave 4 phase 1 conclusion: this function (and its
//  whole closure - AcquireChunk/ReleaseChunk/FlushPendingRecords/
//  SendQueuedRecord, all reconstructed this session) never inspects the
//  bytes it appends beyond a length and a CRC. It is pure byte-plumbing:
//  append incoming bytes to a per-connection buffer, later re-frame a
//  0xF7/0x26-tagged record around whatever has accumulated and hand it to
//  a socket-shaped send slot. Nothing here reads a `GameActionId`/verb
//  byte out of the payload, and nothing here is a dispatcher - there is no
//  branch on payload content anywhere in this call tree. That closes this
//  slot as a lead for the verb-decode question: the decode point is
//  neither here nor reachable from here. See
//  docs/targets/NetRouter_NetPlayer_NetClient_oneslot_sweep.md's wave 4
//  section for the follow-on ActionQueueRecord search this finding
//  motivated.
//
//  Record layout: only the fields this call tree actually touches are
//  named; everything else is reserved padding. `+0x208`/`+0x210` are two
//  independent record lists (not one `TSExplicitList` spanning both -
//  `+0x210` is walked through a plain `field+4` "next" in
//  FlushPendingRecords, not through a masked `m_prevlink`, which is what a
//  real TSExplicitList's own tail end would use); `+0x208` is read once,
//  through the masked "positive pointer or 0" idiom
//  docs/msvc-vc8-idioms.md documents for an embedded TSExplicitList's own
//  `Tail()` - modelled here as the same masked read via a small helper
//  rather than a full `TSExplicitList<T>` instance, since only that one
//  access is established.
//============================================================================
#include <windows.h>
#include "cdatarecycler.h"

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int size);
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);
void* __fastcall AcquireChunk(CDataRecycler* pool);
extern "C" void __fastcall FlushPendingRecords(void* connRecord);

struct NetClientRecordFields
{
    char m_reserved000[0x8];
    void* m_nextRecord;   // +0x08 - global-table walk link
    unsigned int m_recordKey; // +0x0C
    char m_reserved010[0x130 - 0x10];
    int  m_state;          // +0x130
    char m_reserved134[0x148 - 0x134];
    void* m_provider;      // +0x148
    char m_reserved14C[0x184 - 0x14C];
    CRITICAL_SECTION m_critSec;   // +0x184, sizeof(CRITICAL_SECTION) == 0x18
    char m_reserved19C[0x1E8 - (0x184 + sizeof(CRITICAL_SECTION))];
    CDataRecycler m_chunkPool; // +0x1E8
    char m_reserved1F0[0x208 - (0x1E8 + sizeof(CDataRecycler))];
    void* m_insertListHead; // +0x208 - masked "Tail()" of an embedded, otherwise-unmodelled TSExplicitList
    void* m_pendingCursor;  // +0x210 - plain singly-linked pending-flush cursor (see FlushPendingRecords)
};

//  A masked field is only ever a positive object pointer or a poisoned
//  non-positive encoding - docs/msvc-vc8-idioms.md's own "masked Tail()"
//  idiom, `prevlink > 0 ? (T*)prevlink : 0`.
static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

struct AppendReceivedDataSelf
{
    void AppendReceivedData(const void* src, unsigned int size);
};

void AppendReceivedDataSelf::AppendReceivedData(const void* src, unsigned int size)
{
    void* self = this;
    unsigned int key = *(unsigned int*)((char*)self + 4);
    if (!key)
        return;

    g_netClientRecordTableGuard.Wait(0);

    NetClientRecordFields* rec = (NetClientRecordFields*)MaskedPositive(g_netClientRecordListHead);
    while (rec && rec->m_recordKey != key)
        rec = (NetClientRecordFields*)MaskedPositive((int)rec->m_nextRecord);

    if (!rec)
    {
        g_netClientRecordTableGuard.SignalOne(0);
        return;
    }

    if (rec->m_state < 4 || rec->m_state < 7)
    {
        g_netClientRecordTableGuard.SignalOne(0);
        return;
    }

    void* chunk = AcquireChunk(&rec->m_chunkPool);

    unsigned int ticks = GetTickCount();
    *(unsigned int*)((char*)chunk + 8) = ticks;
    *(unsigned int*)((char*)chunk + 0xC) = size;
    memcpy((char*)chunk + 0x18, src, size);

    EnterCritSec(&rec->m_critSec);

    bool consumed = false;
    if (rec->m_pendingCursor)
    {
        void* headChunk = rec->m_insertListHead;
        void* tail = MaskedPositive(*(int*)((char*)*(void**)headChunk + 4));
        unsigned int tailLen = *(unsigned int*)((char*)tail + 0xC);
        if (tailLen + size <= 0x400)
        {
            memcpy((char*)tail + 0x18 + tailLen, src, size);
            *(unsigned int*)((char*)tail + 0xC) = tailLen + size;
            consumed = true;
        }
    }

    if (!consumed)
    {
        //  Unlink `chunk` from wherever it currently sits (harmless no-op
        //  for a freshly acquired one) and push it to the front of the
        //  +0x208 insertion list, donating its own prevlink chain the way
        //  TSExplicitList<T>::LinkAfter's own five stores do
        //  (storm.h) - `where` here is the raw +0x208 slot itself, not a
        //  full TSExplicitList instance.
        void** link = (void**)chunk;
        if (link[0])
        {
            void* next = link[0];
            int prevlink = (int)link[1];
            void** prevSlot;
            if (prevlink <= 0)
                prevSlot = (void**)~prevlink;
            else
                prevSlot = (void**)(prevlink + ((int)chunk - *(int*)((char*)next + 4)));
            *prevSlot = next;
            void** n2 = (void**)link[0];
            void* p2 = link[1];
            *((void**)n2 + 1) = p2;
            link[0] = 0;
            link[1] = 0;
        }

        void* oldHead = rec->m_insertListHead;
        link[0] = oldHead;
        link[1] = *((void**)oldHead + 1);
        *((void**)oldHead + 1) = chunk;
        rec->m_insertListHead = chunk;

        if (!rec->m_pendingCursor)
            rec->m_pendingCursor = chunk;

        chunk = 0;
    }

    if (rec->m_state == 7)
        FlushPendingRecords(rec);

    LeaveCritSec(&rec->m_critSec);

    if (chunk)
        rec->m_chunkPool.ReleaseNode(chunk);

    g_netClientRecordTableGuard.SignalOne(0);
}
