#include <windows.h>
//============================================================================
//  Net::NetProviderBNET - vtable slots 26 (0x6F65EB50), 27 (0x6F65EC70),
//  28 (0x6F660830), 29 (0x6F65ED50) and 30 (0x6F65E580).  Five SEH-cookie-
//  guarded overrides (the `dword_6FAAE140` xor-into-esp pattern
//  docs/msvc-vc8-idioms.md documents this build's fixed /GS- cannot
//  reproduce regardless of source shape - Method_28 additionally carries a
//  full `__except_handler4`-shaped frame) that build a CDataStore/
//  CDataStoreScratch-backed record on the stack via sub_6F65CD80/
//  sub_6F65D3D0/sub_6F654190/sub_6F654270 and forward it through this
//  object's own vtable+0x9C/+0xA0/+0xA4 slots or the inner +0x2B4
//  interface. Un-thunked this session (batch-E-net-misc-1) by transcribing
//  each call site instruction by instruction; several of the callees below
//  (sub_6F65D2E0, sub_6F68A210, sub_6F68A430, sub_6F65D3D0, sub_6F65E840,
//  sub_6F6603D0, sub_6F65E7F0, sub_6F6570A0) have no dump in this batch's
//  own scope and stay naked thunks to their real, unhooked bodies - each
//  one's own stack-argument count is verified against its own dump tail
//  (`retn <n>`) before being declared, which is what makes a wrong count
//  here a caught mistake rather than a live crash.
//
//  Argument *counts* below are load-bearing (each must push exactly the
//  bytes the shipped body's own `retn <n>` cleans - thunk_abi_audit.py's
//  whole reason for existing, confirmed against each root's own dump);
//  the *types* are plain ints rather than a guess at real meaning.
//============================================================================
#include "storm.h"
#include "cdatastorescratch.h"

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

void __stdcall Storm_501(char* dest, const char* src, unsigned int size);
extern "C" void* __cdecl memset(void* dst, int val, unsigned int count);

//  netproviderbnet_recv.cpp's own shared BNET-socket-resolve wrapper and
//  status-signal tail, needed by both ForwardBnetSendRecord_6F68A210 and
//  SendBnetFramed_6F68A430 below - see that file's own header comment for
//  the batch-K-netprovider-misc ABI fix (the real callee takes `handle`
//  as a fifth *stack* argument, not folded into `edx`, and its out-status
//  pointer at the third stack position).
extern "C" void* __fastcall
ResolveBnetSocket_6F688F30(void* flagsTable, void* edxArg, void* handle,
                            int arg1, int* outStatus, int arg2, int arg3);
//  Named, not anonymous - see netproviderbnet_recv.cpp's own declaration for
//  why (an anonymous tag's real MSVC mangling breaks ml.exe on gen_data.asm).
struct SBnetSocketResolveTable { char reserved[1]; };
extern SBnetSocketResolveTable g_bnetSocketResolveTable;
extern int SignalBnetSocketStatus(int status);
void __stdcall SErrSetLastError(unsigned int code);

//  NetProviderSlot27Self::LookupNode - already reconstructed for real in
//  netprovider_slot27.cpp; redeclared here with only what this TU needs to
//  link against the same symbol.
struct LookupNodeQuad { int d0, d4, d8, dC; };
struct LookupNodeBlock184 { int words[46]; };

//  A 32-byte sub-object copied whole is what makes MSVC emit `rep movsd`
//  (docs/msvc-vc8-idioms.md, "MSVC's implicit POD copy is sized, not
//  membered") - ten individual field copies do not, so the block this
//  node's own +0x2C points at has to exist as a real 8-dword struct for
//  the struct-assignment below to reproduce the shipped `rep movsd`.
struct Block32 { int words[8]; };

//  The single by-key list both BnetSendRecordBuilder::Build and
//  BnetSendTargetLookup::Lookup (below) walk, guarded by the
//  CRITICAL_SECTION at `this`+0x62C with its head at `this`+0x64C.
struct BnetSendTargetListNode
{
    int                       d00;
    BnetSendTargetListNode*   next;      // +0x04
    int                       key;       // +0x08
    int                       d0C, d10;
    int                       payload[4];// +0x14 - the 4-dword "record" copy
    int                       d24;
    int                       d28;       // +0x28 - BnetSendTargetLookup's out3
    Block32*                  block2C;   // +0x2C - the 32-byte block copy
};

