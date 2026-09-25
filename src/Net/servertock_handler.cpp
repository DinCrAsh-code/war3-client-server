//============================================================================
//  0x6F66FCA0 - ServerTock_0x27_PacketHandler, the root of this dump: the
//  host-side handler for PacketId.Tock (0x27), queuing a raw, still
//  page-relative circular-queue node built from the packet's own payload
//  into the session's own record, then kicking off relay processing the
//  first time the queue goes from empty to non-empty.
//
//  Shape:
//    1. Guard: self->0x40 must be set (the same session/ready pointer
//       GameAction_0x26_PacketHandler's own entry documents).
//    2. Build a local, non-owning CDataStore view exactly like
//       GameAction_0x26_PacketHandler's own read-side local does, but
//       "read" it through NoOpReadHeader rather than ReadHeaderDword - Tock
//       packets carry no checksum header to actually validate (see that
//       function's own comment). The local's own destructor (inlined, same
//       unreproducible SEH-frame shape as GameAction_0x26_PacketHandler's
//       own local) still runs regardless.
//    3. declaredLength-1 must be in [1, 0x60] - reject (return 1) outside
//       that range.
//    4. Find-or-create this player's own action-queue record
//       (g_actionQueueTable, keyed by self->0x40) exactly like
//       GameAction_0x26_PacketHandler does.
//    5. Under that record's own lock (only when its own state is 6):
//       reserve room in a fixed 0x800-byte slab (ActionQueueRecordFreeList),
//       falling back to a fresh SMemAlloc'd node when the slab is full,
//       memcpy the packet's payload (skipping its own first byte) into it,
//       and push it onto the session's own pending-Tock circular queue
//       (self->0x64, tail-pointer convention). The first node ever queued
//       (the list was empty before this insert) additionally kicks off
//       relay processing (sub_6F66D630, not reconstructed this session -
//       see its own file).
//
//  Field names below are addresses this session did not further identify -
//  kept as raw offset casts per CLAUDE.md's own rule, matching
//  gameaction0x26_handler.cpp's own convention for the same struct.
//============================================================================
#include <windows.h>
#include "cdatastore.h"

extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int count);
extern "C" unsigned int g_actionQueueSignal;   // unk_6FACFF20

void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

CDataStore* __fastcall NoOpReadHeader(CDataStore* self, unsigned int* out);
void __fastcall OnTockQueued(void* record, void* self, unsigned int hasPending);

//  `struct`, not `class`, and deliberately so: gameaction0x26_handler.cpp
//  declares the same global with the same layout, and MSVC encodes the
//  struct/class keyword into the *variable's* mangled name - `class` here
//  emitted ?g_actionQueueTable@@3VActionQueueTable@@A while that file
//  emitted ...@@3UActionQueueTable@@A, so the one global became two distinct
//  unresolved externals.  Keep the two declarations spelled identically.
struct ActionQueueTable
{
    void* FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                              unsigned int arg4, unsigned int createIfMissing);
};
extern ActionQueueTable g_actionQueueTable;   // stru_6FACFF00

struct ConditionVariable
{
    void SignalOne(unsigned int value);
};

class ActionQueueRecordFreeList
{
public:
    void* AllocateFreeNode(unsigned int arg);
};

class ActionQueueRecordList
{
public:
    void* GetLinkOrDefault(void* link);
    void Relink(void* unlinkArg, unsigned int insertCount, void* targetList);
    void* AllocateAndLink(void* linkArg, unsigned int extraSize, unsigned int flags);
};

class RefCountArray
{
public:
    unsigned __int64 IncrementRefCountAt(unsigned int index);
};
extern "C" long dword_6FACFEF8;

