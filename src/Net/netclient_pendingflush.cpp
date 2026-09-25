//============================================================================
//  agent-networking-wave4-2026-08-29 - the three functions
//  AppendReceivedData's own slot-0 callee sub_6F6797A0 pulls in: the
//  per-record retry/backoff gate (0x6F663CC0), the record-flush itself
//  (0x6F674520, builds a small CDataStoreScratch header+CRC packet and
//  sends it), and the tiny vtable-slot-11 send dispatcher both
//  0x6F674520 and 0x6F683DA0's own family reach (0x6F6DAE20).
//
//  0x6F6797A0 itself walks the connection record's own +0x210 pending-list
//  (the SAME field AppendReceivedData, netclient_recvappend.cpp, both
//  tests as a "anything pending?" gate and threads chunks onto), retrying
//  each node against a jittered interval clamped to >= 200ms before
//  sending it, same shape ArmRetryTimer (playertable_installhandler.cpp)
//  already establishes for this neighbourhood's other retry logic.
//============================================================================
#include "cdatarecycler.h"
#include "cdatastorescratch.h"

extern "C" unsigned int __stdcall GetTickCount(void);
void __stdcall SErrSetLastError(unsigned int code);

//----------------------------------------------------------------------------
//  0x6F663CC0 - `__fastcall(ecx=state, edx=intervalMs)` + two stack params.
//  Advances `*state` (a GetTickCount-domain deadline) by `intervalMs`,
//  clamping `*state` up to "now" first if it had already fallen behind.
//  If the newly advanced deadline is already under 15s (0x3A98 ms) away -
//  the ordinary case for a short retry interval - reports "due now" (1).
//  Otherwise (a long interval that has not naturally closed in yet):
//  unless the caller opted in (`allowEscalate`), undoes the advance and
//  reports "not due" (0); if the caller did opt in and the deadline is
//  45s (0xAFC8 ms) or further out, additionally flags
//  `*outLongSilence = 1` before reporting "not due" (0) regardless.
//----------------------------------------------------------------------------
int __fastcall CheckAndAdvanceInterval(int* state, unsigned int intervalMs,
                                       int allowEscalate, int* outLongSilence)
{
    unsigned int now = GetTickCount();
    if ((int)(*state - now) < 0)
        *state = now;
    *state += intervalMs;

    unsigned int remaining = *state - now;
    if (remaining < 0x3A98)
        return 1;

    if (!allowEscalate)
    {
        *state -= intervalMs;
        return 0;
    }

    if (remaining >= 0xAFC8)
        *outLongSilence = 1;
    return 0;
}

//  0x6F6DAE20 - already reconstructed as `DispatchVtableSlot11`
//  (src/Net/cdatastorescratch_writers.cpp) from the ServerTock/BuildRelay
//  session; this TU only calls it. Grepping for the address first (per
//  CLAUDE.md) would have caught the duplicate this file used to carry
//  under a second name (`SendThroughSlot`) before this comment was written -
//  removed in favour of the one real definition.
void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//----------------------------------------------------------------------------
//  0x6F674520 - build one small CDataStoreScratch packet (a fixed 0xF7
//  byte, a fixed 0x26 word, a 16-bit length, and a CRC32 of the record's
//  own payload) and hand it to DispatchVtableSlot11 via the connection record's
//  own +0x148->+0x3C socket-like object. Carries a real
//  `__try`/`__finally`-shaped SEH frame this build's fixed `/GS- /EHs-c-`
//  cannot reproduce (docs/msvc-vc8-idioms.md, "An __except_handler4-shaped
//  frame this toolchain cannot reproduce") - a local CDataStoreScratch
//  with a non-trivial destructor is enough on its own to make MSVC build
//  one, no explicit `__try` in the source required. Reconstructed as plain
//  C++; the frame furniture itself does not reproduce, the body past it
//  does (see BEHAVIOUR).
//
//  `record`'s own two touched fields (+0xC a byte count, +0x18 the payload
//  itself) are named narrowly - nothing else about this record's real
//  layout is established by this call tree.
//----------------------------------------------------------------------------
struct NetClientPendingRecordFields
{
    char         m_reserved00[0xC];
    unsigned int m_length;   // +0x0C
    char         m_payload[4]; // +0x18 conceptually - flexible, only the base address is used
};

CDataStoreScratch* __fastcall NetMsgWriteDword_6F683DA0(CDataStoreScratch*, const unsigned int*);
unsigned int __fastcall Crc32(const void* data, unsigned int length);

void __fastcall SendQueuedRecord(void* connRecord, void* recordNode)
{
    NetClientPendingRecordFields* rec = (NetClientPendingRecordFields*)recordNode;
    unsigned int length = rec->m_length;
    const void* payload = (const char*)recordNode + 0x18;

    unsigned int crc = Crc32(payload, length);

    unsigned short packetLen = (unsigned short)(length + 8);
    const void* body = (const char*)payload + (length - packetLen);

    CDataStoreScratch scratch(const_cast<void*>(body), (void*)(unsigned int)packetLen);
    scratch.WriteByte(0xF7);
    scratch.WriteWord(0x26);
    scratch.WriteWord(packetLen);
    NetMsgWriteDword_6F683DA0(&scratch, &crc);

    void* provider = *(void**)((char*)connRecord + 0x148);
    void* sink = *(void**)((char*)provider + 0x3C);
    DispatchVtableSlot11(sink, (int)body, packetLen);
}

//----------------------------------------------------------------------------
//  0x6F6797A0 - walk the connection record's own +0x210 pending-record
//  list, gating each send through CheckAndAdvanceInterval with a per-record
//  interval (`record->field0xC`, clamped up to a 200ms floor). The gate is
//  a stop condition, not a per-node skip: the moment one record is not yet
//  due, the whole walk ends right there (leaving +0x210 pointed at that
//  record for next time), rather than skipping past it to try the rest of
//  the list.
//----------------------------------------------------------------------------
extern "C" void __fastcall FlushPendingRecords(void* connRecord)
{
    void* provider = *(void**)((char*)connRecord + 0x148);
    if (!*(int*)((char*)provider + 0x3C))
        return;
    void* node = *(void**)((char*)connRecord + 0x210);
    if (!node)
        return;

    int* intervalState = (int*)((char*)connRecord + 0x200);
    for (;;)
    {
        unsigned int intervalMs = *(unsigned int*)((char*)node + 0xC);
        if (intervalMs < 0xC8)
            intervalMs = 0xC8;

        int longSilence = 2;
        if (!CheckAndAdvanceInterval(intervalState, intervalMs, 0, &longSilence))
            return;

        SendQueuedRecord(connRecord, node);

        void* next = *(void**)((char*)node + 4);
        node = ((int)next <= 0) ? 0 : next;
        *(void**)((char*)connRecord + 0x210) = node;
        if (!node)
            return;
    }
}
