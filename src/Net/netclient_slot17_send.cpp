//============================================================================
//  Net::NetClient vtable slot 17 (0x6F67E0C0) - networking wave 9,
//  agent-networking-wave3-2026-08-29.
//
//  Shares the record-table-walk/dual-`state<4`-then-`state<7` shape
//  netclient_recvappend.cpp's own AppendReceivedData already establishes
//  for slot 0 (same two separate comparisons, same convergent single
//  `ConditionVariable::SignalOne` exit - this slot does NOT carry the
//  InstallSession/slot-1 tail-duplication defect; both `jl`s land on the
//  identical shared block in the disassembly).
//
//  Once past the gate, this slot:
//   1. builds a small header ({0xF7, 0x27, word length}) written directly
//      INTO the caller's own `srcBytes` buffer, five bytes *before*
//      `srcBytes` itself (`body = srcBytes - 5`, computed the same
//      backward-offset way SendQueuedRecord's own `body` is - see
//      netclient_pendingflush.cpp) - the caller is expected to have left
//      that slack. The frame's own passthrough callback (0x6F683AF0) is a
//      genuine no-op (`return this`, nothing written) - the header alone
//      is the whole "payload", already-formatted bytes the caller
//      supplied are covered directly by the length field;
//   2. acquires one node from a SECOND, differently-sized `CDataRecycler`
//      embedded at `+0x214` (the record's own receive-chunk pool at
//      `+0x1E8`, netclient_chunkpool.cpp, uses 0x418-byte chunks; this one
//      uses 0x74-byte chunks) and `memcpy`s the caller's own `[srcBytes,
//      srcBytes+len)` range into it at `+0xC`, storing the length itself
//      at `+0x8`;
//   3. under the record's own critical section (`+0x184`), decides a
//      "haven't heard in a while" flag off `+0x1A0`/`+0x1A4` (a tick-count-
//      shaped pair this call tree does not otherwise establish - modelled
//      as raw ints, not a named idiom), conditionally flushes pending
//      records if `state == 7` (`FlushPendingRecords`,
//      netclient_pendingflush.cpp), and always pushes the just-filled node
//      into a small fixed-size ring buffer at `+0x22C`
//      (`RingBufferPushEvict`), releasing whatever chunk the ring evicted
//      back to the `+0x214` pool;
//   4. outside the lock, if the flag was set, dispatches the just-built
//      header straight through `+0x148->+0x3C`'s own vtable slot 11.
//============================================================================
#include <windows.h>
#include "cdatarecycler.h"
#include "cdatastorescratch.h"
#include "storm.h"

extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int size);

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

