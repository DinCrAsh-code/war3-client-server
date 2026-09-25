//============================================================================
//  batch-H-w3gsaction-and-replaystream - 0x6F66FF90, the largest of this
//  session's remaining single handlers: an ability/hero "progress" vote
//  tracker. ParseDwordByteDwordAction_6F666AE0's own output names a
//  per-client-id "progress record" (found via FindProgressRecordById,
//  sub_6F66FF20, a genuine pointer-chain search over record+0x1FC unlike
//  every other list this tree walks) holding a per-caller-id 0xC-byte
//  table entry (state/currentValue/percent) at +0x128 + callerId*0xC; a
//  fresh value is range-checked against that entry's own stored value and
//  a record-wide cap at +0x110, then stored, its displayed percentage
//  recomputed (0xFF for state 0/unset, 0xFE for state 5/a terminal state,
//  otherwise currentValue*100/cap), and - only on state==1 ("locked in") -
//  AdvancePairing (sub_6F66CA30) runs a real O(n^2) matchmaking pass over
//  every node on record->field0xCC's own list, splitting "locked in and at
//  cap" (completed) from "locked in, not yet at cap" (pending) and pairing
//  the first mutually-flagged completed/pending pair it finds (self+0x48's
//  own per-node bitmask, or the completed node happening to be
//  record->field0xD4's own head - a real, dump-confirmed special case, not
//  a guess) - each pair recomputes both sides' own state and sends each
//  side a one-shot notification (tags 0x3E/0x3F) about the other's id.
//  Any change to the percentage itself is broadcast separately (tag 0x40,
//  sub_6F66A8E0) to record->field0xD4's own head node only, through a
//  fresh single-element resizable word array this handler builds and
//  destroys for exactly that one send (sub_6F651EE0/sub_6F667FA0 - the
//  same SMemReAlloc-then-SMemAlloc-and-copy idiom
//  actionqueuerecord_resize.cpp's own ResizableBuffer::ResizeBuffer
//  already establishes, at word rather than byte granularity).
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);
void* __stdcall SMemReAlloc(void* ptr, unsigned int amount, const char* logfilename, int logline, unsigned int flags);
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

static const char aAudistfileprog[] = "AuDistFileProg";   // IDA's own display name for this type descriptor string; content not otherwise reached by this call tree.

//  0x6F666AE0 - already reconstructed for real (w3gsaction_parsers.cpp).
extern int __fastcall ParseDwordByteDwordAction_6F666AE0(void* src, unsigned int declaredLength, unsigned char* out);

//  0x6F651310 - already reconstructed for real (netmsgio_650-family).
extern CDataStoreScratch* __fastcall NetMsgWriteBytePair_6F651310(CDataStoreScratch* self, const unsigned char* pair);

//  0x6F6DAE20 - already reconstructed for real elsewhere in this tree;
//  re-declared identically here for extern linkage.
extern void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  Same ActionQueueTable/ConditionVariable infrastructure
//  w3gsaction_tier4.cpp and friends already establish.
struct ConditionVariable
{
    void SignalOne(unsigned int value);   // sub_6F6D8950
};
extern "C" unsigned int g_actionQueueSignal;   // unk_6FACFF20

struct ActionQueueTable
{
    void* FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                              unsigned int arg4, unsigned int createIfMissing);
};
extern ActionQueueTable g_actionQueueTable;   // stru_6FACFF00

static void g_actionQueueTable_Signal(int wasCreated)
{
    ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)(wasCreated != 0));
}

//  Redeclared exactly as netclient_notifyevent_0x1f.cpp and friends declare
//  it - the real constructor/destructor bodies live in cdatastorecache.cpp.
class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

//  A masked field is only ever a positive object pointer or a poisoned
//  non-positive encoding - the same idiom netclient_recvappend.cpp's own
//  MaskedPositive documents, redeclared locally per this file's own copy.
static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  ParseDwordByteDwordAction_6F666AE0's own output shape (NetMsgReadDwordByteDword_6F684510's
//  own comment: "dword, then byte immediately after it (+4), then unaligned dword at +5").
struct DwordByteDword
{
    unsigned int  id;      // +0x0 - the progress record's own key
    unsigned char type;    // +0x4 - must be < 6
    unsigned int  value;   // +0x5 - the new progress value
};

