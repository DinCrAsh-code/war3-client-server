//============================================================================
//  0x6F47D810 / 0x6F47DA10 - growing the collected set, and the cell-box
//  walk that fills it.
//
//  One translation unit: neither calls the other, and each of their real
//  callees (FillEntries, CollectCell) is somewhere else.
//============================================================================
#include "agentquery.h"

//----------------------------------------------------------------------------
//  0x6F47D810 - the SPathFrontier::Grow shape again, eight bytes an element.
//----------------------------------------------------------------------------
//  Written naked, for the one instruction the C++ below could not reach -
//  the shipped `lea eax,[edi+1]` answer to a zero `count`, materialised off
//  the register just proved zero, which `return count + 1` compiles to
//  `mov eax,1` (the same not-source-addressable choice SSyncRequestHeap::Grow
//  and BuildAgileTypeObject carry).  Everything else was already the shipped
//  stream:
//
//      if (count == 0) return count + 1;
//      unsigned int needed = m_count + count;
//      if (needed > m_capacity)
//      {
//          unsigned int chunk = m_growChunk;
//          if (chunk == 0) return 0;
//          unsigned int shortfall = m_count - m_capacity + count;
//          unsigned int grow = (chunk > shortfall) ? chunk : shortfall;
//          if (!SetSize((m_capacity + grow) * 8, 1)) return 0;
//          m_capacity += grow;
//      }
//      unsigned int start = m_count;
//      m_count = start + count;
//      if (value != 0) FillEntries(start, value, m_count);
//      return 1;
__declspec(naked) int AgileAgentQuery::AppendEntries(
        const SAgentQueryHit* /*value*/, unsigned int /*count*/)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp+10h]              // count
        test    edi, edi
        mov     esi, ecx
        jnz     short haveCount
        lea     eax, [edi+1]
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
        sub     eax, ecx
        add     eax, edi
        cmp     edx, eax
        mov     ebx, edx
        ja      short haveGrow
        mov     ebx, eax
    haveGrow:
        add     ecx, ebx
        add     ecx, ecx                    // * 8, the element size
        add     ecx, ecx
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
        mov     eax, [esi+1Ch]
        mov     edx, [esp+10h]              // value
        test    edx, edx
        lea     ecx, [eax+edi]
        mov     [esi+1Ch], ecx
        jz      short done
        push    ecx
        push    edx
        push    eax
        mov     ecx, esi
        call    AgileAgentQuery::FillEntries
    done:
        pop     ebx
        pop     edi
        mov     eax, 1
        pop     esi
        retn    8
    }
}

//----------------------------------------------------------------------------
//  0x6F47DA10 - clip `box` against the grid's own extent and collect every
//  cell that is left.
//
//  The grid's visit stamp is bumped once for the whole walk and, when this
//  query is anchored to a registration of its own, stamped straight into it
//  so that the walk never collects the object asking the question.
//----------------------------------------------------------------------------
void AgileAgentQuery::CollectBox(CCellGrid* grid, const SCellBox* box)
{
    int x1 = box->m_x1 < grid->m_extent.m_x1 ? box->m_x1 : grid->m_extent.m_x1;
    int y1 = box->m_y1 < grid->m_extent.m_y1 ? box->m_y1 : grid->m_extent.m_y1;
    int x0 = box->m_x0 > grid->m_extent.m_x0 ? box->m_x0 : grid->m_extent.m_x0;
    int y0 = box->m_y0 > grid->m_extent.m_y0 ? box->m_y0 : grid->m_extent.m_y0;

    if (y0 >= y1)
        return;
    if (x0 >= x1)
        return;

    grid->m_visitStamp++;

    int stamp = grid->m_visitStamp;
    if (m_anchor != 0)
        m_anchor->m_visitStamp = stamp;

    for (unsigned int y = (unsigned int)y0; y < (unsigned int)y1; y++)
        for (unsigned int x = (unsigned int)x0; x < (unsigned int)x1; x++)
            CollectCell(grid, grid->m_stride * y + x, stamp);
}
