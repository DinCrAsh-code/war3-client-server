//============================================================================
//  Net::NetProvider - vtable slot 25, 0x6F6570A0.  The largest of this
//  session's slots: tell vtable+0x90 "code 3", zero a 16-byte scratch
//  block, forward through the object's own vtable+0x30, register through
//  NetProviderBase_Register (0x6F672A60, already declared in
//  netprovider_method0x90.cpp / thunked in src/Misc/unreconstructed_
//  thunks.cpp), report the outcome through the object's own vtable+0x34,
//  build a record through BuildRecord below (now real), and finally
//  forward the whole scratch block plus two of the caller's own arguments
//  through the inner +0x2B4 interface's vtable+0x8 slot.
//
//  sub_6F6712C0 (0x6F6712C0, `retn 4`, 60+ further callees of its own
//  reaching Storm imports, GetTickCount and the ConditionVariable/
//  WaitQueueRing family) is genuinely deep and not this session's own
//  target; stays a thunk to the real, unhooked body.
//============================================================================

//  NetProviderBase_Register's real definition lives in src/Misc/
//  unreconstructed_thunks.cpp - only declared here, matching that TU's own
//  __fastcall(self, edxArg, stackArg) signature exactly.
extern int __fastcall NetProviderBase_Register(void*, int, int);

//  0x6F650680 - zero the first four dwords, tolerating a null receiver.
//  Already reconstructed for real in src/Misc/trivialpredicates.cpp
//  (ClearFourDwords_6F650680) - reused rather than duplicated.
void __fastcall ClearFourDwords_6F650680(void* self);

//  Storm.dll ordinal 401/465 - already declared this shape elsewhere in
//  this directory (netproviderloop_bindport.cpp and friends for
//  SMemAlloc/SErrSetLastError respectively).
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);
void  __stdcall SErrSetLastError(unsigned int error);

//  Sync/critsec.cpp's own functions.
long __fastcall InterlockedIncrementAt(volatile long* target);            // 0x6F6C4060
unsigned __int64 __fastcall Interlocked64IncrementAt(volatile unsigned __int64*);  // 0x6F6C40C0

//  0x6F663CA0 - a monotonic "next id" generator; real body in
//  netrouter_ticksequence.cpp (own translation unit so this call site
//  stays a genuine out-of-line call, matching the shipped body).  Not
//  part of this batch's own worklist but needed as BuildRecord's own
//  callee.
int __fastcall AdvanceRouterTickCounter(int delta);

//  0x6F66CC30 - the WaitQueueRing family's own "assign this record a
//  ring-bounded id and enqueue it" member: idempotent (a record that
//  already has a nonzero id at its own +0xC is left alone and that id is
//  returned unchanged), otherwise generates the next id under the ring's
//  own CRITICAL_SECTION, retrying via its own ConditionVariable whenever
//  the freshly-generated id collides with one still live on a second,
//  masked-positive-walked list, then splices the record's own +4
//  TSLink-shaped sub-object onto the ring's own tail list and signals the
//  condition variable once more before releasing the lock.  Reached only
//  from BuildRecord below and genuinely deep on its own account (five
//  further callees: EnterCriticalSection/LeaveCriticalSection plus the
//  already-reconstructed ConditionVariable::Wait/SignalOne pair, all
//  confirmed via this session's own ida_query pull) - left a correctly-
//  ABI'd naked thunk rather than a second reconstruction inside this same
//  session's already-large budget.  `retn 4` - __fastcall(ecx=ring, stack
//  arg=record).
//  A free function cannot be declared `__thiscall` (only a native member
//  function may), so this is a member of a tiny placeholder type - the
//  same pattern w3gsaction_tier8.cpp's own FieldTarget already
//  establishes, and needed here for the same reason: a plain two-fastcall-
//  parameter free function would put `record` in `edx`, not on the stack.
class WaitQueueRing
{
public:
    unsigned int Enqueue(void* record);
};
__declspec(naked) unsigned int WaitQueueRing::Enqueue(void*)
{
    __asm { mov eax, 0x6F66CC30 }
    __asm { jmp eax }
}
extern "C" WaitQueueRing stru_6FACFF38;   // the global WaitQueueRing::Enqueue above operates on

