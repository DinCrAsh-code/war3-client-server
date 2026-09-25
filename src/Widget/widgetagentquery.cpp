//============================================================================
//  0x6F472C20 - SWidgetAgentQuery::Init.  See widgetagentquery.h.
//
//  Eleven stores, two of them -1 sentinels and five plain zeros, all from
//  two registers the compiler keeps loaded across the whole block.  The
//  type-object lookup is the only call, and it is what forces this into a
//  translation unit of its own.
//
//  A __fastcall free function, not a member and not a constructor: the type
//  id arrives in edx and only the pool is on the stack (`retn 4`), where a
//  thiscall member would have pushed both and cleaned eight - the exact
//  `ret <n>` mismatch tools/abi_audit.py exists to catch.
//============================================================================
#include "widgetagentquery.h"

//  0x6F4729C0 - agiletypequery.cpp.
void* __fastcall QueryAgileTypeObject(unsigned int typeId, int unused);

//  Written naked.  The C++ it replaces is the eleven stores in the shipped
//  order,
//
//      query->m_pTypeObject = QueryAgileTypeObject(typeId, 1);
//      query->m_pPool = pool;          query->m_typeId = typeId;
//      query->m_reserved28 = 0xFFFFFFFF;
//      query->m_reserved24 = 0xFFFFFFFF;
//      query->m_tag = 0x2B61676C;
//      query->m_reserved08 = 0;        query->m_reserved14 = 0;
//      query->m_reserved18 = 0;        query->m_reserved1C = 0;
//      query->m_reserved20 = 0;
//      return query;
//
//  which is the same call with the same arguments and the same eleven
//  stores of the same values - VC8 just schedules the block differently:
//  it materialises the zero later, keeps the tag store above the `pop edi`
//  instead of below it, and orders the five zero stores after rather than
//  around the sentinels.  Statement order does not move any of that (this
//  source is already in the shipped order), so the twenty-five
//  instructions are transcribed.
__declspec(naked) SWidgetAgentQuery* __fastcall InitWidgetAgentQuery(
        SWidgetAgentQuery* /*query*/, unsigned int /*typeId*/, void* /*pool*/)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, edx                    // typeId
        mov     esi, ecx                    // query
        mov     edx, 1
        mov     ecx, edi
        call    QueryAgileTypeObject
        mov     [esi+0Ch], eax
        mov     eax, [esp+0Ch]              // pool
        xor     ecx, ecx
        mov     [esi+10h], eax
        or      eax, 0FFFFFFFFh
        mov     [esi+4], edi
        mov     [esi+28h], eax
        mov     [esi+24h], eax
        pop     edi
        mov     dword ptr [esi], 2B61676Ch  // 'lga+'
        mov     [esi+8], ecx
        mov     [esi+14h], ecx
        mov     [esi+18h], ecx
        mov     [esi+1Ch], ecx
        mov     [esi+20h], ecx
        mov     eax, esi                    // return query
        pop     esi
        retn    4
    }
}
