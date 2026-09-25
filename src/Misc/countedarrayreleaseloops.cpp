//============================================================================
//  Two small, otherwise-unconnected "release every element, then the
//  array's own storage" leaves, grouped here because neither has any other
//  caller or callee in this batch to give it a home of its own - the
//  Misc/README shape src/README.md documents for "homeless" leaves.
//
//  Both `this` (ecx) objects are a plain {count @ +0x04, data @ +0x08}
//  pair - no capacity field, unlike Widget/spriteattachreleaseloop.cpp's
//  sibling shape (0x6F4D6760, which does track one) - with one Storm-owned
//  pointer per element at a fixed +0x08 offset off each stride, freed
//  unconditionally when non-null (no "small inline value" sentinel check
//  here), then the data block itself freed unconditionally when non-null.
//  Neither caller is in this batch's own dump, so nothing beyond these two
//  fields and the element stride is asserted about either owner's layout.
//  Both `retn 0`.
//============================================================================
#include "storm.h"

static const char s_elemTagA[] = ".?AUAnimElement@@";
static const char s_arrayTagA[] = ".\\AuAnim.cpp";

//  0x6F4EC2A0 - element stride 0x1C.
struct SAnimObjArray
{
    char         m_reserved00[4];
    unsigned int m_count;   // +0x04
    char*        m_data;    // +0x08, stride 0x1C

    void ReleaseAll();
};

__declspec(naked) void SAnimObjArray::ReleaseAll()
{
    __asm
    {
        push    ebx
        push    esi
        mov     esi, ecx
        xor     ebx, ebx
        cmp     [esi+4], ebx
        jbe     short after_loop
        push    edi
        xor     edi, edi
        mov     edi, edi
    loop_top:
        mov     eax, [esi+8]
        add     eax, edi
        mov     eax, [eax+8]
        test    eax, eax
        jz      short loop_next
        push    0
        push    0FFFFFFFEh
        push    offset s_elemTagA
        push    eax
        call    SMemFree
    loop_next:
        add     ebx, 1
        add     edi, 1Ch
        cmp     ebx, [esi+4]
        jb      short loop_top
        pop     edi
    after_loop:
        mov     esi, [esi+8]
        test    esi, esi
        jz      short done
        push    0
        push    0FFFFFFFEh
        push    offset s_arrayTagA
        push    esi
        call    SMemFree
    done:
        pop     esi
        pop     ebx
        retn
    }
}

static const char s_elemTagB[] = ".?AUPropElement@@";
static const char s_arrayTagB[] = ".\\AuPropArray.cpp";

//  0x6F4EE140 - same shape as SAnimObjArray::ReleaseAll above, element
//  stride 0x10.
struct SPropObjArray
{
    char         m_reserved00[4];
    unsigned int m_count;   // +0x04
    char*        m_data;    // +0x08, stride 0x10

    void ReleaseAll();
};

__declspec(naked) void SPropObjArray::ReleaseAll()
{
    __asm
    {
        push    ebx
        push    esi
        mov     esi, ecx
        xor     ebx, ebx
        cmp     [esi+4], ebx
        jbe     short after_loop
        push    edi
        xor     edi, edi
        mov     edi, edi
    loop_top:
        mov     eax, [esi+8]
        add     eax, edi
        mov     eax, [eax+8]
        test    eax, eax
        jz      short loop_next
        push    0
        push    0FFFFFFFEh
        push    offset s_elemTagB
        push    eax
        call    SMemFree
    loop_next:
        add     ebx, 1
        add     edi, 10h
        cmp     ebx, [esi+4]
        jb      short loop_top
        pop     edi
    after_loop:
        mov     esi, [esi+8]
        test    esi, esi
        jz      short done
        push    0
        push    0FFFFFFFEh
        push    offset s_arrayTagB
        push    esi
        call    SMemFree
    done:
        pop     esi
        pop     ebx
        retn
    }
}
