#include <windows.h>
#include "storm.h"
//============================================================================
//  Net::NetProviderBNET - vtable slot 8, 0x6F65EA70.  Under the +0x6D0
//  lock, if the +0x7D4 "populated" flag is set: clear it, populate a
//  fresh {status, count, items} triple through sub_6F65D470, tell every
//  item in the resulting array (through this object's own vtable+0x78
//  slot) that it is being torn down, clean up the list through
//  sub_6F65D390, notify through sub_6F65AC00, and free the items array if
//  one was returned.  Carries the same __except_handler4-shaped SEH frame
//  docs/msvc-vc8-idioms.md documents as unreproducible at this build's
//  fixed /GS- /EHs-c- - reconstructed as a plain function with no cookie/
//  SEH scaffolding of its own.
//============================================================================

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

//  The type-descriptor literal both sub_6F65D470's own manual free and its
//  own `TSGrowableArray<void*>::SetAlloc` call (0x6F3A1AA0, ptrarray.cpp)
//  push - IDA names it `aK_1` and this session did not resolve its real
//  text (unresolved-symbol pushes canonicalise identically regardless of
//  content, so it costs no score either way).
static const char s_bnetListTypeTag[] = "?";

//  0x6F65D470 - resize this object's own +0x650/+0x64C linked-list-backed
//  count to `out`'s allocation (a real `TSGrowableArray<void*>` - its field
//  layout at +0/+4/+8 matches m_alloc/m_count/m_data exactly, and the
//  resize call is the template's own already-reconstructed SetAlloc), then
//  copy every live node's own value field into it. Un-thunked this
//  session (batch-E-net-misc-1); `this` in ecx, one stack pointer,
//  `retn 4`.
class NetProviderBnetListPopulator
{
public:
    void Populate(TSGrowableArray<void*>* out);
};
void NetProviderBnetListPopulator::Populate(TSGrowableArray<void*>* out)
{
    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x62C));

    unsigned int wantCount = *(unsigned int*)((char*)this + 0x650);
    if (wantCount != out->m_count)
    {
        if (wantCount == 0)
        {
            if (out->m_data != 0)
                SMemFree(out->m_data, s_bnetListTypeTag, -2, 0);
            out->m_alloc = 0;
            out->m_count = 0;
            out->m_data = 0;
        }
        else
        {
            out->SetAlloc(wantCount);
            out->m_count = wantCount;
        }
    }

    //  +0x64C - a singly-linked list head; each node's own +8 is the
    //  value copied out, +4 the next pointer. The shipped comparison is
    //  a signed `> 0` (the field's own declared type, not a raw pointer
    //  test) - kept as the shipped code has it rather than rewritten to
    //  an unsigned null check. The entry guard is the same branchless
    //  clamp-to-positive-or-zero idiom NetProviderSlot27Self::LookupNode's
    //  own +0x610 count read already establishes (`(raw > 0) ? raw : 0`
    //  compiles to the shipped `xor/test/setle/sub/and` rather than a
    //  branch) - only the loop's own back-edge test stays a plain `> 0`.
    int rawHead = *(int*)((char*)this + 0x64C);
    char* node = (rawHead > 0) ? (char*)(intptr_t)rawHead : (char*)0;
    if (node > (char*)0)
    {
        int i = 0;
        do
        {
            void* value = *(void**)(node + 8);
            out->m_data[i] = value;
            node = *(char**)(node + 4);
            ++i;
        } while ((int)node > 0);
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x62C));
}

//  0x6F65D390 - list cleanup once every item has been notified: releases
//  every node of the same +0x64C list this class's own free-pool helpers
//  (netprovider_freepool_helpers.cpp) already reconstruct
//  (NetProviderSlot9Self::ReleasePayload/UnlinkAndFreeNode), under the
//  +0x62C lock. `this` in ecx, `retn 0` (tail `jmp` straight into
//  LeaveCritSec). Un-thunked this session.
//
//  Redeclares NetProviderSlot9Self here with only the two members this
//  function calls - the real class (netprovider_freepool_helpers.cpp)
//  already defines both out of line, so this TU just needs their exact
//  signatures to link against the same symbols.
class NetProviderSlot9Self
{
public:
    void ReleasePayload(int value);
    void UnlinkAndFreeNode(void* node);
};
class NetProviderBnetListCleanup
{
public:
    void Cleanup();
};
void NetProviderBnetListCleanup::Cleanup()
{
    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x62C));

    //  A plain `while`, not a `for(;;)` with a `break` - the shipped body
    //  reloads +0x64C and tests it exactly once per iteration, with the
    //  backward branch landing on that same reload rather than duplicating
    //  the test at the bottom (the loop is not rotated into a do-while
    //  with a separate entry guard, the way the `for(;;)`/`break` spelling
    //  compiled it).
    void* node = *(void**)((char*)this + 0x64C);
    while ((int)node > 0)
    {
        ((NetProviderSlot9Self*)this)->ReleasePayload(*(int*)((char*)node + 8));
        ((NetProviderSlot9Self*)this)->UnlinkAndFreeNode(node);
        node = *(void**)((char*)this + 0x64C);
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x62C));
}

//  0x6F6588A0 - the real logging/assert sink NetProviderBnetPostNotify's
//  own Notify (below) forwards into; two plain stdcall stack dwords
//  (category, &codeCopy), `retn 8` confirmed off its own dump tail. Its
//  own SEH-framed body is out of batch-E-net-misc-1's scope - a thunk to
//  the real, unhooked target, the same reasoning as LogNotify_6F6587B0
//  (netproviderbnet_locks.cpp).
void __stdcall LogNotify_6F6588A0(int category, int* code);
__declspec(naked) void __stdcall LogNotify_6F6588A0(int, int*)
{
    __asm { mov eax, 0x6F6588A0 }
    __asm { jmp eax }
}

//  0x6F65AC00 - post-cleanup notify; `retn 4`, same "copy to a local and
//  forward by address with a fixed category" shape as
//  NetProviderBnetFirstTimeNotify::Notify (netproviderbnet_locks.cpp),
//  category 10 rather than 9. `this` is declared but never read.
class NetProviderBnetPostNotify
{
public:
    void Notify(int code);
};
void NetProviderBnetPostNotify::Notify(int code)
{
    int codeCopy = code;
    LogNotify_6F6588A0(10, &codeCopy);
}

namespace Net
{

class NetProviderBNET
{
public:
    //  0x6F65EA70 - vtable slot 8.  `retn 0`.
    void Method_8();
};

void NetProviderBNET::Method_8()
{
    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));

    int* populated = (int*)((char*)this + 0x7D4);
    if (*populated != 0)
    {
        *populated = 0;

        TSGrowableArray<void*> result = { 0, 0, 0, 0 };
        ((NetProviderBnetListPopulator*)this)->Populate(&result);

        for (unsigned int i = 0; i < result.m_count; ++i)
        {
            void* item = result.m_data[i];
            typedef void (__thiscall *Slot0x78Fn)(void*, void*, int);
            Slot0x78Fn fn = (Slot0x78Fn)(*(void***)this)[0x78 / 4];
            fn(this, item, 1);
        }

        ((NetProviderBnetListCleanup*)this)->Cleanup();
        ((NetProviderBnetPostNotify*)this)->Notify(1);

        if (result.m_data != 0)
            SMemFree(result.m_data, s_bnetListTypeTag, -2, 0);
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));
}

}  // namespace Net
