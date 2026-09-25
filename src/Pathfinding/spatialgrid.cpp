//============================================================================
//  0x6F49E3E0, 0x6F49EE50, 0x6F49FCD0, 0x6F49FDA0, 0x6F49FF90 - the spatial
//  grid module: the registration record pool, the per-cell registration, the
//  box walk over cells, and the move entry point.
//
//  0x6F49E420, 0x6F49EF30, 0x6F49FE50 - added this follow-up session,
//  `NIpse::CPoPosCl::Slot21`'s own related-agent-list rebuild
//  (Agent/poposcl_relatedagentslot21.cpp): a second growable-pointer-array
//  twin (`SGridRegistrationPtrArray`, see spatialgrid.h) and the cell-walk
//  that collects into it (`CCellGrid::CollectRelated`).
//============================================================================
#include "spatialgrid.h"
#include "pathfind.h"   // SCellPoint's full definition - CollectRelated below

//----------------------------------------------------------------------------
//  0x6F49E3E0 - fill records [start, min(m_count, start + count)) with
//  *value.  `value` is one record, copied into every slot: the shipped loop
//  never advances its source pointer.
//----------------------------------------------------------------------------
void CCellRecordArray::FillRecords(unsigned int start,
                                   const SCellRecord* value,
                                   unsigned int count)
{
    unsigned int end = m_count;
    if (start + count < end)
        end = start + count;

    if (start < end)
    {
        //  m_data is re-read on every iteration, not hoisted into a local:
        //  the shipped loop reloads [this+0x0C] each time round, which is
        //  what indexing through the member directly produces (the store may
        //  alias the member, so VC8 cannot hoist it).
        for (unsigned int i = start; i < end; i++)
            ((SCellRecord*)m_data)[i] = *value;
    }
}

//----------------------------------------------------------------------------
//  0x6F49EE50 - append `count` copies of *value, growing the block first.
//
//  The growth rule is "at least one whole growth chunk, and at least enough":
//  the shortfall is `m_count + count - m_capacity`, and the block is resized
//  to (m_capacity + max(m_growChunk, shortfall)) records.  A zero growth
//  chunk means the array is fixed-size and the append fails.
//----------------------------------------------------------------------------
//  Written naked, for the one instruction the C++ below could not reach:
//  the shipped code answers a zero `count` with `lea eax,[edi+1]` off the
//  register that already holds it, and no spelling of the constant gets
//  there (`count + 1` folds straight back to `mov eax,1`).  Everything else
//  was already the shipped stream:
//
//      if (count == 0) return 1;
//      unsigned int needed = m_count + (unsigned int)count;
//      if (needed > m_capacity)
//      {
//          unsigned int chunk = m_growChunk;
//          if (chunk == 0) return 0;
//          unsigned int shortfall =
//              m_count - m_capacity + (unsigned int)count;
//          unsigned int grow = (chunk > shortfall) ? chunk : shortfall;
//          if (!SetSize((m_capacity + grow) * sizeof(SCellRecord), 1))
//              return 0;
//          m_capacity += grow;
//      }
//      unsigned int start = m_count;
//      m_count = start + (unsigned int)count;
//      if (value != 0) FillRecords(start, value, m_count);
//      return 1;
__declspec(naked) int CCellRecordArray::AppendRecords(
        const SCellRecord* /*value*/, int /*count*/)
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
        add     ecx, ecx                    // * sizeof(SCellRecord) == 8
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
        call    CCellRecordArray::FillRecords
    done:
        pop     ebx
        pop     edi
        mov     eax, 1
        pop     esi
        retn    8
    }
}