//----------------------------------------------------------------------------
//  0x6F66FF20 - a genuine pointer-chain search: record+0x1FC's own list
//  (next@+4, key@+8), unlike the array-indexed ListWalker every other list
//  in this tree uses.
//----------------------------------------------------------------------------
void* __fastcall FindProgressRecordById(void* record, unsigned int id)
{
    void* node = MaskedPositive(*(int*)((char*)record + 0x1FC));
    while (node)
    {
        if (*(unsigned int*)((char*)node + 8) == id)
            return node;
        node = MaskedPositive(*(int*)((char*)node + 4));
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F651EE0 / 0x6F667FA0 - a single-element resizable word array, the
//  same SMemReAlloc-then-SMemAlloc-and-copy shape
//  actionqueuerecord_resize.cpp's own ResizableBuffer::ResizeBuffer already
//  establishes at byte granularity, here at word (2-byte) granularity. The
//  four-field local it operates on - {tag, count, oldCount, ptr} - is built
//  and torn down entirely by its one caller below; `arr` here always points
//  at the `count` field (arr[-1] is the caller's own tag word, read only by
//  the caller, never by these two).
//----------------------------------------------------------------------------
void __fastcall ResizeWordArray(void* arr, unsigned int newCount)
{
    char* s = (char*)arr;
    unsigned short* oldPtr = *(unsigned short**)(s + 8);
    *(unsigned int*)s = newCount;

    unsigned int amount = newCount * 2;
    unsigned short* newPtr = (unsigned short*)SMemReAlloc(oldPtr, amount, aAudistfileprog, -2, 0x10);
    *(unsigned short**)(s + 8) = newPtr;
    if (newPtr != 0)
        return;

    newPtr = (unsigned short*)SMemAlloc(amount, aAudistfileprog, -2, (unsigned int)newPtr);
    *(unsigned short**)(s + 8) = newPtr;
    if (oldPtr == 0)
        return;

    unsigned int oldCapacity = *(unsigned int*)(s + 4);
    unsigned int copyLen = newCount;
    if (copyLen >= oldCapacity)
        copyLen = oldCapacity;

    for (unsigned int i = 0; i < copyLen; ++i)
    {
        unsigned short* dst = *(unsigned short**)(s + 8) + i;
        if (dst != 0)
            *dst = oldPtr[i];
    }

    SMemFree(oldPtr, aAudistfileprog, -2, 0);
}

void __fastcall FreeWordArrayBuffer(void* tagAndArr)
{
    void* ptr = *(void**)((char*)tagAndArr + 0xC);
    if (ptr)
        SMemFree(ptr, aAudistfileprog, -2, 0);
}

//----------------------------------------------------------------------------
//  0x6F651D40 - fastcall(this=writer, edx=&{count,oldCount,ptr}). Writes
//  `oldCount` (the real element count the caller sets right after
//  resizing - self+4 relative to `arr`) as a length-prefix byte, then that
//  many raw 2-byte pairs from `ptr` (self+8) via NetMsgWriteBytePair_6F651310.
//----------------------------------------------------------------------------
void __fastcall WritePercentPairs(CDataStoreScratch* writer, unsigned int* arr)
{
    writer->WriteByte((unsigned char)arr[1]);
    unsigned short* pairs = *(unsigned short**)&arr[2];
    unsigned int count = arr[1];
    for (unsigned int i = 0; i < count; ++i)
        NetMsgWriteBytePair_6F651310(writer, (const unsigned char*)&pairs[i]);
}

//----------------------------------------------------------------------------
//  0x6F684F60 - fastcall(this=writer, edx=&scratch). WriteDword the tag
//  (scratch[0]), then WritePercentPairs the {count,oldCount,ptr} tail
//  (scratch+4, i.e. &scratch[1]).
//----------------------------------------------------------------------------
void __fastcall WritePercentPayload(CDataStoreScratch* writer, unsigned int* scratch)
{
    writer->WriteDword(scratch[0]);
    WritePercentPairs(writer, &scratch[1]);
}

//  0x6F66A8E0 - fastcall(this=target, edx=&scratch). Frames {0xF7,0x40,
//  payload} through a local CDataStoreCache1460 the same way every other
//  member of this tag family does.
int __fastcall SendPercentNotify(void* target, unsigned int* scratch)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x40);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WritePercentPayload(writer, scratch);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//----------------------------------------------------------------------------
//  0x6F684180 / 0x6F66A660 - tag 0x3E: {dword,byte,dword} payload.
//----------------------------------------------------------------------------
struct PairNotifyDBD
{
    unsigned int  a;
    unsigned char b;
    unsigned int  c;
};

void __fastcall WritePairNotifyDBD(CDataStoreScratch* writer, PairNotifyDBD* msg)
{
    writer->WriteDword(msg->a);
    writer->WriteByte(msg->b);
    writer->WriteDword(msg->c);
}

int __fastcall SendPairNotify_0x3E(void* target, PairNotifyDBD* msg)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x3E);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WritePairNotifyDBD(writer, msg);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//----------------------------------------------------------------------------
//  0x6F6841E0 / 0x6F66A760 - tag 0x3F: {dword,byte} payload. 0x6F6841E0 is
//  already reconstructed for real as NetMsgWriteDwordByte_6F6841E0
//  (netmsgio_684.cpp) - the exact same shape, reused rather than
//  redeclared.
//----------------------------------------------------------------------------
struct PairNotifyDB
{
    unsigned int  a;
    unsigned char b;
};

