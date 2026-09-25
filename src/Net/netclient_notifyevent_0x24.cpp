//============================================================================
//  0x6F67A370 - Net::NetClient vtable slot 9, and its own callee 0x6F677A70
//  (networking wave 6, agent-networking-wave3-2026-08-29's claim).
//
//  Second member of the seven-function "post a connection-lifecycle
//  notification frame" family wave 5 found (slots 9/11/12/13/14/15/16,
//  see netclient_notifyevent_0x1f.cpp's own header for the full family
//  table and the shared shape). This member is NOT the same fixed-payload
//  no-op slot 12 turned out to be: its own differing final callee
//  (0x6F6843A0) genuinely writes a caller-supplied dword into the frame,
//  so the slot itself takes one stack argument (`retn 4`) and threads it
//  through by address - the shipped code reads the incoming stack slot
//  into a register and writes it straight back to the same slot before
//  taking its address, which is exactly what a by-value parameter whose
//  address is then taken compiles to.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

//  netclient_recvappend.cpp - the same global guard and list head every
//  member of this family walks.
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  Redeclared at file scope exactly as netclient_notifyevent_0x1f.cpp
//  declares it - the real constructor/destructor bodies live in
//  cdatastorecache.cpp and bind through the linker.
class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  Same field layout as netclient_notifyevent_0x1f.cpp's own
//  NetClientNotifyRecord - redeclared locally per CLAUDE.md's rule
//  against inventing a shared header from independent partial views.
struct NetClientNotifyRecord
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x130 - 0x10];
    int  m_state;              // +0x130
    char m_reserved134[0x148 - 0x134];
    void* m_provider;          // +0x148
    char m_reserved14C[0x244 - 0x14C];
    int  m_flag244;            // +0x244
};

//  0x6F6843A0 - write the dword `*passthrough` into the frame.  Called
//  with ecx = the local CDataStoreCache1460 write buffer and edx = the
//  address of the (by-value) stack argument the slot itself received.
static CDataStoreScratch* __fastcall WriteDwordFromPtr_0x24(CDataStoreScratch* writer, void* passthrough)
{
    writer->WriteDword(*(unsigned int*)passthrough);
    return writer;
}

//  0x6F677A70 - see the family note above: {0xF7, 0x24} header, length
//  placeholder, the dword payload, patch the length in, dispatch through
//  vtable slot 11.
int __fastcall NetClientNotifyEvent_0x24(void* target, void* passthrough)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x24);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WriteDwordFromPtr_0x24(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F67A370 - the vtable slot itself.  Condition widens to state
//  [4,6) (slot 12's own was [4,5)) - a real per-member difference, not a
//  transcription slip; each family member's own state range and flag
//  gate is read off its own listing, not assumed shared.
struct NetClientNotifySelf
{
    void PostEvent_0x24(unsigned int value);
};

void NetClientNotifySelf::PostEvent_0x24(unsigned int value)
{
    void* self = this;
    void* key = *(void**)((char*)self + 4);
    if (!key)
        return;

    g_netClientRecordTableGuard.Wait(0);

    NetClientNotifyRecord* rec = (NetClientNotifyRecord*)MaskedPositive(g_netClientRecordListHead);
    while (rec && (void*)rec->m_recordKey != key)
        rec = (NetClientNotifyRecord*)MaskedPositive((int)rec->m_nextRecord);

    if (!rec)
    {
        g_netClientRecordTableGuard.SignalOne(0);
        return;
    }

    if (rec->m_state >= 4 && rec->m_state < 6 && rec->m_flag244 != 0)
    {
        void* provider = rec->m_provider;
        void* target = *(void**)((char*)provider + 0x3C);
        NetClientNotifyEvent_0x24(target, &value);
    }

    g_netClientRecordTableGuard.SignalOne(0);
}