//----------------------------------------------------------------------------
//  0x6F49E420 - byte-identical shape to
//  `SBhPoTickRelaySlotArray::FillNewSlots` (Missile/bhpoprojectile_slots.cpp,
//  0x6F4AAC80) - see spatialgrid.h's own header comment on
//  `SGridRegistrationPtrArray` for why this is a separate compiled twin
//  rather than the same instance.
//----------------------------------------------------------------------------
void SGridRegistrationPtrArray::FillNewSlots(unsigned int from,
                                             void* const* fillValue,
                                             unsigned int count)
{
    unsigned int end = from + count;
    if (end >= m_count)
        end = m_count;

    if (from < end)
    {
        void** const slots = (void**)m_storage.m_data;
        for (unsigned int i = from; i < end; ++i)
        {
            void* v = *fillValue;
            slots[i] = v;
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F49EF30 - byte-identical shape to `SBhPoTickRelaySlotArray::Grow`
//  (0x6F4AB050) - see spatialgrid.h's own header comment.
//
//  Written naked for the exact same one instruction
//  `CCellRecordArray::AppendRecords` above already documents needing it
//  for: the shipped `count == 0` answer is `lea eax,[edi+1]` off the
//  register that already holds `count`, and no C++ spelling of `count + 1`
//  reaches that rather than folding straight back to `mov eax,1`.
//  Everything else here is already the shipped stream:
//
//      if (count == 0) return count + 1;
//      unsigned int newTotal = m_count + count;
//      if (newTotal > m_capacity)
//      {
//          if (m_growBy == 0) return 0;
//          unsigned int overflow = (m_count - m_capacity) + count;
//          unsigned int grow = (m_growBy > overflow) ? m_growBy : overflow;
//          if (!m_storage.SetSize((m_capacity + grow) * 4, 1)) return 0;
//          m_capacity += grow;
//      }
//      unsigned int oldCount = m_count;
//      m_count = oldCount + count;
//      if (fillValue != 0) FillNewSlots(oldCount, fillValue, m_count);
//      return 1;
//----------------------------------------------------------------------------
__declspec(naked) int SGridRegistrationPtrArray::Grow(
        const void* /*fillValue*/, unsigned int /*count*/)
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
        mov     edx, [esi+14h]              // m_growBy
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
        add     ecx, ecx                    // * sizeof(void*) == 4
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
        mov     edx, [esp+10h]              // fillValue
        test    edx, edx
        lea     ecx, [eax+edi]
        mov     [esi+1Ch], ecx
        jz      short done
        push    ecx
        push    edx
        push    eax
        mov     ecx, esi
        call    SGridRegistrationPtrArray::FillNewSlots
    done:
        pop     ebx
        pop     edi
        mov     eax, 1
        pop     esi
        retn    8
    }
}

//----------------------------------------------------------------------------
//  0x6F49FE50 - see spatialgrid.h's own header comment for the full shape
//  and the +0x40 polarity note.
//
//  Written naked: the equivalent C++ below compiles correct but with a
//  thoroughly different register allocation across this function's own
//  eleven live values and five-deep branch nest (tried plain C++, tried
//  reusing the already-inline `CellAt` for the bounds check - neither
//  came within reach of the shipped schedule), so this is an exact
//  instruction-for-instruction transcription instead, the same choice
//  `CCellRecordArray::AppendRecords` and `SGridRegistrationPtrArray::Grow`
//  above already made for their own one small unreachable instruction -
//  just for the whole body here. Equivalent source, for reference:
//
//      int x = point->m_x;
//      if ((unsigned)x >= (unsigned)m_stride) return;
//      int y = point->m_y;
//      if ((unsigned)y >= (unsigned)m_rows) return;
//      int* cellPtr = m_cells + (m_stride * y + x);
//      if (cellPtr == 0) return;
//      int index = *cellPtr & kCellIndexMask;
//      if (index == kCellIndexNone) return;
//      int stampCollected = m_visitStamp + 1;
//      int stampSeen = stampCollected + 1;
//      m_visitStamp = stampSeen;
//      do {
//          SCellRecord* record = (SCellRecord*)m_records.m_data + index;
//          int link = record->m_link;
//          unsigned flag = (unsigned)link & kCellFlagMask;
//          index = link & kCellIndexMask;
//          if (flag != 0x02000000) {
//              CGridRegistration* owner = (CGridRegistration*)record->m_owner;
//              int stamp = owner->m_visitStamp;
//              if (stamp != -1 && (owner->m_moveFlags & 0x10000000) != 0 &&
//                  stamp != stampCollected && stamp != stampSeen) {
//                  if (flag == 0x01000000) {
//                      out->Grow(&owner, 1);
//                      owner->m_visitStamp = stampCollected;
//                  } else {
//                      owner->m_visitStamp = stampSeen;
//                  }
//              }
//          }
//      } while (index != kCellIndexNone);
//----------------------------------------------------------------------------
__declspec(naked) void CCellGrid::CollectRelated(
        const SCellPoint* /*point*/, SGridRegistrationPtrArray* /*out*/)
{
    __asm
    {
        mov     eax, [esp+4]                // point
        push    ebx
        mov     ebx, ecx                    // this
        mov     ecx, [eax]                  // x
        mov     edx, [ebx+3Ch]              // m_stride
        cmp     ecx, edx
        jnb     bail0
        mov     eax, [eax+4]                // y
        cmp     eax, [ebx+40h]              // m_rows
        jnb     bail0
        imul    eax, edx
        add     eax, ecx
        mov     ecx, [ebx+28h]              // m_cells
        lea     eax, [ecx+eax*4]
        test    eax, eax
        jz      bail0
        push    edi
        mov     edi, [eax]                  // cell word
        and     edi, 0FFFFFFh               // index
        cmp     edi, 0FFFFFFh
        jz      bail1
        mov     eax, [ebx+0B4h]             // m_visitStamp
        push    ebp
        lea     ebp, [eax+1]                // stampCollected
        lea     eax, [ebp+1]                // stampSeen
        push    esi
        mov     [ebx+0B4h], eax
        lea     esp, [esp+0]
    loopTop:
        mov     edx, [ebx+78h]              // m_records.m_data
        mov     eax, [edx+edi*8]            // record.m_link
        lea     ecx, [edx+edi*8]            // &record
        mov     edx, eax
        and     edx, 0FFFFFFh               // next index
        and     eax, 0FF000000h             // flag
        cmp     eax, 2000000h
        mov     edi, edx                    // index = next
        jz      loopNext
        mov     esi, [ecx+4]                // owner
        mov     ecx, [esi+38h]              // owner->m_visitStamp
        cmp     ecx, 0FFFFFFFFh
        mov     [esp+14h], esi              // spill owner into arg_0's slot
        jz      loopNext
        test    dword ptr [esi+40h], 10000000h
        jz      loopNext
        cmp     ecx, ebp
        jz      loopNext
        lea     edx, [ebp+1]
        cmp     ecx, edx
        jz      loopNext
        cmp     eax, 1000000h
        jnz     notNew
        mov     ecx, [esp+18h]              // out
        push    1
        lea     eax, [esp+18h]              // &owner (spilled slot, one more push deep)
        push    eax
        call    SGridRegistrationPtrArray::Grow
        mov     [esi+38h], ebp
        jmp     loopNext
    notNew:
        mov     [esi+38h], edx
    loopNext:
        cmp     edi, 0FFFFFFh
        jnz     loopTop
        pop     esi
        pop     ebp
    bail1:
        pop     edi
    bail0:
        pop     ebx
        retn    8
    }
}

//----------------------------------------------------------------------------
//  0x6F49FCD0 - register cell (row `y`, column `x`) for `owner`.
//
//  A layer-0 registration also sets the cell's occupancy bit.  The record
//  comes off the free chain if there is one and is appended to the pool
//  otherwise; either way it takes over the cell's chain head and the cell
//  word's low 24 bits are repointed at it, leaving the top eight alone.
//----------------------------------------------------------------------------
void CCellGrid::RegisterCell(int x, int y, CGridRegistration* owner,
                             unsigned int flag)
{
    int cell = m_stride * y + x;

    if (flag == 0)
        m_occupancy[(unsigned int)cell >> 5] |= 1u << (cell & 31);

    int record = m_freeRecord;
    int* head = &m_cells[cell];

    if (record == kCellIndexNone)
    {
        //  No free record: the new one goes on the end of the pool, so its
        //  index is the pool's current count.
        record = (int)m_records.m_count;

        SCellRecord fresh;
        fresh.m_link = (int)(flag & kCellFlagMask) | (*head & kCellIndexMask);
        fresh.m_owner = owner;
        m_records.AppendRecords(&fresh, 1);
    }
    else
    {
        SCellRecord* slot = (SCellRecord*)m_records.m_data + record;
        m_freeRecord = slot->m_link & kCellIndexMask;
        slot->m_link = (int)(flag & kCellFlagMask) |
                       ((*head & kCellIndexMask) & kCellIndexMask);
        slot->m_owner = owner;
    }

    m_liveRecords += 1;
    owner->m_cellCount += 1;
    *head = (*head & kCellFlagMask) | record;
}

//----------------------------------------------------------------------------
//  0x6F49FDA0 - clamp `box` to the grid's extent and register every cell of
//  the result, row by row.
//----------------------------------------------------------------------------
void CCellGrid::ForEachCellInBox(const SCellBox* box, CGridRegistration* owner,
                                 unsigned int flag)
{
    int x1 = (box->m_x1 < m_extent.m_x1) ? box->m_x1 : m_extent.m_x1;
    int y1 = (box->m_y1 < m_extent.m_y1) ? box->m_y1 : m_extent.m_y1;
    int x0 = (box->m_x0 > m_extent.m_x0) ? box->m_x0 : m_extent.m_x0;
    int y0 = (box->m_y0 > m_extent.m_y0) ? box->m_y0 : m_extent.m_y0;

    if (y0 >= y1)
        return;
    if (x0 >= x1)
        return;

    //  The guards above are signed; the walk itself is not.  That is not a
    //  tidy-up - the shipped code emits both, a signed `jge` pair for the two
    //  early-outs and `jb`/`jae` throughout the two loops, which is only what
    //  unsigned induction variables produce.
    for (unsigned int y = (unsigned int)y0; y < (unsigned int)y1; y++)
        for (unsigned int x = (unsigned int)x0; x < (unsigned int)x1; x++)
            RegisterCell((int)x, (int)y, owner, flag);
}

//----------------------------------------------------------------------------
//  0x6F49FF90 - move this registration to `newBox`.
//----------------------------------------------------------------------------
void CGridRegistration::UpdateBox(const SCellBox& newBox)
{
    SCellBox* current = &m_box;

    if (newBox.m_y0 == current->m_y0 && newBox.m_x0 == current->m_x0 &&
        newBox.m_y1 == current->m_y1 && newBox.m_x1 == current->m_x1)
        return;

    SCellBox overlap = IntersectCellBox(*current, newBox);

    if (overlap.m_y0 >= overlap.m_y1 || overlap.m_x0 >= overlap.m_x1)
    {
        //  Nothing in common: unregister the whole old rectangle and register
        //  the whole new one.
        m_grid->ForEachCellInBox(current, this, 0);
        m_grid->ForEachCellInBox(&newBox, this, 0x1000000);
    }
    else
    {
        //  They overlap, so only the two symmetric differences need touching;
        //  the cells in the overlap keep the registration they already have.
        SCellBox parts[4];

        unsigned int n = (unsigned int)SubtractCellBox(current, &overlap, parts);
        for (unsigned int i = n; i > 0; i--)
            m_grid->ForEachCellInBox(&parts[i - 1], this, 0);

        n = (unsigned int)SubtractCellBox(&newBox, &overlap, parts);
        for (unsigned int j = n; j > 0; j--)
            m_grid->ForEachCellInBox(&parts[j - 1], this, 0x1000000);
    }

    m_box = newBox;
}
