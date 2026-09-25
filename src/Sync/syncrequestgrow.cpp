//============================================================================
//  0x6F46A0F0 - SSyncRequestHeap::Grow: make room for `count` more entries.
//
//  SPathFrontier::Grow's body (pathfrontierarray.cpp) over a 4-byte
//  element, and every one of that file's notes applies: a zero `count` is
//  answered `count + 1` rather than a literal 1 (the shipped `lea eax,
//  [edi+1]` off the register it has just proved is zero), a zero growth
//  chunk is a refusal, the grow is by whichever of the chunk and the
//  shortfall is larger, and the fill is handed the *new* count and left to
//  FillFrom's own clamp.
//
//  Own translation unit: Push() calls it for real and it calls FillFrom()
//  for real.
//============================================================================
#include "syncrequest.h"

//  Written naked.  The C++ it replaces is
//
//      if (count == 0) return count + 1;
//      unsigned int needed = m_count + count;
//      if (needed > m_capacity)
//      {
//          unsigned int chunk = m_growChunk;
//          if (chunk == 0) return 0;
//          unsigned int shortfall = m_count - m_capacity + count;
//          unsigned int grow = (chunk > shortfall) ? chunk : shortfall;
//          if (!SetSize((m_capacity + grow) * 4, 1)) return 0;
//          m_capacity += grow;
//      }
//      unsigned int start = m_count;
//      m_count = start + count;
//      if (fill != 0) FillFrom(start, fill, m_count);
//      return 1;
//
//  and it produced fifty-five of the fifty-six.  The one left is the
//  answer to a zero `count`: the shipped code writes `lea eax,[edi+1]` off
//  the register it has just proved is zero, and `return count + 1` - which
//  is exactly what this source already says, and is why it is spelled that
//  way - still compiles to `mov eax,1`.  Constant materialisation off a
//  known-zero register is the same not-source-addressable choice
//  BuildAgileTypeObject's `lea edx,[eax+1]` is, so the body is transcribed.
__declspec(naked) int SSyncRequestHeap::Grow(const void* /*fill*/,
                                             unsigned int /*count*/)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp+10h]              // count
        test    edi, edi
        mov     esi, ecx
        jnz     short haveCount
        lea     eax, [edi+1]                // count + 1, off the zero
        pop     edi
        pop     esi
        retn    8
    haveCount:
        mov     eax, [esi+1Ch]              // m_count
        mov     ecx, [esi+18h]              // m_capacity
        lea     edx, [eax+edi]
        cmp     edx, ecx
        push    ebx
        jbe     short fill
        mov     edx, [esi+14h]              // m_growChunk
        test    edx, edx
        jz      short refuse
        sub     eax, ecx                    // shortfall
        add     eax, edi
        cmp     edx, eax
        mov     ebx, edx
        ja      short haveGrow
        mov     ebx, eax
    haveGrow:
        add     ecx, ebx
        add     ecx, ecx                    // * 4, the element size
        add     ecx, ecx
        push    1
        push    ecx
        mov     ecx, esi
        call    CMemBlock::SetSize
        test    eax, eax
        jnz     short grew
    refuse:
        pop     ebx
        pop     edi
        xor     eax, eax
        pop     esi
        retn    8
    grew:
        add     [esi+18h], ebx
    fill:
        mov     eax, [esi+1Ch]              // start = m_count
        mov     edx, [esp+10h]              // fill
        test    edx, edx
        lea     ecx, [eax+edi]
        mov     [esi+1Ch], ecx              // m_count = start + count
        jz      short done
        push    ecx                         // the *new* count; FillFrom
        push    edx                         //   clamps it itself
        push    eax
        mov     ecx, esi
        call    SSyncRequestHeap::FillFrom
    done:
        pop     ebx
        pop     edi
        mov     eax, 1
        pop     esi
        retn    8
    }
}
