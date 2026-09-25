//============================================================================
//  CSimpleTop::Method_0x30 (vtable slot 12, 0x6F60C580) and the two small
//  SRenderBatch helpers it drives (0x6F60DF80, 0x6F60D0E0) -
//  docs/notes/pause-mouse-capture.md's last named lead in the search for
//  CSimpleTop::m_pCapturedFrame's write site.
//
//  Neither this function nor anything in its full depth-3 closure
//  (asm/sub_6F60C580_0x6F60C580_calltree_asm.md, 888 lines through 27
//  functions) ever references +0x16C - see frame.h's own comment on the
//  declaration for the rest of that finding.
//
//  Deliberately thunked: the qsort/Storm-import/render-device leaf
//  SDirtyBatchContext::ProcessDirtyRenderBatch (0x6F60D1B0, its own
//  separate texture/font closure) and the SBatchPtrArray grow-or-fail
//  helper EnsureCapacity
//  (0x6F60AF30, inline small-buffer-of-8 fast path plus Storm
//  alloc/free/realloc) - both declared in framethunks.h/.cpp.  The chunk
//  computation ahead of EnsureCapacity is *not* thunked: it is genuinely
//  inline in this function's own byte range (no separate `call`), and it
//  is the same "highest power of two below 64, then round up to a
//  multiple of it" shape CFramePriorityArray::ComputeChunk
//  (framepriorityalloc.cpp, this same module) already has real source for.
//============================================================================
#include "frame.h"
#include "framethunks.h"

extern "C" void __cdecl qsort(void* base, unsigned int num, unsigned int size,
                              int (__cdecl *cmp)(const void*, const void*));

//  ".?AURENDERBATCHENTRY@@" - confirmed live (`ida_query.py strings
//  AURenderBatch`, 0x6FA9AA8C), not the truncated IDA symbol name
//  (`aAurenderbatche`) this dump's own comments carry.  The same tag
//  0x6F60AF30's own three internal Storm calls use (thunked, so not
//  reproduced here) and 0x6F60CB10's own qsort comparator's element type -
//  m_14's sub-array is really an array of RENDERBATCHENTRY, 0x1C bytes
//  each.
static const char kRenderBatchEntryTypeName[] = ".?AURENDERBATCHENTRY@@";

//----------------------------------------------------------------------------
//  0x6F60CB10 - qsort's own CompareFunction for the RENDERBATCHENTRY
//  sub-array: the difference of each entry's own leading int (confirmed
//  live, `ida_query.py bytes 0x6F60CB10 16`: `mov eax,[a]; mov eax,[eax];
//  mov ecx,[b]; sub eax,[ecx]; retn`).
//----------------------------------------------------------------------------
int __cdecl CompareRenderBatchEntries(const void* a, const void* b)
{
    return *(const int*)a - *(const int*)b;
}

//----------------------------------------------------------------------------
//  0x6F60AF30's own caller-side chunk computation, inlined at its one call
//  site below rather than kept as a separate function that would need its
//  own address to be real - see the file's header comment.  Bodily
//  identical to CFramePriorityArray::ComputeChunk (framepriorityalloc.cpp)
//  with the write-back into m_chunk dropped: SBatchPtrArray has no such
//  field (see frame.h's own comment on it), so every grow recomputes.
//----------------------------------------------------------------------------
static unsigned int ComputeBatchPtrChunk(unsigned int count)
{
    if (count < 64)
    {
        unsigned int lower = (count - 1) & count;
        if (lower != 0)
        {
            do
            {
                count = lower;
                lower = (count - 1) & count;
            }
            while (lower != 0);
        }

        if (count >= 1)
            return count;

        return 1;
    }

    return 64;
}

//----------------------------------------------------------------------------
//  0x6F60DF80 - drop this batch's own resource reference (if any) and clear
//  both count fields.
//----------------------------------------------------------------------------
void SRenderBatch::Reset()
{
    m_10 = 0;
    if (m_08 != 0)
    {
        ReleaseBatchResource(m_08);
        m_08 = 0;
    }
    m_04 = 0;
}

//----------------------------------------------------------------------------
//  0x6F60D0E0 - qsort the +0x14 sub-array once it holds more than one
//  entry.
//----------------------------------------------------------------------------
void SRenderBatch::SortIfNeeded()
{
    if (m_10 > 1)
        qsort(m_14, m_10, 0x1C, CompareRenderBatchEntries);
}

//----------------------------------------------------------------------------
//  Slot 12 (+0x30).  See frame.h's own comment on the declaration for the
//  full per-step breakdown; this is that breakdown as code.
//----------------------------------------------------------------------------
void CSimpleTop::Method_0x30()
{
    int anyDirty = 0;

    //  Unsigned throughout - `i` genuinely is (this is exactly the classic
    //  signed-vs-unsigned-division tell, docs/msvc-vc8-idioms.md: a signed
    //  `int i` divided by 5 compiles to `imul`+`sar`+sign-fixup, where the
    //  shipped stream's own magic-multiply-by-0xCCCCCCCD/`shr 2` is the
    //  unsigned form).
    for (unsigned int i = 0; i < 50; ++i)
    {
        if (m_dirty[i] != 0)
        {
            unsigned int group = i / 5;
            int lastAnswerWasZero = 0;
            anyDirty = 1;
            unsigned int col = i - group * 5;

            //  Reset this cell's own batch *before* the region walk, not
            //  after - a real call this session's first pass missed
            //  entirely.  Confirms m_04 (Method_0x30's own "has pending
            //  work" test, frame.h's own comment on SRenderBatch) really is
            //  cleared here and can only be set again by whatever a
            //  region's own Method_0x3C override does to the batch pointer
            //  it is handed below.
            m_batches[i].Reset();

            int raw = m_frameLayers[group].m_terminator.m_prevlink;
            while (raw > 0)
            {
                REGIONNODE* r = (REGIONNODE*)raw;
                if (((CSimpleFrame*)r->m_pRegion)->Method_0x3C(col, &m_batches[i]) == 0)
                    lastAnswerWasZero = 1;
                raw = r->m_link.m_prevlink;
            }

            m_batches[i].SortIfNeeded();
            m_dirty[i] = lastAnswerWasZero;
        }
    }

    if (!anyDirty)
        return;

    if (m_dirtyBatches.m_alloc > 8)
        SMemFree(m_dirtyBatches.m_data, kRenderBatchEntryTypeName, -2, 0);
    m_dirtyBatches.m_alloc = 0;
    m_dirtyBatches.m_count = 0;
    m_dirtyBatches.m_data = 0;

    for (int i = 0; i < 50; ++i)
    {
        if (m_batches[i].m_04 != 0)
        {
            unsigned int newCount = m_dirtyBatches.m_count + 1;
            if (newCount > m_dirtyBatches.m_alloc)
            {
                unsigned int roundedCount = newCount;
                if (newCount > 8)
                {
                    unsigned int chunk = ComputeBatchPtrChunk(newCount);
                    unsigned int over = newCount % chunk;
                    if (over != 0)
                        roundedCount = newCount + (chunk - over);
                }
                m_dirtyBatches.EnsureCapacity(roundedCount);
            }

            m_dirtyBatches.m_data[m_dirtyBatches.m_count] = &m_batches[i];
            m_dirtyBatches.m_count++;
        }
    }

    for (unsigned int i = 0; i < m_dirtyBatches.m_count; ++i)
        m_8F8.ProcessDirtyRenderBatch(m_dirtyBatches.m_data[i]);
}