class NetProviderSlot27Self
{
public:
    int LookupNode(int key, const char* name, int* out8, int* outC,
                   LookupNodeQuad* outQuad, LookupNodeBlock184* outBlock);
};

//  Both self-vtable dispatch shapes every one of these five slots reaches:
//  slot 0x9C, `this` plus three plain ints/pointers, and slot 0xA0, `this`
//  plus five. Neither is a guess at meaning - just at how many words the
//  real, unhooked callee expects, read straight off each call site's own
//  push count.
typedef void (__thiscall *Slot0x9CFn)(void*, int, int, void*);
typedef void (__thiscall *Slot0xA0Fn)(void*, int, int, int, void*, int);
typedef void (__thiscall *Slot0xA4Fn)(void*, int, int);
//  The inner +0x2B4 interface's own vtable+0xC slot - the exact same
//  nine-stack-argument shape Net::NetProvider::ForwardWithLookup
//  (netprovider_slot27.cpp) already established, reused verbatim by three
//  of these five overrides.
typedef void (__thiscall *InnerSlot0xCFn)(void*, void*, void*, int, int,
                                          void*, int, const char*,
                                          unsigned short, int);

//  0x6F65D2E0 - the same critical-section-guarded by-key list lookup
//  BnetSendTargetLookup::Lookup (above) walks, over the identical
//  `this`+0x64C list of `BnetSendTargetListNode`s: on a match, copies the
//  node's own 4-dword payload at +0x14 into `record` and its own 32-byte
//  block at +0x2C into `block32` (the same `rep movsd`-sized struct copy -
//  **not** a single status scalar; see the bug note below), then marks the
//  slot used through the thunked sub_6F65AC20 (itself just a stack-frame
//  forwarder into the `__except_handler4`-shaped sub_6F658B70 this
//  session does not attempt, the same unreproducible-under-this-build's
//  fixed /GS- frame docs/msvc-vc8-idioms.md documents) and removes the
//  node through the thunked sub_6F65D000. `this` in ecx, three stack args
//  (`key`, `record`, `block32`), `retn 0Ch` confirmed off its own dump
//  tail.
//
//  **Bug fix, not a rewrite**: the previous naked-thunk declaration typed
//  the third parameter `int* status` (four bytes) on the strength of
//  Method_30's own call-site guess below; the real body writes a full
//  32-byte block into it via `rep movsd`.  Method_30's own `int status`
//  local was a 28-byte stack buffer overflow on every successful call -
//  live, on the shipped instruction stream itself, invisible to
//  verify.py (which never links or runs) and to thunk_abi_audit.py (the
//  byte count the thunk declared into the *shipped* callee - retn 0Ch -
//  was always right; this is a local's *size* being wrong, not the ABI).
//  Fixed in the same commit as this reconstruction: Method_30's local is
//  now a `Block32`.
class BnetSendRecordBuilder
{
public:
    int Build(int key, void* record, void* block32);
};

static void AckBnetSendSlot_6F65AC20(void* self, int flag);
static void RemoveBnetSendNode_6F65D000(void* self, void* node);

int BnetSendRecordBuilder::Build(int key, void* record, void* block32)
{
    CRITICAL_SECTION* cs = (CRITICAL_SECTION*)((char*)this + 0x62C);
    EnterCritSec(cs);

    int found = 0;
    int headRaw = *(int*)((char*)this + 0x64C);
    int node = headRaw > 0 ? headRaw : 0;
    if (node > 0)
    {
        for (;;)
        {
            BnetSendTargetListNode* n = (BnetSendTargetListNode*)node;
            if (n->key == key)
            {
                ((int*)record)[0] = n->payload[0];
                ((int*)record)[1] = n->payload[1];
                ((int*)record)[2] = n->payload[2];
                ((int*)record)[3] = n->payload[3];
                *(Block32*)block32 = *n->block2C;

                AckBnetSendSlot_6F65AC20(this, 1);
                RemoveBnetSendNode_6F65D000(this, n);

                found = 1;
                break;
            }
            node = (int)n->next;
            if (node <= 0)
                break;
        }
    }

    LeaveCritSec(cs);
    return found;
}