//  Net::NetRouter::PreGame - the object BuildRecord below allocates and
//  fills.  Only the touched fields are named; +0x14 begins the 0xB8-byte
//  (46-dword) block `rep movsd` copies straight from the caller's own
//  `edxArg` pointer (bounds-checked at edxArg+0xB0 against 0x10 up front),
//  so its own internal layout is opaque here - genuinely a raw byte copy,
//  not a struct assignment this build's `rep movsd` idiom happens to
//  reproduce for (docs/msvc-vc8-idioms.md's own note on
//  netproviderltcp_notify.cpp's identical-size descriptor copy).
//  A plain 46-dword (0xB8-byte) POD block, used only to get this build's
//  own struct-assignment codegen to `rep movsd` - the same technique
//  netproviderltcp_notify.cpp's own DescriptorBlock documents.
struct DescriptorBlock46 { unsigned int dwords[0x2E]; };

struct PreGameRecord
{
    void* m_vtable;         // +0x00
    int   m_reserved04;      // +0x04
    int   m_reserved08;      // +0x08
    int   m_reserved0C;      // +0x0C - WaitQueueRing::Enqueue's own idempotency key
    void* m_owner;           // +0x10 - back-pointer to the NetProviderSlot25Self that built this
    char  m_copiedBlock[0xB8]; // +0x14 - raw copy of *edxArg, opaque
    int   m_field0CC;         // +0xCC - *scratch1C
    int   m_field0D0;         // +0xD0 - the id BuildRecord itself generated (*var18)
    int   m_field0D4;         // +0xD4 - field24
    int   m_field0D8;         // +0xD8 - field28
    int   m_field0DC;         // +0xDC - the caller's own "zero" argument, stored verbatim
};

//  0x6FACFEFC/0x6FACFEF8 - already declared exactly this shape in
//  actionqueuerecord_notify.cpp; a shared 64-bit counter array guarded by
//  the same "count <= bound" check every reader of it in this repo uses.
extern "C" long dword_6FACFEFC;
extern "C" long dword_6FACFEF8;

class NetProviderSlot25Self
{
public:
    //  0x6F670D50 - build/queue a record from this object's own +0x24/+0x28
    //  fields and the scratch block; `retn 1Ch` (7 stack args) plus `this`
    //  in ecx and a second register argument in edx.  Returns only its own
    //  low 16 bits meaningfully (both exits leave the high half of eax
    //  whatever it already held - a bounds-check failure clears just `ax`,
    //  and the normal exit overwrites just `ax` from a cached word global
    //  right before the `retn` - so this is `unsigned short`, not `int`,
    //  matching both tails' own instruction shape exactly).
    unsigned short __fastcall BuildRecord(void* edxArg, int* scratch1C, int field24,
                                          int field28, int zero, int arg8, int* var20,
                                          int* var18);

    //  0x6F6712C0 - `retn 4`, `this` in ecx, one stack argument.
    void __fastcall FinishRecord(int stackArg, int edxArg);
};
__declspec(naked) void __fastcall
NetProviderSlot25Self::FinishRecord(int, int)
{
    __asm { mov eax, 0x6F6712C0 }
    __asm { jmp eax }
}

//  0x6FACFE80 - already declared in netprovider_register.cpp as the
//  cached slot-15 registration result; BuildRecord's own normal exit
//  reports it back out through `ax` for reasons that file's own header
//  comment covers (an intentionally shared, possibly-uninitialised cache).
extern short g_netProviderRegisterSlot15Cache;   // word_6FACFE80

