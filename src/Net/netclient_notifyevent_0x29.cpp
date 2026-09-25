//============================================================================
//  0x6F67A6D0 - Net::NetClient vtable slot 15, and its own callee
//  0x6F678070 (networking wave 6, agent-networking-wave3-2026-08-29's
//  claim).
//
//  Fourth member reconstructed of the seven-function notification family
//  (see netclient_notifyevent_0x1f.cpp's own header). This member's own
//  final callee (0x6F683C30) is a genuine no-op - `mov eax,ecx / retn`,
//  the same shape slot 12's own 0x6F683BF0 already confirmed - but the
//  gating condition is not the shared [4,6)/flag244 test every other
//  member so far uses: it checks `m_state == 8` exactly, plus a
//  provider-owned send counter (`provider->m_sentCount < rec->m_sendCap`,
//  incrementing the counter on a pass), and does not test flag244 at
//  all. Modelled from this member's own listing rather than assumed
//  shared with the rest of the family.
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

//  This member additionally reads m_sendCap at +0x240 (checked against
//  the provider's own +0x84 counter below); m_state is not range-tested
//  here, so it stays a plain int rather than a member with a shared name.
struct NetClientNotifyRecord
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x130 - 0x10];
    int  m_state;              // +0x130
    char m_reserved134[0x148 - 0x134];
    void* m_provider;          // +0x148
    char m_reserved14C[0x240 - 0x14C];
    unsigned int m_sendCap;    // +0x240 - unsigned: the shipped compare is
                               // `jae`, not `jge`
    char m_reserved244[4];
};

//  The provider object's own send counter, read/incremented at a fixed
//  offset in place - only this one field of the provider is touched
//  here, so nothing else about its layout is asserted.
struct NetProviderSendCounter
{
    char m_reserved00[0x84];
    unsigned int m_sentCount;   // +0x84 - unsigned, see m_sendCap above
};

//  0x6F683C30 - `mov eax,ecx / retn`: genuinely does nothing with either
//  argument, same as slot 12's own NoOpCallback.
void* __fastcall NetClientNotify0x29_NoOpCallback(void* buffer, void* passthrough)
{
    return buffer;
}

//  0x6F678070 - {0xF7, 0x29} header, length placeholder, no-op payload,
//  patch the length in, dispatch through slot 11.
int __fastcall NetClientNotifyEvent_0x29(void* target, void* passthrough)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x29);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    NetClientNotify0x29_NoOpCallback(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F67A6D0 - the vtable slot itself.  `state == 8` exactly, and a
//  provider-owned send counter gates the event separately from flag244.
struct NetClientNotifySelf
{
    void PostEvent_0x29();
};

void NetClientNotifySelf::PostEvent_0x29()
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

    if (rec->m_state == 8)
    {
        NetProviderSendCounter* provider = (NetProviderSendCounter*)rec->m_provider;
        if (provider->m_sentCount < rec->m_sendCap)
        {
            provider->m_sentCount += 1;
            void* target = *(void**)((char*)rec->m_provider + 0x3C);
            unsigned char passthrough;
            NetClientNotifyEvent_0x29(target, &passthrough);
        }
    }

    g_netClientRecordTableGuard.SignalOne(0);
}