//  0x6F65AC20 - out of scope: a stack-frame forwarder into the
//  `__except_handler4`-shaped sub_6F658B70 (2 levels deep - the exact
//  unreproducible frame docs/msvc-vc8-idioms.md documents), `this` in
//  ecx (threaded straight through untouched), one stack arg, `retn 4`
//  confirmed off its own dump tail.
class AckBnetSendSlotTarget
{
public:
    void Ack(int flag);
};
__declspec(naked) void AckBnetSendSlotTarget::Ack(int)
{
    __asm { mov eax, 0x6F65AC20 }
    __asm { jmp eax }
}
static void AckBnetSendSlot_6F65AC20(void* self, int flag)
{
    ((AckBnetSendSlotTarget*)self)->Ack(flag);
}

//  0x6F65D000 - out of scope: unlinks a node from the list (or an
//  Unlink-shaped `TSLink`-adjacent walk - the `test [esi+4]` / `not ecx`
//  branch is the same "prevlink positive-or-complemented" shape
//  storm.h's own TSLink::Unlink documents) and releases it and the
//  node's own +0x2C block through Storm_403/SMemFree, then decrements
//  this object's own +0x650 refcount. `this` in ecx, one stack arg (the
//  node pointer), `retn 4` confirmed off its own dump tail.
class RemoveBnetSendNodeTarget
{
public:
    void Remove(void* node);
};
__declspec(naked) void RemoveBnetSendNodeTarget::Remove(void*)
{
    __asm { mov eax, 0x6F65D000 }
    __asm { jmp eax }
}
static void RemoveBnetSendNode_6F65D000(void* self, void* node)
{
    ((RemoveBnetSendNodeTarget*)self)->Remove(node);
}

//  0x6F68A210 - forwards the record built above onward through the
//  +0x6A0 OS handle; `this`/`code` in ecx/edx (fastcall), two stack args
//  (record pointer, status pointer), `retn 8`.  Validates `code` (must be
//  1, 2 or 3) and `outStatus` (must be non-null) up front - a rejection
//  is `SErrSetLastError(0x57)` and a plain return, no resolve attempted -
//  then shares the exact same resolve/forward/signal shape
//  ResolveBnetSocket_6F688F30's own header comment (netproviderbnet_recv.cpp)
//  documents, forwarding through sub_6F6A36E0 on a successful resolve.
//
//  sub_6F6A36E0 itself (`retn 0Ch`, `this`=resolvedHandle plus three
//  stack args, its own SEH-cookie-guarded body reaching a further
//  undumped callee sub_6F6A06D0) is genuinely deep - out of this batch's
//  own scope; stays a naked thunk, argument order confirmed against this
//  call site's own push sequence (6F68A266..6F68A26F).
class ForwardOnResolvedSocketTarget
{
public:
    int Forward(int code, void* record, void* outStatus);
};
__declspec(naked) int
ForwardOnResolvedSocketTarget::Forward(int, void*, void*)
{
    __asm { mov eax, 0x6F6A36E0 }
    __asm { jmp eax }
}

int __fastcall ForwardBnetSendRecord_6F68A210(void* handle, int code,
                                               void* record, void* outStatus)
{
    if ((code < 1 || code > 3) || outStatus == 0)
    {
        SErrSetLastError(0x57);
        return 0;
    }

    int localStatus = -1;
    void* resolved = ResolveBnetSocket_6F688F30(
        &g_bnetSocketResolveTable, (void*)code, handle, 0, &localStatus, 0, 1);
    int result = 5;
    if (resolved != 0)
    {
        result = ((ForwardOnResolvedSocketTarget*)resolved)->Forward(
            code, record, outStatus);
        SignalBnetSocketStatus(localStatus);
    }
    return result;
}

//  0x6F68A430 - the same resolve/forward/signal shape netproviderbnet_recv.cpp's
//  own four functions share (see that file's own header comment for the
//  ResolveBnetSocket_6F688F30 ABI fix this reconstruction is the fifth
//  real caller of): resolve `handle` through the shared BNET-socket table,
//  forward six of the caller's own values (the caller's `edxVal` again,
//  `arg0..arg3`, and the caller's own `outStatus` pointer) through the
//  resolved-socket primitive sub_6F6A3840 on success (else the fixed
//  fallback 5), then signal `g_bnetSocketSignalPair` off the *local*
//  resolve status (not the caller's `outStatus`, which sub_6F6A3840 itself
//  writes through directly - the same two-status split every one of that
//  file's own four siblings has).  `__fastcall(ecx=handle, edx=edxVal)`
//  plus five stack args, `retn 14h`.
//
//  sub_6F6A3840 itself (`retn 18h`, `this`=resolvedHandle plus six stack
//  args) is genuinely deep - its own SEH-cookie-guarded body reaches a
//  further undumped callee (sub_6F6A0B30) and a `stru_6FACFEE0`-style
//  critical section of its own - out of this batch's own scope; stays a
//  naked thunk, argument order confirmed against this call site's own
//  push sequence (6F68A454..6F68A46E) rather than its own body.
class SendFramedOnResolvedSocketTarget
{
public:
    int Send(int edxVal, int arg0, int arg1, int arg2, int arg3, int* outStatus);
};
__declspec(naked) int
SendFramedOnResolvedSocketTarget::Send(int, int, int, int, int, int*)
{
    __asm { mov eax, 0x6F6A3840 }
    __asm { jmp eax }
}

