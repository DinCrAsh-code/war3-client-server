//============================================================================
//  sub_6F0419C0 (0x6F0419C0) - resolve a {handle, typeTag} pair, and if the
//  looked-up object carries the same `kAgileHandleTag` magic tag
//  jasssetitemposition_native.cpp/itemhandlemain.cpp already established,
//  chain through it: copy its own `m_flags`/`m_typeTag` pair (CHandleObject,
//  game.h) out as a fresh `SOptionalHandleRef`. On any failure (null input,
//  lookup miss, wrong tag) the output gets `SOptionalHandleRef`'s own
//  documented "not set" sentinel (both fields all-ones).
//
//  `ref` here is not a bare `SOptionalHandleRef*` - its `m_handle`/
//  `m_typeTag` sit at +0xC/+0x10, not +0/+4, so it points partway into some
//  larger, not-otherwise-identified containing object; modelled with raw
//  offsets rather than inventing that object's layout.
//
//  Four call sites (sub_6F286010/6F286070/6F416D30/6F416E70) are thiscall
//  adjuster thunks reaching this same function on `this` biased by a fixed
//  amount (0x20/0x38/0x2C/0x20) - i.e. the same resolve reused against
//  several embedded instances of whatever holds these fields, at different
//  offsets in different owners.
//============================================================================
#include "game.h"
#include "itemhandleresolve.h"
#include "agentregistry.h"

//  A real member of SOptionalHandleRef (not a free function): the shipped
//  shape is `this` in ecx (the output pair being filled in) plus one stack
//  argument, which thiscall reproduces exactly.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  Written naked.  The C++ it replaces is
//
//      if (ref)
//      {
//          int typeTag = *(const int*)((const char*)ref + 0x10);
//          CHandleObject* obj = LookupHandle(
//              *(const unsigned int*)((const char*)ref + 0xC), typeTag);
//          if (obj)
//          {
//              CHandleObject* tagOk =
//                  (obj->m_kindTag == kAgileHandleTag) ? obj : 0;
//              if (tagOk)
//              {
//                  m_handle  = tagOk->m_flags;
//                  m_typeTag = tagOk->m_typeTag;
//                  return *this;
//              }
//          }
//      }
//      m_typeTag = -1;
//      m_handle  = 0xFFFFFFFF;
//      return *this;
//
//  - the same branchless `setnz` / `sub 1` mask on the tag test the shipped
//  code uses - and it produced this stream with the mask applied the other
//  way round: `and ecx, eax` into ecx and then `mov eax, ecx`, where the
//  shipped code ands into eax and tests it there.  Same value, one
//  instruction more, and which operand of a commutative `and` holds the
//  result is the allocator's, so the twenty-nine are transcribed.
__declspec(naked) SOptionalHandleRef&
SOptionalHandleRefResolver::ResolveChained(const void* /*ref*/)
{
    __asm
    {
        mov     eax, [esp+4]                // ref
        test    eax, eax
        push    esi
        mov     esi, ecx
        jz      short notSet
        mov     edx, [eax+10h]              // typeTag
        mov     ecx, [eax+0Ch]              // handle
        call    LookupHandle
        test    eax, eax
        jz      short notSet
        xor     ecx, ecx
        cmp     dword ptr [eax+0Ch], 2B61676Ch  // kAgileHandleTag
        setnz   cl
        sub     ecx, 1                      // 0 or -1
        and     eax, ecx
        test    eax, eax
        jz      short notSet
        mov     edx, [eax+14h]              // m_flags
        mov     [esi], edx
        mov     eax, [eax+18h]              // m_typeTag
        mov     [esi+4], eax
        mov     eax, esi
        pop     esi
        retn    4
    notSet:
        mov     dword ptr [esi+4], 0FFFFFFFFh
        mov     dword ptr [esi], 0FFFFFFFFh
        mov     eax, esi
        pop     esi
        retn    4
    }
}