unsigned short __fastcall NetProviderSlot25Self::BuildRecord(
    void* edxArg, int* scratch1C, int field24, int field28, int zero,
    int arg8, int* var20, int* var18)
{
    if ((unsigned int)*(int*)((char*)edxArg + 0xB0) > 0x10)
    {
        SErrSetLastError(0x57);
        return 0;
    }

    int id18 = 0;
    if (arg8 != 0)
    {
        id18 = AdvanceRouterTickCounter(0);
        if (id18 == 0)
            id18 = 1;
    }
    *var18 = id18;

    //  0x6F9715C0 - Net::NetRouter::PreGame's own vtable (user_knowledge.json),
    //  stamped by address rather than through a from-scratch derived class,
    //  the same fallback netcommand_dispatch.cpp's own
    //  kNetEventSaveCompleteVftable establishes for a class this closure
    //  only ever constructs, never derives from.
    static const void* const kPreGameVftable = (const void*)0x6F9715C0;

    PreGameRecord* rec = (PreGameRecord*)SMemAlloc(0xE0, ".\\NetRouter.cpp", 0xA60, 8);
    if (rec != 0)
    {
        rec->m_reserved04 = 0;
        rec->m_reserved08 = 0;
        rec->m_reserved0C = 0;
        rec->m_vtable = (void*)kPreGameVftable;
        *((char*)rec + 0x14) = 0;
        *((char*)rec + 0x34) = 0;
        *((char*)rec + 0x44) = 0;
    }

    InterlockedIncrementAt((volatile long*)((char*)this + 0x10));

    //  Faithful to the shipped body: these stores run unconditionally, even
    //  on a failed allocation (`rec == 0`), which would fault exactly the
    //  way the real function does - not "fixed" with a null check the
    //  shipped code itself does not have.
    rec->m_owner = this;
    //  A plain struct assignment rather than `memcpy` - the same choice
    //  netproviderltcp_notify.cpp's own identical-size (0xB8-byte)
    //  descriptor copy documents, and what actually gets this build to
    //  the shipped body's own `rep movsd` rather than a real call.
    *(DescriptorBlock46*)rec->m_copiedBlock = *(const DescriptorBlock46*)edxArg;
    rec->m_field0CC = *scratch1C;
    rec->m_field0D0 = *var18;
    rec->m_field0D4 = field24;
    rec->m_field0D8 = field28;
    rec->m_field0DC = zero;

    if ((unsigned int)dword_6FACFEFC <= 1)
    {
        SErrSetLastError(0x57);
    }
    else
    {
        Interlocked64IncrementAt((volatile unsigned __int64*)((char*)&dword_6FACFEF8 + 8));
    }

    *var20 = (int)stru_6FACFF38.Enqueue(rec);

    return (unsigned short)g_netProviderRegisterSlot15Cache;
}

namespace Net
{

class NetProvider
{
public:
    //  0x6F6570A0 - vtable slot 25.  Bug fix, not a rewrite: the shipped
    //  body's own tail is `mov eax, 1` right before the SEH-cookie check
    //  (unconditional - every path through this function falls into the
    //  same tail), and its own caller (NetProviderBNET::Method_28,
    //  netproviderbnet_seh.cpp - a second, otherwise-unrelated caller of
    //  this exact address, reached while un-thunking that file's own
    //  ScratchForwarder::Forward) captures that return value with
    //  `mov edi, eax` and hands it onward as its own `int` result. The
    //  previous `void` declaration silently dropped a real return value
    //  every caller reads.
    int Method_25(void* arg0, void* arg4, int arg8);
};

int NetProvider::Method_25(void* arg0, void* arg4, int arg8)
{
    typedef void (__thiscall *Method0x90Fn)(void*, int, int, int);
    ((Method0x90Fn)(*(void***)this)[0x90 / 4])(this, 3, 0, 0);

    int scratch[4];
    ClearFourDwords_6F650680(&scratch);

    typedef int (__thiscall *InnerSlot0x30Fn)(void*, int*);
    InnerSlot0x30Fn slot0x30 = (InnerSlot0x30Fn)(*(void***)this)[0x30 / 4];
    int status = slot0x30(this, scratch);

    unsigned short cachedWord = *(unsigned short*)((char*)this + 0x2C0);
    int var20 = 0;
    int var18 = 0;
    NetProviderBase_Register(this, cachedWord, cachedWord + 1);

    if ((short)status != 0)
    {
        typedef void (__thiscall *InnerSlot0x34Fn)(void*, int*, int*, int);
        void* self = this;
        InnerSlot0x34Fn slot0x34 =
            (InnerSlot0x34Fn)(*(void***)self)[0x34 / 4];
        slot0x34(self, scratch, scratch, status);
    }

    int field24 = *(int*)((char*)this + 0x24);
    int field28 = *(int*)((char*)this + 0x28);
    int outcome = ((NetProviderSlot25Self*)this)->BuildRecord(
        arg0, &scratch[3], field24, field28, 0, arg8, &var20, &var18);

    if (var20 == 0)
        ((NetProviderSlot25Self*)this)->FinishRecord(1, 0);
    else
        *(int*)((char*)this + 0x2B8) = 1;

    void* inner = *(void**)((char*)this + 0x2B4);
    typedef void (__thiscall *InnerSlot0x8Fn)(void*, void*, int*, int, int,
                                              int*, void*, void*);
    InnerSlot0x8Fn slot0x8 = (InnerSlot0x8Fn)(*(void***)inner)[0x08 / 4];
    slot0x8(inner, this, scratch, var20, var18, &scratch[1], arg4, arg0);
    (void)outcome;

    return 1;
}

}  // namespace Net