int __fastcall SendBnetFramed_6F68A430(void* handle, int edxVal, int arg0,
                                        int arg1, int arg2, int arg3,
                                        int* outStatus)
{
    int localStatus = -1;
    void* resolved = ResolveBnetSocket_6F688F30(
        &g_bnetSocketResolveTable, (void*)edxVal, handle, 0, &localStatus, 0, 1);
    if (resolved == 0)
        return 5;

    int result = ((SendFramedOnResolvedSocketTarget*)resolved)->Send(
        edxVal, arg0, arg1, arg2, arg3, outStatus);
    SignalBnetSocketStatus(localStatus);
    return result;
}

//  0x6F65D3D0 - critical-section-guarded linked-list lookup by key: walk
//  the singly-linked list rooted at `this`+0x64C (guarded by the
//  CRITICAL_SECTION at `this`+0x62C) comparing each node's own +8 key
//  field against `key`; on a match, copy the node's own 4-dword payload
//  at +0x14 into `out1` (this is Method_29's own `BnetSendTarget4`, an
//  address/port pair plus two leading dwords Method_29 never reads back),
//  the 8-dword block its own +0x2C points at into `out2`, and its own +0x28
//  scalar into `out3`, returning 1; otherwise returns 0 with none of the
//  outputs touched.  `this` in ecx, four stack args (`key`/`out1`/`out2`/
//  `out3`), `retn 10h` confirmed off its own dump tail.  The node's own
//  "next" pointer at +4 and the `this`+0x64C head are both walked/tested
//  as *signed* ints (`jg`/`jle`), the same "non-positive == end/empty"
//  convention CopyProviderConfigTarget's own list-adjacent code elsewhere
//  in this session's batch does not use, but this call tree does.
class BnetSendTargetLookup
{
public:
    int Lookup(int key, void* out1, void* out2, void* out3);
};

int BnetSendTargetLookup::Lookup(int key, void* out1, void* out2, void* out3)
{
    CRITICAL_SECTION* cs = (CRITICAL_SECTION*)((char*)this + 0x62C);
    EnterCritSec(cs);

    int found = 0;
    //  The masked-entry idiom (docs/msvc-vc8-idioms.md, "A masked link value
    //  walked as a raw int"): `head > 0 ? head : 0` is the `setle`/`sub`/
    //  `and` clamp the shipped stream spells before ever dereferencing the
    //  head, and the walk below has to keep testing the raw signed int
    //  rather than a `T*` to keep reproducing the `jle`/`jg` the shipped
    //  code uses instead of a redundant pointer-truthiness test.
    int headRaw = *(int*)((char*)this + 0x64C);
    int node = headRaw > 0 ? headRaw : 0;
    if (node > 0)
    {
        for (;;)
        {
            BnetSendTargetListNode* n = (BnetSendTargetListNode*)node;
            if (n->key == key)
            {
                ((int*)out1)[0] = n->payload[0];
                ((int*)out1)[1] = n->payload[1];
                ((int*)out1)[2] = n->payload[2];
                ((int*)out1)[3] = n->payload[3];
                *(Block32*)out2 = *n->block2C;
                *(int*)out3 = n->d28;
                found = 1;
                break;
            }
            node = (int)n->next;
            if (node <= 0)
                break;
        }
    }

    LeaveCritSec(cs);
    return found;
}

