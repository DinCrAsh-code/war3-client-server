//============================================================================
//  0x6F67A560 - Net::NetClient vtable slot 13, and its own callee
//  0x6F677E70 (networking wave 6, agent-networking-wave3-2026-08-29's
//  claim).
//
//  Third member of the seven-function notification family (see
//  netclient_notifyevent_0x1f.cpp's own header). Like slot 9's own final
//  callee this one genuinely writes a dword payload
//  (0x6F684320 - the same `WriteDword(*edx)` shape as 0x6F6843A0), but
//  the slot itself takes no stack argument: the shipped code zero-
//  initialises a local dword and passes *its* address, so the payload is
//  always a literal 0, not a caller-supplied value. Reproduced as a
//  local rather than a hardcoded WriteDword(0) call so the call-site
//  shape (address-of-local into the helper) still matches.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

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

//  0x6F684320 - identical shape to 0x6F6843A0 (see
//  netclient_notifyevent_0x24.cpp): write the dword `*passthrough`.
static CDataStoreScratch* __fastcall WriteDwordFromPtr_0x20(CDataStoreScratch* writer, void* passthrough)
{
    writer->WriteDword(*(unsigned int*)passthrough);
    return writer;
}

//  0x6F677E70 - {0xF7, 0x20} header, length placeholder, the (always-
//  zero) dword payload, patch the length in, dispatch through slot 11.
int __fastcall NetClientNotifyEvent_0x20(void* target, void* passthrough)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x20);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WriteDwordFromPtr_0x20(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F67A560 - the vtable slot itself.  Same [4,6)/flag244 condition as
//  slot 9; no stack argument at all (retn 0), unlike slot 9's retn 4.
struct NetClientNotifySelf
{
    void PostEvent_0x20();
};

void NetClientNotifySelf::PostEvent_0x20()
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
        void* target = *(void**)((char*)rec->m_provider + 0x3C);
        unsigned int zero = 0;
        NetClientNotifyEvent_0x20(target, &zero);
    }

    g_netClientRecordTableGuard.SignalOne(0);
}
