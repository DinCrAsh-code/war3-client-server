//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F680740. FOUR stack arguments, not the
//  three every sibling in this switch takes - w3gsaction_thunks.cpp's own
//  header already documented this from the shipped `retn 10h` and the
//  dispatcher's own four-push call site, which is why the thunk it
//  replaces was already declared with the right arity. What that header
//  could not see: the shipped body's very first instruction (`push ecx`)
//  spills a FIFTH, genuinely undeclared argument arriving in ecx - the
//  same "hidden receiver" hazard w3gsaction_tier11.cpp's own header
//  documents for 0x6F67AA80, captured here the same way (an inline `mov`
//  at entry, before anything else can touch ecx) and DIFFERS rather than
//  IDENTICAL for the same reason. The field validator also reuses one of
//  the caller's own stack argument slots as its output buffer (`&arg4`,
//  not a fresh local) - the shape w3gsaction_6f667c10.cpp's own header
//  already documents elsewhere in this dispatcher family - and `self`
//  here is a pointer TO the real receiver (`*(void**)self`), not the
//  receiver itself, which every other entry in this batch takes directly.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"
#include "playertable.h"

CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C50(CDataStoreScratch* self, const void*);    // netmsgio_683_empty.cpp
void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);                  // netclient_notifyevent_0x23.cpp

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

struct GameRecord15
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x114 - 0x5C];
    char* m_field114;           // +0x114 (inner array base)
    char m_reserved118[4];
    int m_field11C;              // +0x11C (inner list head; the walk's
                                   // own "current index is 0" branch
                                   // computes `&this+0x118` and reads its
                                   // own +4 - i.e. this SAME field again -
                                   // which is unreachable in practice
                                   // (index 0 only arises by comparing
                                   // against a non-null `self`) but kept
                                   // as its own literal branch below for
                                   // instruction-level fidelity)
    char m_reserved120[0x130 - 0x120];
    int m_state;                 // +0x130
    char m_reserved134[0x260 - 0x134];
    unsigned char m_field260;    // +0x260
};

//  0x6F6759D0 - fastcall(ecx=src, edx=declaredLength, stack out), retn 4.
//  Same family as w3gsaction_tier15.cpp's own FieldValidator_6F6755B0 -
//  a real `CDataStore` local (vtable written explicitly, not skipped),
//  one field reader (NetMsgReadByte_6F684050), success iff m_readPos ==
//  m_field10; the unified SEH/`/GS`-cookie frame docs/msvc-vc8-idioms.md
//  documents is unreproducible with this build's fixed `/GS-`.
CDataStore* __fastcall NetMsgReadByte_6F684050(CDataStore* self, unsigned char* dst);   // netmsgio_684.cpp
int __fastcall FieldValidator_6F6759D0(void* src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadByte_6F684050(&local, (unsigned char*)out);
    return local.m_readPos == local.m_field10;
}

//  0x6F6506A0 - already reconstructed for real (not a redirect) in
//  netprovider_keypattern.cpp: FixedPatternKey::Matches().
struct FixedPatternKey
{
    int Matches();
};

//  0x6F67E280 - fastcall(record, selfPtr) + one stack arg (argC), retn 4.
//  Out of scope for this batch (not one of its own worklist addresses).
extern "C" __declspec(naked) int __fastcall CheckAndAdvance_6F67E280(void*, void*, unsigned int)
{
    __asm { mov eax, 0x6F67E280 }
    __asm { jmp eax }
}

//  0x6F67BC80 - fastcall(record, subobj58), plain `retn`. Out of scope.
extern "C" __declspec(naked) void __fastcall RefreshDeadline_6F67BC80(void*, void*)
{
    __asm { mov eax, 0x6F67BC80 }
    __asm { jmp eax }
}

//  0x6F679560 - fastcall(record), plain `retn`. Out of scope.
extern "C" __declspec(naked) void __fastcall MaybeAdvanceState_6F679560(void*)
{
    __asm { mov eax, 0x6F679560 }
    __asm { jmp eax }
}

void __fastcall PlayerTable_InstallHandler(PlayerTable*, NetProviderMethod0x90Self*, unsigned char);   // playertable_installhandler.cpp