//  0x6F65E840 - stamp a fresh 184-byte scratch record's own header: format
//  the fixed literal "(team game)" into its first 0x20 bytes and the
//  `ValueName` literal (the same unresolved-content string
//  Method_29's own `s_valueName` below already stands in for) into the
//  next 0x10, then store the caller's own `value` (ecx) at +0xB0 and the
//  constant 0x1000 at +0xB4.  Plain fastcall(ecx=value, edx=record),
//  `retn` bare - no stack args to clean.
static const char s_teamGame[] = "(team game)";

//  A fixed field-name string literal Method_29's own dump names `ValueName`
//  (an IDA label for a string constant this session did not resolve the
//  real text of - unresolved-symbol pushes canonicalise identically
//  regardless of content).  Moved up from its original spot just above
//  `namespace Net` so this earlier user (0x6F65E840) can see it too.
static const char s_valueName[] = "?";

void __fastcall MarkBnetSendSlot_6F65E840(int value, void* record)
{
    char* rec = (char*)record;
    Storm_501(rec, s_teamGame, 0x20);
    Storm_501(rec + 0x20, s_valueName, 0x10);
    *(int*)(rec + 0xB0) = value;
    *(unsigned int*)(rec + 0xB4) = 0x1000;
}

//  0x6F65E7F0 - fill a CDataStoreScratch from the 184-byte table
//  Method_28 formats: write the table's own +4 count byte through
//  CDataStoreScratch::WriteByte (0x6F4C2160, already reconstructed), then
//  write each of that many 0x10-byte entries starting at the table's own
//  +8 base pointer as a NUL-terminated string through
//  CDataStoreScratch::WriteString (0x6F4C2BA0, already reconstructed) -
//  both already-known real bodies, once their call site itself was
//  transcribed for real.  `ecx`=the scratch object, `edx`=the table
//  pointer, `retn 4` - one genuine register argument in `edx` plus one
//  dead stack dword the shipped body never reads (the mirror image of
//  the spurious-`edx` shape CNetData_SenderFromKeyLookup already uses in
//  netcommand_dispatch.cpp: there the real argument is on the stack and
//  `edx` is dead; here it is the other way round).  Returns `this`,
//  matching every other CDataStoreScratch writer's own fluent shape.
CDataStoreScratch* __fastcall FillScratchFromTable_6F65E7F0(
    CDataStoreScratch* self, void* table, int /*unused_stack_arg*/)
{
    char* t = (char*)table;
    self->WriteByte(*(unsigned char*)(t + 4));

    unsigned int i = 0;
    if (i < *(unsigned int*)(t + 4))
    {
        char* entry = *(char**)(t + 8);
        do
        {
            self->WriteString(entry);
            i += 1;
            entry += 0x10;
        } while (i < *(unsigned int*)(t + 4));
    }

    return self;
}

//  0x6F6570A0 - one shipped address, two call sites: already reconstructed
//  for real as Net::NetProvider::Method_25 (netprovider_slot25.cpp, vtable
//  slot 25) from a *different* caller (NetProviderBase_Register's own
//  install chain). Redeclared here rather than re-thunked - the exact
//  "one shipped address carrying two C++ names" defect
//  docs/notes/thunk-removal-pass.md documents thunk_inventory.py existing
//  to catch. Local re-declaration of the same class/method shape this
//  repo already uses everywhere else a method is shared across TUs
//  (netproviderbnet_lifecycle.cpp's own `class NetProvider { int
//  Method_0x2(); };`, etc).
namespace Net { class NetProvider { public: int Method_25(void*, void*, int); }; }

//  0x6F6DAEF0 - BuildSockAddr, already reconstructed for real
//  (sockaddrinet.cpp); redeclared here with only what this TU needs. A
//  plain fastcall free function, not a member - `addr`/`port` in ecx/edx.
void __fastcall BuildSockAddr(unsigned int addr, unsigned short port, void* out);

//  0x6F65D3D0's own second out-param is a 4-dword struct (IDA split it
//  into `var_FC`/`var_F8`/`var_F4`/`var_F0` rather than recognising one
//  struct, the same shape LookupNodeQuad already documents for
//  sub_6F65CD80's own out-param) - only the last two dwords (an address
//  and a port) are read back by Method_29 below.
struct BnetSendTarget4 { int d0, d4; unsigned int addr; unsigned int port; };

