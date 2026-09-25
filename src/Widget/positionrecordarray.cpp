//============================================================================
//  0x6F4A5440 / 0x6F4A54A0 - the two members `SPositionRecordSink::SubmitRecord`
//  (positionrecordbuild.cpp) calls on its own `b` argument once `b != 0`: an
//  append to a growable array of 0x2C-byte `SPositionRecordBlock` records
//  kept at `b`'s own +0x108, and a second entry point that builds one of its
//  own locally before doing the same append.
//
//  `b`'s real class is not established.  It reaches this leaf pair only
//  through `CCameraWar3::Method_0x20`'s two `Position::RecordSlot8` forwarding
//  calls (docs/targets/CCameraWar3.md), but it is not `CCameraWar3` itself -
//  that class's own +0x114 is already ground-truthed as `m_farZ` (an
//  `SCameraProp`, GameUI/gameui.h), not this array's `m_data`.  So it stays
//  an unidentified receiver here, offset-addressed rather than guessed at.
//  See positionrecordarray.h for the struct layout.
//
//  The array's own grow/insert (0x6F4A4A40) stays thunked: it calls a second
//  leaf, 0x6F4A4390 - reached from the Position family batch instead
//  (reachable from both SmartPosition's and ProjectilePosition's own
//  subtrees; see docs/targets/Position-family.md), now reconstructed as
//  SPositionRecordArray::FillRange below.  Contrary to this comment's own
//  earlier guess ("shift existing elements when an insertion point is
//  given"), it is a broadcast, not a shift: its source pointer is reloaded
//  fresh every loop iteration while only the destination advances, so it
//  copies the *same* one record into every slot of the given range rather
//  than moving a block of them along by one.  GrowInsert itself is still
//  out of this batch's scope and stays a naked thunk to real shipped code.
//  It does call CMemBlock::SetSize (0x6F4B39C0, Pathfinding/cellbuffer.cpp)
//  for the raw storage resize, confirming the array's first 0x14 bytes are a
//  `CMemBlock` - the thunk's declared receiver type says so even though the
//  thunk body itself is unhooked shipped code.
//============================================================================
#include "positionrecordarray.h"

//----------------------------------------------------------------------------
//  0x6F4A4A40 - grow (if `count` more elements do not fit) and advance
//  `m_count` by `count`; `insertPtr` is the shift-from point for an in-place
//  insert when non-null.  `retn 8` - two stack arguments, `this` in ecx.
//  Naked: its own body reaches sub_6F4B39C0 (CMemBlock::SetSize,
//  reconstructed) and sub_6F4A4390 (not dump-confirmed in this batch), so it
//  stays real shipped code rather than a guessed-at 39-instruction rewrite.
//----------------------------------------------------------------------------
__declspec(naked) int SPositionRecordArray::GrowInsert(void*, unsigned int)
{
    __asm
    {
        mov     eax, 06F4A4A40h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F4A5440 - append one record: capture the array's current count as the
//  new element's index, grow the array by one (the shipped call's own
//  `insertPtr` argument is `block` - unread by GrowInsert unless the grow
//  actually shifts elements, so passing it through costs nothing to match),
//  then patch three of the new slot's fields.  `retn 8` - `block` and `d`.
//  Return value unread by the one caller in this batch (SubmitRecord always
//  returns its own `a`), so it is left as whatever GrowInsert yields - the
//  shipped stream never touches eax again after it.
//----------------------------------------------------------------------------
void SPositionRecordArraySink::AppendRecord(SPositionRecordBlock* block,
                                             int d)
{
    unsigned int index = m_array.m_count;

    m_array.GrowInsert(block, 1);

    //  Re-read `m_data` for each field store rather than caching the slot
    //  pointer once: the shipped stream reloads `[esi+114h]` three times
    //  with no intervening call, which is what three separate expressions
    //  through the member (not a cached local) produce.
    ((SPositionRecordBlock*)m_array.m_storage.m_data)[index].m_argC   = 0;
    ((SPositionRecordBlock*)m_array.m_storage.m_data)[index].m_zero1C = d;
    ((SPositionRecordBlock*)m_array.m_storage.m_data)[index].m_zero20 =
        block->m_argC;
}

//----------------------------------------------------------------------------
//  0x6F4A54A0 - build a local SPositionRecordBlock (the same 0x2C-byte
//  family positionrecordbuild.cpp's builder uses, populated differently:
//  `c` in the tag slot, a hard-coded 1 rather than an argument in `m_argC`,
//  `a` in `m_global`, `flags` in `m_zero10`) and append it.  `retn 0Ch` -
//  three stack arguments, `this` in ecx.  The field-store order below is the
//  shipped order: `c`, the five zeroed words, the two -1 sentinels (+0x28
//  before +0x24, same as positionrecordbuild.cpp's builder), then `a`, the
//  literal 1, then `flags` - not declaration order.
//----------------------------------------------------------------------------
int SPositionRecordArraySink::BuildAndInsertRecord(int a, int flags, int c)
{
    SPositionRecordBlock block;

    block.m_tag0       = c;
    block.m_global     = a;
    block.m_tag1       = 0;
    block.m_zero14     = 0;
    block.m_zero18     = 0;
    block.m_zero1C     = 0;
    block.m_zero20     = 0;
    block.m_sentinel28 = -1;
    block.m_sentinel24 = -1;
    block.m_argC       = 1;
    block.m_zero10     = flags;

    return m_array.GrowInsert(&block, 1);
}

//----------------------------------------------------------------------------
//  0x6F4A4390 - SPositionRecordArray::FillRange.  Clamp `start + count` to
//  the array's own m_count (the caller has already grown the array before
//  reaching here, so this is a downward clamp against whatever GrowInsert
//  actually committed to, not an upward one), then - only if `start` is
//  still inside the clamped range - broadcast-copy the 0x2C-byte record at
//  `record` into every slot from `start` up to that clamped end.  `retn
//  0Ch` - three stack arguments, `this` in ecx.
//
//  The struct assignment below (rather than a byte copy) is what the
//  shipped `mov ecx, 0Bh` / `rep movsd` (11 dwords = 0x2C bytes = one
//  SPositionRecordBlock) comes from: SPositionRecordBlock has no
//  user-declared special members, so its compiler-generated copy
//  assignment is exactly a flat dword-for-dword copy.
//----------------------------------------------------------------------------
void SPositionRecordArray::FillRange(unsigned int start, const void* record,
                                      unsigned int count)
{
    unsigned int end = count + start;
    end = (end < m_count) ? end : m_count;

    SPositionRecordBlock* dst = (SPositionRecordBlock*)m_storage.m_data;
    const SPositionRecordBlock* src = (const SPositionRecordBlock*)record;

    for (unsigned int i = start; i < end; ++i)
        dst[i] = *src;
}
