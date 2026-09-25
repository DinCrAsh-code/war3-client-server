//============================================================================
//  0x6F4D6760 - release every element of a small RTTI-tagged array, then the
//  array's own storage block.
//
//  `this` (ecx) is a `TSGrowableArray`-shaped sub-object: `m_alloc` at +0x00,
//  `m_count` at +0x04, `m_data` at +0x08 (storm.h), with each element 0x1C
//  bytes.  Reached (`lea ecx,[esi+0x38]; call`) from CSprite::~CSprite
//  (0x6F4D7990, Widget/spriteattachdtor.cpp) against CSprite's own
//  m_reserved37 block - so this is CSprite's attachment-array teardown, but
//  nothing here asserts any more of CSprite's own +0x38 sub-object layout
//  than the three TSGrowableArray fields this body itself reads.
//
//  Per element: a "small inline value" field at +0x08 (0, 1 or 2 - not a
//  real pointer) is left alone; anything above 2 is a real heap pointer at
//  +0x10 and is freed through Storm, tagged with the element's own RTTI
//  type-descriptor name (IDA's own `asc_6FA4F788`/`aAuspriteanim` labels -
//  reproduced as local placeholder literals rather than through funcmap's
//  DATA table, the same reasoning Widget/animtrackarray.cpp's own
//  s_animKeyTag/s_animTrackTag give: the exact bytes do not affect the
//  generated `offset` operand, which canonicalises to an anonymous symbol
//  either way).  `retn 0`.
//============================================================================
#include "storm.h"

static const char s_attachElemTag[] = ".?AUAttachElement@@";
static const char s_attachArrayTag[] = ".\\AuSpriteAnim.cpp";

struct SAttachArray
{
    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    char*        m_data;    // +0x08, element stride 0x1C

    void ReleaseAll();
};

__declspec(naked) void SAttachArray::ReleaseAll()
{
    __asm
    {
        push    esi
        push    edi
        mov     esi, ecx
        xor     edi, edi
        cmp     [esi+4], edi
        jbe     short after_loop
        push    ebx
        xor     ebx, ebx
        mov     edi, edi
    loop_top:
        mov     eax, [esi+8]
        cmp     dword ptr [ebx+eax+8], 2
        lea     eax, [ebx+eax+8]
        jbe     short loop_next
        mov     ecx, [eax+8]
        push    0
        push    0FFFFFFFEh
        push    offset s_attachElemTag
        push    ecx
        call    SMemFree
    loop_next:
        add     edi, 1
        add     ebx, 1Ch
        cmp     edi, [esi+4]
        jb      short loop_top
        pop     ebx
    after_loop:
        cmp     dword ptr [esi], 1
        jbe     short done
        mov     edx, [esi+8]
        push    0
        push    0FFFFFFFEh
        push    offset s_attachArrayTag
        push    edx
        call    SMemFree
    done:
        pop     edi
        pop     esi
        retn
    }
}
