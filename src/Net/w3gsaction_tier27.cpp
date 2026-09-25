//============================================================================
//  batch-H-w3gsaction-and-replaystream - 0x6F66E890. Same
//  ActionQueueTable::FindOrCreateRecord/self+0x80 flag/record-state family
//  w3gsaction_tier4.cpp and w3gsaction_tier26.cpp already establish, but
//  its own field reader is ParseRawDwordDwordAction_6F666770
//  (w3gsaction_parsers.cpp - a 16-byte raw run plus two trailing dwords),
//  one of the trailing dwords is bound-checked against 0x40 before the
//  record lookup even happens, and its own success path frames a
//  {0xF7, 0x16, <16 raw bytes>, dwordA, dwordB} payload through a local
//  CDataStoreCache1460/CDataStoreScratch write buffer - the same framing
//  shape netclient_notifyevent_0x1f.cpp's own family already establishes,
//  reused here rather than reinvented - and broadcasts it to every node on
//  record->field0xCC's own list via DispatchVtableSlot11, finishing with
//  the same Sub_6F66DB40 drain/teardown w3gsaction_tier4.cpp's own
//  W3GSAction_6F66E430 already thunks.
//
//  One real difference from the netclient_notifyevent_0x1f.cpp family:
//  that family's own list-walk goes through an explicit ListWalker::Step
//  call (w3gsaction_tier10.cpp/w3gsaction_tier26.cpp, kept in their own
//  translation units specifically so the compiler cannot inline it); this
//  handler's own callee (sub_6F66A340, below) shares a translation unit
//  with the real body in the shipped build and the walk comes out fully
//  inlined - reproduced here as the same raw two-field arithmetic
//  ListWalker::Step's own body performs (base = *(walkerBase+0), next =
//  *(base+node+4), starting from *(walkerBase+8)) rather than a call this
//  build cannot make disappear the same way.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int n);

//  0x6F666770 - already reconstructed for real (w3gsaction_parsers.cpp).
extern int __fastcall ParseRawDwordDwordAction_6F666770(void* src, unsigned int declaredLength, unsigned char* out);

//  0x6F66DB40 - already a thunk (w3gsaction_tier4.cpp); re-declared
//  identically here for extern linkage.
extern "C" void __fastcall Sub_6F66DB40(void*);

//  0x6F6DAE20 - already reconstructed for real elsewhere in this tree;
//  re-declared identically here for extern linkage.
extern void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  Same ActionQueueTable/ConditionVariable infrastructure
//  w3gsaction_tier4.cpp/w3gsaction_tier26.cpp already establish.
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

//  ParseRawDwordDwordAction_6F666770's own output shape (0x6F6843F0's own
//  comment: "a 0x10-byte raw run, then dword at +0x10, then unaligned
//  dword"). `sizeCheck` is bound-checked against 0x40 by this handler
//  before the record lookup; `tag` becomes self's own +0x4C field.
struct ParsedRecordFields
{
    unsigned char raw16[0x10];  // +0x00
    unsigned int  tag;          // +0x10 -> self+0x4C
    unsigned int  sizeCheck;    // +0x14 -> self+0x68, bound-checked <= 0x40
};

//  The broadcast payload sub_6F6840A0 (below) frames into the write
//  buffer: self's own id byte, the same 16 raw bytes, then the two parsed
//  fields in the *opposite* order to how they're stored into self (field
//  0x4C's own value first, then field 0x68's).
struct BroadcastPayload
{
    unsigned char id;           // +0x00 = self->field0xB4
    unsigned char raw16[0x10];  // +0x01
    unsigned int  tag;          // +0x11 = self->field0x4C (ParsedRecordFields::tag)
    unsigned int  sizeCheck;    // +0x15 = self->field0x68 (ParsedRecordFields::sizeCheck)
};

//  0x6F6840A0 - fastcall(this=writer, edx=&BroadcastPayload). Frames the
//  payload's own four pieces through the writer exactly as declared above.
void __fastcall WritePlayerRecordPayload(CDataStoreScratch* writer, BroadcastPayload* payload)
{
    writer->WriteByte(payload->id);
    writer->TailCallWriteRaw(payload->raw16, 0x10);
    writer->WriteDword(payload->tag);
    writer->WriteDword(payload->sizeCheck);
}

//  0x6F66A340 - fastcall(this=walkerBase (record+0xCC), edx=&payload).
//  Frames {0xF7, 0x16, payload} through a local CDataStoreCache1460 the
//  same way netclient_notifyevent_0x1f.cpp's own family does, then
//  broadcasts the framed bytes to every node on the walker's own list -
//  see this file's header comment for why the walk itself is inlined
//  rather than going through ListWalker::Step. Returns the sum of every
//  dispatched node's own buffer length (not a node count).
int __fastcall BroadcastPlayerRecord(void* walkerBase, BroadcastPayload* payload)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x16);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WritePlayerRecordPayload(writer, payload);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    int total = 0;
    int node = *(int*)((char*)walkerBase + 8);
    while (node > 0)
    {
        void* target = *(void**)((char*)node + 0x3C);
        if (target)
        {
            DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);
            total += bufferLen;
        }
        node = *(int*)(*(char**)walkerBase + node + 4);
    }

    return total;
}

//  0x6F66E890 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction_6F66E890(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    ParsedRecordFields parsed;
    if (!ParseRawDwordDwordAction_6F666770((void*)arg4, arg8, (unsigned char*)&parsed))
        return 1;

    if (parsed.sizeCheck > 0x40)
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    void* record = g_actionQueueTable.FindOrCreateRecord(key, 1, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        unsigned int selfFlags = *(unsigned int*)((char*)self + 0x80);
        if (selfFlags & 0x400000)
        {
            status = 2;
        }
        else if (*(int*)((char*)record + 0xE4) == 4)
        {
            if (selfFlags & 1)
            {
                status = 1;
            }
            else
            {
                BroadcastPayload payload;
                payload.id = *(unsigned char*)((char*)self + 0xB4);
                memcpy(payload.raw16, parsed.raw16, 0x10);
                payload.tag = parsed.tag;
                payload.sizeCheck = parsed.sizeCheck;

                *(unsigned int*)((char*)self + 0x80) = selfFlags | 1;
                *(unsigned int*)((char*)self + 0x68) = parsed.sizeCheck;
                *(unsigned int*)((char*)self + 0x4C) = parsed.tag;

                BroadcastPlayerRecord((char*)record + 0xCC, &payload);
                Sub_6F66DB40(record);
                status = 2;
            }
        }

        if (wasCreated != -1)
            g_actionQueueTable_Signal(wasCreated);
    }
    return status;
}