namespace Net
{

class NetProviderBNET
{
public:
    int  Method_26(int a0, int a4);            // 0x6F65EB50, slot 26, retn 8
    void Method_27(int a0, int a4, int a8);     // 0x6F65EC70, slot 27, retn 0Ch
    int  Method_28(int a0, int a4, void** a8);  // 0x6F660830, slot 28, retn 0Ch
    void Method_29(int a0, int a4);             // 0x6F65ED50, slot 29, retn 8
    void Method_30(int a0, int a4);             // 0x6F65E580, slot 30, retn 8
};

//----------------------------------------------------------------------------
//  0x6F65EB50 - slot 26.  Formats `a0`/`a4` into the front of a 184-byte
//  scratch record, looks the record up by key 0/name `a0` through
//  LookupNode, and either forwards it through vtable+0x9C (found) or tries
//  to send it directly through the +0x6A0 handle, falling back to
//  vtable+0x9C again if that send did not report success (1).
//----------------------------------------------------------------------------
int NetProviderBNET::Method_26(int a0, int a4)
{
    LookupNodeBlock184 record;
    char* recordBytes = (char*)&record;

    Storm_501(recordBytes, (const char*)a0, 0x20);
    *(int*)(recordBytes + 0x20) = 0;
    *(int*)(recordBytes + 0x30) = 0;
    Storm_501(recordBytes + 0x20, (const char*)a4, 0x10);

    int out8 = 0, outC = 0;
    LookupNodeQuad quad = { 0, 0, 0, 0 };
    int result = ((NetProviderSlot27Self*)this)->LookupNode(0, (const char*)a0,
                                                             &out8, &outC, &quad, &record);

    if (out8 != 0)
    {
        Slot0x9CFn fn = (Slot0x9CFn)(*(void***)this)[0x9C / 4];
        fn(this, 1, result, &record);
        return result;
    }

    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));

    int status;
    int rc = SendBnetFramed_6F68A430(*(void**)((char*)this + 0x6A0), 0x3FF, 0x3FF,
                                      1, a0, a4, &status);
    if (rc == 1)
    {
        *(int*)((char*)this + 0x6EC) = status;
        *(int*)((char*)this + 0x6E8) = 5;
        for (int i = 0; i < 0x2E; ++i)
            ((int*)((char*)this + 0x6F0))[i] = ((int*)recordBytes)[i];
    }
    else
    {
        Slot0x9CFn fn = (Slot0x9CFn)(*(void***)this)[0x9C / 4];
        fn(this, rc, 0, &record);
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));
    return result;
}

//----------------------------------------------------------------------------
//  0x6F65EC70 - slot 27.  Tells vtable+0xA4 "code 2" up front, formats `a4`
//  into a 184-byte scratch record the same way slot 26 does, looks it up by
//  key `a4`, and either forwards it through the inner +0x2B4 interface's
//  own vtable+0xC slot (found) or through vtable+0xA0 (not found).
//----------------------------------------------------------------------------
void NetProviderBNET::Method_27(int a0, int a4, int a8)
{
    (void)a0;

    Slot0xA4Fn slotA4 = (Slot0xA4Fn)(*(void***)this)[0xA4 / 4];
    slotA4(this, 2, 0);

    LookupNodeBlock184 record;
    char* recordBytes = (char*)&record;
    *(char*)(recordBytes + 0x18) = 0;
    *(char*)(recordBytes + 0x38) = 0;
    *(char*)(recordBytes + 0x48) = 0;
    Storm_501(recordBytes + 0x18, (const char*)a4, 0x10);

    int out8 = 0, outC = 0;
    LookupNodeQuad quad = { 0, 0, 0, 0 };
    int result = ((NetProviderSlot27Self*)this)->LookupNode(a4, 0, &out8, &outC,
                                                             &quad, &record);

    unsigned short cachedWord = *(unsigned short*)((char*)this + 0x2C0);

    if (out8 != 0)
    {
        void* inner = *(void**)((char*)this + 0x2B4);
        InnerSlot0xCFn fn = (InnerSlot0xCFn)(*(void***)inner)[0x0C / 4];
        fn(inner, this, &quad, result, outC, &record, a8, (const char*)a4, cachedWord, 0);
    }
    else
    {
        Slot0xA0Fn fn = (Slot0xA0Fn)(*(void***)this)[0xA0 / 4];
        fn(this, 7, 0, a8, &record, 0);
    }
}

