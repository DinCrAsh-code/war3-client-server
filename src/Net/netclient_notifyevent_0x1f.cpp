//============================================================================
//  0x6F67A4D0 - Net::NetClient vtable slot 12, and its own callee
//  0x6F677D70 (networking wave 5, agent-networking-wave3-2026-08-29's
//  claim, continued).
//
//  One member of a family of seven near-identical "post a connection-
//  lifecycle notification frame" slots this wave found sitting
//  consecutively in NetClient's own vtable - slots 9/11/12/13/14/15/16
//  (0x6F67A370/0x6F67A410/0x6F67A4D0/0x6F67A560/0x6F67A5F0/0x6F67A6D0/
//  0x6F67A770). Each one's own call graph is identical in shape: the
//  record-table walk AppendReceivedData already established
//  (netclient_recvappend.cpp), a state check, then one 0x6F677Axx-
//  neighbourhood helper (0x677A70/677C70/677D70/677E70/677F70/678070/
//  678170, stepping by exactly 0x100 - a real address family, not
//  coincidence) that frames a fixed {0xF7, tag} header through a local
//  CDataStoreCache1460 write buffer, calls one more address that differs
//  per member, patches the length in, and dispatches through the
//  provider's own vtable slot 11 (DispatchVtableSlot11,
//  cdatastorescratch_writers.cpp - already reconstructed, reused verbatim
//  here as it was for BuildRelay0/1's own identical framing shape in
//  tock_relay_builders.cpp).
//
//  This session traced and reconstructed only this one member (tag 0x1F)
//  end to end, including its own "differs per member" callee
//  (0x6F683BF0, confirmed a genuine no-op: `mov eax,ecx / retn`, ignoring
//  both arguments). The other six were **not** individually confirmed to
//  be no-ops too - their own differing addresses (0x6F6843A0/0x6F685050/
//  0x6F684320/0x6F684360/0x6F683C30/0x6F683C10) were not read this
//  session, so they may write real payload bytes rather than doing
//  nothing. See docs/targets/NetRouter_NetPlayer_NetClient_oneslot_sweep.md's
//  wave 5 section for the full family table and addresses - a fast,
//  mechanical follow-on for whoever picks this back up next, now that the
//  shared shape and every other callee in it are established.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

//  netclient_recvappend.cpp - the same global guard and list head
//  AppendReceivedData's own record-table walk uses.
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

//  cdatastorescratch_writers.cpp - dispatch the framed bytes through the
//  provider's own vtable slot 11.
void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  Redeclared at file scope exactly as tock_relay_builders.cpp declares it -
//  the real constructor/destructor bodies live in cdatastorecache.cpp, and
//  both TUs' identical declarations bind to that one definition through the
//  linker, the same cross-TU shape netclientgame0_deleteselfadjustor.cpp's
//  own adjustor forward uses for a member function instead of a class.
class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

//  A masked field is only ever a positive object pointer or a poisoned
//  non-positive encoding - the same "masked Tail()" idiom
//  netclient_recvappend.cpp's own MaskedPositive documents, redeclared
//  locally here (that file's copy is not exported through a header) rather
//  than guessed at as a different shape.
static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  Field layout matches NetClientRecordFields in netclient_recvappend.cpp
//  exactly through +0x148 (m_nextRecord/m_recordKey/m_state/m_provider);
//  this member additionally reads a flag at +0x244 that call tree does
//  not otherwise touch. Redeclared locally with only the fields this file
//  reads named, per CLAUDE.md's own rule against inventing a shared header
//  from two independent, not-yet-reconciled partial views of one struct.
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

//  0x6F683BF0 - `mov eax,ecx / retn`. Called with ecx = the local
//  CDataStoreCache1460 write buffer and edx = the caller's own
//  pass-through pointer (see below); genuinely does nothing with either
//  and its own return value is never read by its one caller. Modelled as
//  a real, if pointless, function rather than skipped, so the call site
//  it sits at still matches instruction for instruction.
void* __fastcall NetClientNotify0x1F_NoOpCallback(void* buffer, void* passthrough)
{
    return buffer;
}

//  0x6F677D70 - see the family note above. `passthrough` is the address
//  of an uninitialized local byte the caller (below) owns; this function
//  never reads or writes through it, only forwards it into the no-op
//  callback above, so its real purpose is not established here.
int __fastcall NetClientNotifyEvent_0x1F(void* target, void* passthrough)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x1F);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    NetClientNotify0x1F_NoOpCallback(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F67A4D0 - the vtable slot itself: `this->field4` is the same
//  connection-record key AppendReceivedData reads at the same offset.
//  Not found, or found but outside state [4,5) or with +0x244 clear:
//  no event posted, but the global guard is still signalled either way -
//  two distinct call sites in the shipped code (an early-return exit and
//  a shared fall-through tail), reproduced here the same way
//  netclient_recvappend.cpp's own early-return shape is.
struct NetClientNotifySelf
{
    void PostEvent_0x1F();
};

void NetClientNotifySelf::PostEvent_0x1F()
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

    if (rec->m_state >= 4 && rec->m_state < 5 && rec->m_flag244 != 0)
    {
        void* provider = rec->m_provider;
        void* target = *(void**)((char*)provider + 0x3C);
        //  The shipped code passes the address of an uninitialized stack
        //  byte through here - never dereferenced anywhere in this call
        //  tree (see NetClientNotifyEvent_0x1F's own header), so this
        //  local exists only to have its address taken.
        unsigned char passthrough;
        NetClientNotifyEvent_0x1F(target, &passthrough);
    }

    g_netClientRecordTableGuard.SignalOne(0);
}