extern CDataStoreScratch* __fastcall NetMsgWriteDwordByte_6F6841E0(CDataStoreScratch* self, const unsigned char* src);

int __fastcall SendPairNotify_0x3F(void* target, PairNotifyDB* msg)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x3F);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    NetMsgWriteDwordByte_6F6841E0(writer, (const unsigned char*)msg);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//----------------------------------------------------------------------------
//  0x6F66CA30 - the matchmaking pass. See this file's header comment for
//  the algorithm; `record` is the ActionQueueTable record (its own +0xCC
//  the node list, +0xD4 the list head), `progressRecord` the same one
//  FindProgressRecordById found, whose own +0x128+id*0xC table this
//  function both reads and mutates.
//----------------------------------------------------------------------------
void __fastcall AdvancePairing(void* record, void* progressRecord)
{
    void* completedGroup[16];
    void* pendingGroup[16];
    int completedCount = 0;
    int pendingCount = 0;

    //  The same array-indexed walk ListWalker::Step's own body performs,
    //  fully inlined here (this function shares a translation unit with
    //  the real body in the shipped build - see w3gsaction_tier27.cpp's
    //  own header comment for the same situation with BroadcastPlayerRecord).
    void* walkerBase = (char*)record + 0xCC;
    int node = *(int*)((char*)record + 0xD4);
    node = (node > 0) ? node : 0;
    while (node > 0)
    {
        unsigned char id = *(unsigned char*)((char*)node + 0xB4);
        char* entry = (char*)progressRecord + id * 0xC + 0x128;
        if (*(int*)entry == 1)
        {
            unsigned int cap = *(unsigned int*)((char*)progressRecord + 0x110);
            if (*(unsigned int*)(entry + 4) == cap)
                completedGroup[completedCount++] = (void*)node;
            else
                pendingGroup[pendingCount++] = (void*)node;
        }

        node = *(int*)(*(char**)walkerBase + node + 4);
    }

    for (int i = 0; i < completedCount; ++i)
    {
        void* completedNode = completedGroup[i];
        unsigned char completedId = *(unsigned char*)((char*)completedNode + 0xB4);
        unsigned int completedMask = 1u << (completedId - 1);

        if (pendingCount <= 0)
            continue;

        int j = 0;
        for (;;)
        {
            //  A completed node that is exactly record->field0xD4's own
            //  (masked) head bypasses the mutual-consent bitmask check
            //  entirely - a real, dump-confirmed special case.
            void* headMasked = MaskedPositive(*(int*)((char*)record + 0xD4));
            bool matched;
            unsigned char pendingId = 0;

            if (completedNode == headMasked)
            {
                matched = true;
                pendingId = *(unsigned char*)((char*)pendingGroup[j] + 0xB4);
            }
            else
            {
                void* pendingNode = pendingGroup[j];
                pendingId = *(unsigned char*)((char*)pendingNode + 0xB4);
                unsigned int pendingMask = 1u << (pendingId - 1);

                matched = (*(unsigned int*)((char*)pendingNode + 0x48) & completedMask) != 0
                       && (*(unsigned int*)((char*)completedNode + 0x48) & pendingMask) == 0;
            }

            if (!matched)
            {
                j += 1;
                if (j >= pendingCount)
                    break;
                continue;
            }

            void* pendingNode = pendingGroup[j];
            pendingGroup[j] = pendingGroup[--pendingCount];

            char* completedEntry = (char*)progressRecord + completedId * 0xC + 0x128;
            char* pendingEntry   = (char*)progressRecord + pendingId   * 0xC + 0x128;
            *(unsigned int*)completedEntry = 2;
            *(unsigned int*)pendingEntry   = 4;

            PairNotifyDBD msg1;
            msg1.a = *(unsigned int*)((char*)progressRecord + 8);
            msg1.b = pendingId;
            msg1.c = *(unsigned int*)(pendingEntry + 4);
            SendPairNotify_0x3E(*(void**)((char*)completedNode + 0x3C), &msg1);

            PairNotifyDB msg2;
            msg2.a = *(unsigned int*)((char*)progressRecord + 8);
            msg2.b = completedId;
            SendPairNotify_0x3F(*(void**)((char*)pendingNode + 0x3C), &msg2);

            break;
        }
    }
}