int __stdcall ServerTock_0x27_PacketHandler(void* self, const void* src, unsigned int declaredLength)
{
    if (*(void**)((char*)self + 0x40) == 0)
        return 1;

    {
        CDataStore local;
        local.m_field4 = (void*)src;
        local.m_field8 = 0;
        local.m_field0xC = -1;
        local.m_field10 = declaredLength;
        local.m_readPos = 0;
        unsigned int unused;
        NoOpReadHeader(&local, &unused);
        if ((unsigned int)local.m_readPos > (unsigned int)local.m_field10)
            return 1;
    }

    unsigned int payloadLength = declaredLength - 1;
    if (payloadLength == 0 || payloadLength > 0x60)
        return 1;

    const char* payload = (const char*)src + 1;

    void* sessionKey = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    ActionQueueRecordFreeList* record = (ActionQueueRecordFreeList*)
        g_actionQueueTable.FindOrCreateRecord(sessionKey, 0, &wasCreated, 0, 1);

    int status;
    if (!record)
    {
        status = 0;
    }
    else
    {
        char* r = (char*)record;

        if (*(unsigned int*)((char*)self + 0x80) & 0x400000)
        {
            status = 2;
        }
        else if (*(int*)(r + 0xE4) == 7)
        {
            status = 2;
        }
        else if (*(int*)(r + 0xE4) != 6)
        {
            status = 1;
        }
        else
        {
            void* lock = r + 0x188;
            EnterCritSec((CRITICAL_SECTION*)lock);

            *(unsigned int*)((char*)self + 0x60) += 1;
            int capRemaining = *(int*)(r + 0x1A8) - *(int*)((char*)self + 0x60);

            void* node = 0;
            if (capRemaining < 0)
            {
                status = 1;
            }
            else
            {
                //  Try to grab a slot from an existing slab provider,
                //  walking record->0x1C4's own chain (each link's "next"
                //  via ActionQueueRecordList::GetLinkOrDefault) until one
                //  yields a node or the chain runs out.
                int rawChain = *(int*)(r + 0x1C4);
                void* chain = rawChain > 0 ? (void*)(int)rawChain : 0;
                node = 0;
                while (chain != 0)
                {
                    node = ((ActionQueueRecordFreeList*)chain)->AllocateFreeNode(payloadLength);
                    if (node != 0)
                    {
                        ((ActionQueueRecordList*)(r + 0x1BC))->Relink(0, 1, chain);
                        break;
                    }
                    void* next = ((ActionQueueRecordList*)chain)->GetLinkOrDefault(chain);
                    chain = (int)next > 0 ? next : 0;
                }

                if (node == 0)
                {
                    //  No existing provider had room - allocate a fresh
                    //  slab provider, register it (a global refcount bump,
                    //  index 6 - meaning not established), and grab this
                    //  record's own first slot from it.
                    void* freshChain = ((ActionQueueRecordList*)(r + 0x1BC))->AllocateAndLink(0, 0, 1);
                    RefCountArray* refCounts = (RefCountArray*)&dword_6FACFEF8;
                    refCounts->IncrementRefCountAt(6);
                    node = ((ActionQueueRecordFreeList*)freshChain)->AllocateFreeNode(payloadLength);
                }

                memcpy((char*)node + 8, payload, payloadLength);

                void** head = (void**)((char*)self + 0x64);
                if (*head != 0)
                {
                    void* oldHead = *head;
                    void* oldHeadNext = *(void**)oldHead;
                    *(void**)node = oldHeadNext;
                    *(void**)oldHead = node;
                }
                else
                {
                    *(void**)node = node;
                }
                int firstInsert = (*(void**)node == node);
                *head = node;

                *(unsigned int*)((char*)self + 0x5C) = GetTickCount();
                if (capRemaining == 0)
                {
                    void __fastcall OnTockQueued(void*, void*, unsigned int);
                    OnTockQueued(record, self, 0);
                }

                if (firstInsert)
                {
                    void __fastcall FlushPendingTocks(void*, unsigned int);
                    FlushPendingTocks(record, *(unsigned int*)((char*)self + 0x60));
                }

                status = 2;
            }

            LeaveCritSec((CRITICAL_SECTION*)lock);
        }

        if (wasCreated != -1)
            ((ConditionVariable*)&g_actionQueueSignal)->SignalOne(wasCreated != 0);
    }

    return status;
}