//----------------------------------------------------------------------------
//  0x6F660830 - slot 28.  A distinct shape from the rest of this file:
//  formats every entry of the `a4`-length `a8` pointer array into a
//  184-byte scratch table via Storm_501, marks it through sub_6F65E840,
//  fills a CDataStoreScratch local from it (sub_6F65E7F0), and forwards
//  the scratch buffer through sub_6F6570A0. Carries the full
//  `__except_handler4`-shaped SEH frame docs/msvc-vc8-idioms.md documents
//  as unreproducible under this build's fixed /GS- - the biggest single
//  factor in this slot's own score gap.
//----------------------------------------------------------------------------
int NetProviderBNET::Method_28(int a0, int a4, void** a8)
{
    (void)a0;

    LookupNodeBlock184 table;
    char* tableBytes = (char*)&table;
    *(int*)(tableBytes + 0x08) = 0;
    *(int*)(tableBytes + 0x28) = 0;
    *(int*)(tableBytes + 0x38) = 0;

    MarkBnetSendSlot_6F65E840(a4 + 1, tableBytes);

    if (a4 > 0)
    {
        for (int i = 0; i < a4; ++i)
            Storm_501(tableBytes + 0x18 + i * 0x10, (const char*)a8[i], 0x10);
    }

    CDataStoreScratch scratch(0, 0);
    FillScratchFromTable_6F65E7F0(&scratch, tableBytes, 0);

    int result = ((Net::NetProvider*)this)->Method_25(&scratch, (void*)a0, 1);
    return result;
}

//----------------------------------------------------------------------------
//  0x6F65ED50 - slot 29.  Looks a send target up through sub_6F65D3D0 (out
//  of this batch's own scope); on a hit, resolves its address through the
//  already-reconstructed BuildSockAddr (0x6F6DAEF0, sockaddrinet.cpp),
//  marks the send slot, and forwards through the inner +0x2B4 interface's
//  vtable+0xC slot with the fixed name string; on a miss, zeroes a
//  184-byte scratch record and forwards through vtable+0xA0 instead.
//----------------------------------------------------------------------------
void NetProviderBNET::Method_29(int a0, int a4)
{
    BnetSendTarget4 target = { 0, 0, 0, 0 };
    int scalarOut = 0;
    int countOut = 0;
    int found = ((BnetSendTargetLookup*)this)->Lookup(a0, &target, &scalarOut, &countOut);

    if (found != 0)
    {
        char addrLocal[0x10];
        BuildSockAddr(target.addr, (unsigned short)target.port, addrLocal);

        LookupNodeBlock184 record;
        char* recordBytes = (char*)&record;
        *(int*)(recordBytes + 0x00) = 0;
        *(int*)(recordBytes + 0x20) = 0;
        *(int*)(recordBytes + 0x30) = 0;
        MarkBnetSendSlot_6F65E840(countOut + 1, recordBytes);

        unsigned short cachedWord = *(unsigned short*)((char*)this + 0x2C0);
        void* inner = *(void**)((char*)this + 0x2B4);
        InnerSlot0xCFn fn = (InnerSlot0xCFn)(*(void***)inner)[0x0C / 4];
        fn(inner, this, addrLocal, countOut, scalarOut, &record, a4, s_valueName, cachedWord, 0);
    }
    else
    {
        LookupNodeBlock184 record;
        memset(&record, 0, 0xB8);

        Slot0xA0Fn fn = (Slot0xA0Fn)(*(void***)this)[0xA0 / 4];
        fn(this, 7, 0, a4, &record, 0);
    }
}

//----------------------------------------------------------------------------
//  0x6F65E580 - slot 30.  Builds a record through the now-reconstructed
//  BnetSendRecordBuilder::Build and, if it built one, forwards it through
//  the +0x6A0 OS handle (sub_6F68A210) - `code` defaulting to 2 when the
//  caller's own `a4` is -1.  The `block` local is a real `Block32`, not
//  the `int status` this call site used to guess: Build's own third
//  parameter is a 32-byte `rep movsd` destination (see that function's own
//  header comment), so the previous 4-byte local was a live stack buffer
//  overflow.
//----------------------------------------------------------------------------
void NetProviderBNET::Method_30(int a0, int a4)
{
    char   record[0x10];
    Block32 block;

    int built = ((BnetSendRecordBuilder*)this)->Build(a0, record, &block);
    if (built != 0)
    {
        int code = a4;
        if (code == -1)
            code = 2;
        ForwardBnetSendRecord_6F68A210(*(void**)((char*)this + 0x6A0), code, record, &block);
    }
}

}  // namespace Net
