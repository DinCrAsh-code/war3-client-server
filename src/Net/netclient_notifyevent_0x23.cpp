//============================================================================
//  0x6F67A770 - Net::NetClient vtable slot 16, and its own callee
//  0x6F678170 (networking wave 6, agent-networking-wave3-2026-08-29's
//  claim).
//
//  Fifth member reconstructed of the seven-function notification family
//  (see netclient_notifyevent_0x1f.cpp's own header). Its own final
//  callee (0x6F683C10) is a genuine no-op, the same shape as slot 15's
//  0x6F683C30 and slot 12's 0x6F683BF0. The gating condition is a third
//  distinct shape again: `m_state == 6` exactly, plus a one-shot latch -
//  a bit in the provider's own flags dword that this slot both tests and
//  sets, so the event fires at most once per provider until whatever
//  clears that bit resets it (not read anywhere in this call tree).
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
};

//  The provider object's own flags dword at +0x80 - the same field
//  slot 0 (AppendReceivedData, netclient_recvappend.cpp) tests bit
//  0x400000 of, here tested/set at bit 0x10000 instead.
struct NetProviderFlags
{
    char m_reserved00[0x80];
    unsigned int m_flags;   // +0x80
};

//  0x6F683C10 - `mov eax,ecx / retn`: genuinely does nothing with either
//  argument, same as slots 12/15's own no-op callbacks.
void* __fastcall NetClientNotify0x23_NoOpCallback(void* buffer, void* passthrough)
{
    return buffer;
}

//  0x6F678170 - {0xF7, 0x23} header, length placeholder, no-op payload,
//  patch the length in, dispatch through slot 11.
int __fastcall NetClientNotifyEvent_0x23(void* target, void* passthrough)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x23);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    NetClientNotify0x23_NoOpCallback(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F67A770 - the vtable slot itself.  `state == 6` exactly, gated by
//  a one-shot latch bit (0x10000) in the provider's own flags dword.
struct NetClientNotifySelf
{
    void PostEvent_0x23();
};

void NetClientNotifySelf::PostEvent_0x23()
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

    if (rec->m_state == 6)
    {
        NetProviderFlags* provider = (NetProviderFlags*)rec->m_provider;
        if (!(provider->m_flags & 0x10000))
        {
            provider->m_flags |= 0x10000;
            void* target = *(void**)((char*)rec->m_provider + 0x3C);
            unsigned char passthrough;
            NetClientNotifyEvent_0x23(target, &passthrough);
        }
    }

    g_netClientRecordTableGuard.SignalOne(0);
}