//  0x6F6806E0 - fastcall(record, subobj58=record->own +0x58 sub-object)
//  plus THREE stack arguments (selfPtr, argC, parsedField), retn 0Ch.
//  Real body: CheckAndAdvance_6F67E280(record, selfPtr, argC) - a 0
//  return reports status 2 up front - then unconditionally
//  RefreshDeadline_6F67BC80(record, subobj58); if the low byte of
//  `parsedField` disagrees with `record->0x148->0xB4`,
//  MaybeAdvanceState_6F679560(record); either way finishes with
//  PlayerTable_InstallHandler(record, subobj58, parsedField).
int __fastcall Dispatch_6F6806E0(void* record, void* subobj58, void* selfPtr, unsigned int argC, unsigned int parsedField)
{
    int result = 0;
    if (CheckAndAdvance_6F67E280(record, selfPtr, argC) == 0)
        result = 2;

    RefreshDeadline_6F67BC80(record, subobj58);

    unsigned char* field148 = *(unsigned char**)((char*)record + 0x148);
    if ((unsigned char)parsedField != field148[0xB4])
        MaybeAdvanceState_6F679560(record);

    PlayerTable_InstallHandler((PlayerTable*)record, (NetProviderMethod0x90Self*)subobj58, (unsigned char)parsedField);
    return result;
}

//  0x6F678970 - fastcall(this=the hidden ecx receiver, edx=scratch out-
//  pointer), plain `retn`. Real body: same CDataStoreCache1460-on-stack
//  write shape as w3gsaction_tier11.cpp's own HiddenReceiverMethod and
//  netclient_notifyevent_0x23.cpp's own NetClientNotifyEvent_0x23 - a
//  {0xF7, 0x2A} header, a length placeholder, a no-op payload
//  (NetMsgIoNoFields_6F683C50), the length patched back in, then
//  dispatched through vtable slot 11 and the buffer's own length
//  returned - the shipped body's own final `mov eax, esi` reloads the
//  length, not the buffer base, matching NetClientNotifyEvent_0x23's own
//  return value rather than returning `void`.
int __fastcall Cleanup_6F678970(void* target, unsigned int* passthrough)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x2A);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    NetMsgIoNoFields_6F683C50(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F680740 - case (see w3gsactionhandler2.cpp). On a record match
//  whose own state >= 7 and own +0x260 == 0: unless self's own +0x80 bit
//  0x100000 is set, search the record's own +0x114/+0x118/+0x11C inner
//  list (the ListWalker idiom w3gsaction_tier10.cpp's own header
//  documents, here walked by hand since its own "current index 0" case
//  reads a distinct +0x118 virtual head rather than delegating to
//  ListWalker::Step) for a node that is either `self` itself or whose own
//  +0x90 FixedPatternKey matches, stamping a found node's own +0x44 from
//  1 to 7. Either way, dispatches through Dispatch_6F6806E0 with the
//  validator's own parsed dword and the caller's own argC, and - only
//  when that call does not itself report 2 - runs the not-in-scope
//  Cleanup_6F678970 through the hidden ecx receiver.
int __stdcall W3GSAction2_6F680740(void* self, unsigned int arg4, unsigned int arg8, unsigned int argC)
{
    void* hiddenEcx;
    __asm { mov hiddenEcx, ecx }

    if (!FieldValidator_6F6759D0((void*)arg4, arg8, &arg4))
        return 1;

    void* realSelf = *(void**)self;
    void* key = *(void**)((char*)realSelf + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord15* rec = (GameRecord15*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord15*)rec->m_nextRecord;

        if (rec && rec->m_state >= 7 && rec->m_field260 == 0)
        {
            void* field58 = rec->m_field58;

            if (!(*(unsigned int*)((char*)realSelf + 0x80) & 0x100000))
            {
                int index = (int)MaskedPositive(rec->m_field11C);
                while (index > 0)
                {
                    if ((void*)index != realSelf)
                    {
                        if (((FixedPatternKey*)((char*)index + 0x90))->Matches())
                            goto foundMatch;
                    }
                    index = (index == 0)
                        ? rec->m_field11C
                        : *(int*)(rec->m_field114 + index + 4);
                }
                goto searchDone;

            foundMatch:
                if (*(unsigned int*)((char*)index + 0x44) == 1)
                    *(unsigned int*)((char*)index + 0x44) = 7;
            searchDone:;
            }

            status = Dispatch_6F6806E0(rec, field58, self, argC, arg4);
            if (status != 2)
                Cleanup_6F678970(hiddenEcx, &arg8);
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}
