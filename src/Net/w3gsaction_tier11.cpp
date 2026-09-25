//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67AA80. Same genuinely-undeclared-
//  incoming-ecx hazard w3gsaction_6f667c10.cpp's own header already
//  documents for its own sibling: the shipped body's very first real
//  instruction (`mov esi, ecx`) captures a fourth argument this
//  dispatcher's own 3-stack-argument convention has no slot for, before
//  ecx gets reused for the field validator's own argument. Modelled the
//  same way: an inline `mov` capturing ecx literally, DIFFERS rather than
//  IDENTICAL for the same reason.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

CDataStore* __fastcall NetMsgReadDword_6F683DD0(CDataStore* self, unsigned int* dst);              // netmsgio_683.cpp
CDataStoreScratch* __fastcall NetMsgWriteDword_6F683DE0(CDataStoreScratch* self, const unsigned int* src);   // netmsgio_683.cpp
void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);                    // netclient_notifyevent_0x23.cpp

class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();
    char m_inlineBuffer[1460];
};

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

struct GameRecord4
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x178 - 0x10];
    unsigned int m_field178;   // +0x178
};

//  0x6F675030 - fastcall(ecx=src, edx=declaredLength, stack out), retn 4.
//  Same family as w3gsaction_tier15.cpp's own FieldValidator_6F6755B0 - a
//  real CDataStore local, one field reader (NetMsgReadDword_6F683DD0),
//  success iff m_readPos == m_field10; the unified SEH/`/GS`-cookie frame
//  docs/msvc-vc8-idioms.md documents is unreproducible with this build's
//  fixed `/GS-`.
int __fastcall FieldValidator_6F675030(void* src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F683DD0(&local, (unsigned int*)out);
    return local.m_readPos == local.m_field10;
}

//  0x6F678870 - genuine fastcall(this=ecx, edx=&scratch): the call site's
//  own `lea edx,[...]` loads the pointer straight into edx with no push at
//  all, so this is a real two-register-argument call, not a stack
//  argument (thunk_abi_audit.py caught the earlier `retn 4` guess - the
//  shipped body cleans 0 bytes, `retn 0`). Real body: same
//  CDataStoreCache1460-on-stack write shape as w3gsaction_tier22.cpp's own
//  Cleanup_6F678970 and netclient_notifyevent_0x23.cpp's own
//  NetClientNotifyEvent_0x23 - a {0xF7, 0x46} header, a length placeholder,
//  one dword payload write (NetMsgWriteDword_6F683DE0, the value taken
//  from `edx`, i.e. this function's own second argument), the length
//  patched back in, dispatched through vtable slot 11, the buffer's own
//  length returned.
int __fastcall HiddenReceiverMethod(void* target, unsigned int* value)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x46);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    NetMsgWriteDword_6F683DE0(writer, value);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

extern "C" unsigned int __stdcall GetTickCount(void);

//  0x6F67AA80 - case (see w3gsactionhandler2.cpp). Validate one field via
//  FieldValidator_6F675030, forward arg8 through the hidden receiver
//  captured off entry-ecx, then - on a record match - stamp the record's
//  own +0x178 with GetTickCount() and report status 2.
int __stdcall W3GSAction2_6F67AA80(void* self, unsigned int arg4, unsigned int arg8)
{
    void* hiddenEcx;
    __asm { mov hiddenEcx, ecx }

    if (!FieldValidator_6F675030((void*)arg4, arg8, &arg8))
        return 1;

    unsigned int scratch = arg8;
    HiddenReceiverMethod(hiddenEcx, &scratch);

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(0);

        GameRecord4* rec = (GameRecord4*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord4*)rec->m_nextRecord;

        if (rec)
        {
            rec->m_field178 = GetTickCount();
            status = 2;
        }

        g_netClientRecordTableGuard.SignalOne(0);
    }
    return (int)status;
}