//  0x6F66FF90 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction_6F66FF90(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    DwordByteDword fields;
    if (!ParseDwordByteDwordAction_6F666AE0((void*)arg4, arg8, (unsigned char*)&fields))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    void* record = g_actionQueueTable.FindOrCreateRecord(key, 1, &wasCreated, 0, 1);

    //  esi starts at 1 in the shipped body (reused from an earlier
    //  argument-setup register) and is never reset on this particular
    //  early-out, so "record not found" reports status 1 here - unlike
    //  every sibling handler in this family, which reports 0. Confirmed
    //  against the dump, not assumed from the family's usual shape.
    int status = 1;
    if (record)
    {
        unsigned int selfFlags = *(unsigned int*)((char*)self + 0x80);
        if (selfFlags & 0x400000)
        {
            status = 2;
        }
        else if (!(selfFlags & 1))
        {
            status = 2;

            void* progressRecord = FindProgressRecordById(record, fields.id);
            if (progressRecord)
            {
                unsigned char clientId = *(unsigned char*)((char*)self + 0xB4);
                char* entry = (char*)progressRecord + clientId * 0xC + 0x128;
                unsigned int cap = *(unsigned int*)((char*)progressRecord + 0x110);

                if (fields.value >= *(unsigned int*)(entry + 4) &&
                    fields.value <= cap &&
                    fields.type < 6)
                {
                    *(unsigned int*)(entry + 0) = fields.type;
                    *(unsigned int*)(entry + 4) = fields.value;

                    int changed = 0;
                    if (cap != 0)
                    {
                        int state = *(int*)(entry + 0);
                        unsigned char percent;
                        if (state == 0)
                            percent = 0xFF;
                        else if (state == 5)
                            percent = 0xFE;
                        else
                            percent = (unsigned char)((fields.value * 100u) / cap);

                        if (percent != *(unsigned char*)(entry + 8))
                        {
                            *(unsigned char*)(entry + 8) = percent;
                            changed = 1;
                        }
                    }

                    if (*(int*)(entry + 0) == 1)
                        AdvancePairing(record, progressRecord);

                    if (changed)
                    {
                        int headNode = *(int*)((char*)record + 0xD4);
                        if (headNode > 0 && *(void**)((char*)headNode + 0x3C) != 0)
                        {
                            unsigned int scratch[4] = { 0, 0, 0, 0 };
                            ResizeWordArray(&scratch[1], 1);
                            scratch[2] = 1;
                            unsigned char* buf = *(unsigned char**)&scratch[3];
                            buf[0] = clientId;
                            buf[1] = *(unsigned char*)(entry + 8);
                            scratch[0] = *(unsigned int*)((char*)progressRecord + 8);

                            void* target = *(void**)((char*)headNode + 0x3C);
                            SendPercentNotify(target, scratch);

                            FreeWordArrayBuffer(&scratch[0]);
                        }
                    }
                }
            }
        }

        if (wasCreated != -1)
            g_actionQueueTable_Signal(wasCreated);
    }
    return status;
}