//  netclient_recvappend.cpp - the same global guard and list head every
//  other NetClient record-table walk in this directory uses.
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);
extern "C" void __fastcall FlushPendingRecords(void* connRecord);
void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//----------------------------------------------------------------------------
//  0x6F683AF0 - the frame's own passthrough callback: a genuine no-op.
//  `return this`, nothing written - see the file header above for why.
//  Written naked (matching NetClientBuildARelay_Callback's own reasoning,
//  netclient_buildarelay.cpp) so this build's /O2 cannot see through the
//  trivial body and optimise the call away entirely - the shipped code
//  keeps a real out-of-line call here too.
//----------------------------------------------------------------------------
__declspec(naked) CDataStoreScratch* __fastcall NoOpFrameCallback(CDataStoreScratch* /*self*/, void* /*passthrough*/)
{
    __asm
    {
        mov eax, ecx
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F67C5B0 - the `+0x214` pool's own acquire helper: identical shape to
//  AcquireChunk (netclient_chunkpool.cpp) - TryPopChunk first, grow to
//  exactly 0x74 bytes through this object's own vtable slot 3 (`Realloc`)
//  if the popped chunk is not already that big - just a different target
//  size. Reached the same way AcquireChunk is (`ecx` pointed at the
//  sub-object directly, `retn` with no operand), so modelled the same way:
//  a free function over `CDataRecycler*` instead of a class member.
//----------------------------------------------------------------------------
typedef void* (__thiscall* CDataRecyclerReallocSlot)(void*, void*, unsigned int, unsigned int*, const char*, int);

void* __fastcall AcquirePoolNode74(CDataRecycler* pool)
{
    void* ptr = 0;
    unsigned int size = 0;
    pool->TryPopChunk(&ptr, &size, 0, 0);

    void* chunk;
    if (size >= 0x74)
    {
        chunk = ptr;
    }
    else
    {
        CDataRecyclerReallocSlot reallocSlot = (CDataRecyclerReallocSlot)(*(void***)pool)[3];
        chunk = reallocSlot(pool, ptr, 0x74, &size, 0, 0);
    }

    if (chunk)
    {
        *(unsigned int*)chunk = 0;
        *((unsigned int*)chunk + 1) = 0;
    }
    return chunk;
}

//----------------------------------------------------------------------------
//  0x6F67D680 - the `+0x214` pool's own release helper: unlink `node` from
//  whatever intrusive list it is on (the same poison-tagged `TSLink<T>`
//  shape CDataRecycler::ReleaseNode already uses, netclient_chunkpool.cpp -
//  a distinct instantiation since this pool's own element type is not
//  established to be the same one either), then hand it to `pool`'s own
//  ReleaseChunk with a fixed 0x74-byte size. `ecx` (the pool) is never
//  touched by the unlink itself and flows straight through to the
//  ReleaseChunk call unmodified - a real `CDataRecycler*` thiscall member,
//  same shape ReleaseNode's own header already documents.
//----------------------------------------------------------------------------
struct UnknownChunkNode74_6F67D680;
typedef TSLink<UnknownChunkNode74_6F67D680> ChunkNode74Link;

//  Truncated the same way aAuclientturnNe/aE_1 already are - IDA's own
//  listing comment names it "type descriptor name" but does not spell it
//  out, and the exact bytes make no difference to the score (both sides
//  canonicalise the operand to SYM).
static const char aAuclientturnss[] = "?AU...";

void CDataRecycler::ReleaseNode74(void* node)
{
    ((ChunkNode74Link*)node)->Unlink();
    ReleaseChunk(node, 0x74, aAuclientturnss, -2);
}

//  0x6F677300 - netclient_ringbuffer.cpp, split into its own TU so this
//  build's /O2 does not inline it back into this file's one call site.
struct RingBufferSlot17
{
    void* Push(void* value);

    void*        m_unused0;   // +0x00 - not read by this function
    unsigned int m_capacity;  // +0x04
    void**       m_slots;     // +0x08
    unsigned int m_index;     // +0x0C
};

//  Only the fields this slot itself reads/writes; see the sibling files'
//  own header comments for why these stay raw offset casts rather than
//  one shared record struct.
struct NetClientRecordFieldsSlot17
{
    char m_reserved000[0xC];
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x130 - 0x10];
    int   m_state;             // +0x130
    char m_reserved134[0x148 - 0x134];
    void* m_provider;          // +0x148
    char m_reserved14C[0x184 - 0x14C];
    CRITICAL_SECTION m_critSec; // +0x184, sizeof == 0x18
    char m_reserved19C[0x1A0 - (0x184 + sizeof(CRITICAL_SECTION))];
    int   m_field1A0;           // +0x1A0
    int   m_field1A4;           // +0x1A4
    char m_reserved1A8[0x214 - 0x1A8];
    CDataRecycler m_pool74;      // +0x214, sizeof == 0x18
    RingBufferSlot17 m_ring;      // +0x22C
};

struct NetClientDispatchSlot17Self
{
    void SendFramedChunk(const void* srcBytes, unsigned int length);
};

//----------------------------------------------------------------------------
//  0x6F67E0C0 - Net::NetClient vtable slot 17.
//----------------------------------------------------------------------------
void NetClientDispatchSlot17Self::SendFramedChunk(const void* srcBytes, unsigned int length)
{
    void* self = this;
    unsigned int key = *(unsigned int*)((char*)self + 4);
    if (!key)
        return;

    g_netClientRecordTableGuard.Wait(0);

    //  Same do-while walk shape netclient_slot1_send.cpp's own root uses:
    //  only the list head goes through the full MaskedPositive normalisation;
    //  each node's own `next` step is a plain `> 0` continuation test.
    NetClientRecordFieldsSlot17* rec = (NetClientRecordFieldsSlot17*)MaskedPositive(g_netClientRecordListHead);
    bool found = false;
    if (rec)
    {
        do
        {
            if (rec->m_recordKey == key)
            {
                found = true;
                break;
            }
            rec = (NetClientRecordFieldsSlot17*)*(void**)((char*)rec + 0x8);
        } while ((int)rec > 0);
    }

    if (found && (rec->m_state < 4 || rec->m_state < 7))
        found = false;

    if (found)
    {
        //  Header word length is `length + 5`, truncated to 16 bits (the
        //  frame's own {0xF7, 0x27, word} is written 5 bytes *before*
        //  `srcBytes`, into slack the caller left there).
        unsigned short frameLen = (unsigned short)(length + 5);
        void* body = (char*)srcBytes - (unsigned int)frameLen + length;

        {
            CDataStoreScratch writer(body, (void*)(unsigned int)frameLen);
            writer.WriteByte(0xF7);
            writer.WriteByte(0x27);
            writer.WriteWord(frameLen);
            NoOpFrameCallback(&writer, (void*)&length);
        }

        //  The pool node holds the *whole* framed packet (header included) -
        //  `frameLen` bytes copied starting at `body`, not just the raw
        //  payload the caller passed in.
        void* node = AcquirePoolNode74(&rec->m_pool74);
        *((unsigned int*)node + 2) = frameLen;
        memcpy((char*)node + 0xC, body, frameLen);

        EnterCritSec(&rec->m_critSec);

        bool shouldDispatch = true;
        if (rec->m_field1A0 != 0)
        {
            if (rec->m_field1A4 == rec->m_field1A0)
                rec->m_field1A0 = 0;
            else
                shouldDispatch = false;
        }

        if (rec->m_state == 7)
            FlushPendingRecords(rec);

        rec->m_field1A4 += 1;

        //  The freshly filled node is always what gets released if the ring
        //  push never happens (`shouldDispatch == false`); otherwise it is
        //  whatever the ring buffer just evicted (possibly the same node
        //  right back, possibly an older one, possibly nothing at all).
        void* released = node;
        if (shouldDispatch)
            released = rec->m_ring.Push(node);

        LeaveCritSec(&rec->m_critSec);

        if (released)
            rec->m_pool74.ReleaseNode74(released);

        if (shouldDispatch)
        {
            void* provider = rec->m_provider;
            if (provider)
            {
                void* target = *(void**)((char*)provider + 0x3C);
                DispatchVtableSlot11(target, (int)body, frameLen);
            }
        }
    }

    g_netClientRecordTableGuard.SignalOne(0);
}
