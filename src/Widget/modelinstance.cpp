//============================================================================
//  0x6F50C4A0 and 0x6F501D40 - the two model-instance entry points the
//  sprite setters reach.
//
//  IDA prints each of these as one function in two chunks: a two-instruction
//  null guard at the low address and the real body far away (0x6F5149B0 and
//  0x6F514950), reached by `jmp`.  They are written here as one function
//  each with an early return, not as a guard tail-calling a separate
//  helper: a separate helper is what produces that `jmp`, but it also puts
//  the body in its own PROC, and the shipped listing scores the two chunks
//  as one instruction stream.  The cost is the `jmp` itself; everything
//  after it lines up.
//
//  Both guards read the same +0x98 member, but they do NOT both work on what
//  it points at directly.  RescaleModelTimeline reads its fields straight off
//  that object (+0x54, +0x70, +0x74, +0xE4); MarkModelTrackDirty first steps
//  through the object's own +0x44 pointer and reads the track table and the
//  track blocks off *that*.  Getting this wrong is not cosmetic - see the
//  note on CModelTrackSet below.
//============================================================================
#include "sprite.h"

//  Storm ordinal 465 - set this thread's last-error code.
void __stdcall SErrSetLastError(unsigned int error);
//  Storm ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

//----------------------------------------------------------------------------
//  What CModelAnim::+0x44 points at: the model's track set.  This has to be
//  its own object, because the shipped body at 0x6F5149B0 opens with
//  `mov ecx, [ecx+44h]` and only then reads +0x08 and +0x18 - the descriptor
//  table and the block array belong to *this* object, not to CModelAnim.
//
//  Modelling them as CModelAnim's own +0x08/+0x18 (which is what this file
//  did until the miss was found) drops that one indirection and makes
//  MarkModelTrackDirty read the table pointer and the block base out of
//  whatever CModelAnim happens to hold there.  Its final statement is a
//  read-modify-write - `|= 2` - so the damage is not a bad read that faults
//  where it happens: it is a two-bit write through a pointer built out of
//  garbage, landing anywhere.  Live, that corrupted model state during the
//  attack-animation setup that reaches it through
//  StopSpriteAnimationsOfType, and the process died later inside the D3D8
//  driver with EIP on the stack.
//----------------------------------------------------------------------------
class CModelTrackSet
{
public:
    char          m_reserved00[0x08];
    void*         m_pTrackTable;       // +0x08  12-byte descriptors
    //  +0x0C - the track count.  SetTrackValue (0x6F5125A0) is what reaches
    //  it, as a `track >= m_trackCount` bounds check before indexing the
    //  descriptor table.
    unsigned int  m_trackCount;        // +0x0C
    //  +0x10 - one slot index per sequence, -1 where the sequence has none.
    //  StopModelSequence is what reaches it.
    int*          m_pSlotBySequence;   // +0x10
    char          m_reserved14[0x04];
    char*         m_pTrackBlocks;      // +0x18  0x8C bytes per entry
};

//  The object CModelInstance::+0x98 points at.  Only the members these two
//  functions touch are named.
class CModelAnim
{
public:
    //  0x6F5125A0 - resolve `track` to its block through the track set and
    //  push `value` at it, answering whether the track had a block at all.
    //  `retn 4`: a __fastcall member, the track in edx and the value pushed.
    int __fastcall SetTrackValue(int track, int value);

    char            m_reserved00[0x18];
    //  +0x18 - one entry per slot; StopModelSequence reads the entry's own
    //  flag byte at +0x24 and its position in the active list at +0x25.
    void**          m_pSlotEntries;    // +0x18
    char            m_reserved1C[0x20];
    //  +0x3C / +0x40 - the "this animation finished" callback and the
    //  context handed back with it.  SetModelAnimCallback is the only thing
    //  in this tree that writes them and nothing here reads them, so the
    //  names say what the one writer's caller installs (0x6F4D8EEC passes
    //  0x6F4D3EB0 and the sprite).
    void*           m_pFinishedCallback;   // +0x3C
    void*           m_pFinishedContext;    // +0x40
    CModelTrackSet* m_pTrackSet;       // +0x44
    char            m_reserved48[0x0C];
    unsigned char   m_flags54;         // +0x54  bit 0x80 gates the rescale
    char            m_reserved55[0x1B];
    unsigned int*   m_pKeyTimes;       // +0x70
    unsigned int    m_keyTimeCount;    // +0x74
    char            m_reserved78[0x5C];
    //  +0xD4 .. +0xE0 - the active-sequence list: 12 bytes per record, and
    //  the only thing in this tree that touches it is StopModelSequence,
    //  which takes one record out of the middle of it.
    struct SActiveSequenceArray
    {
        //  0x6F35C6D0 - grow the block to fit `count` records (each 12
        //  bytes / three floats, `NTempest::C3Vector`-shaped, matching
        //  SFilterSlotArray's own element), zero-filling everything past
        //  the old count; shrinking just lowers m_count.  `retn 4`.
        void SetCount(unsigned int count);
        //  0x6F5154C0 - once a removal has left the array with a new
        //  count: free everything and zero the object if that count is
        //  now 0, otherwise call SetAlloc(m_count) to drop the allocation
        //  down to exactly what's left (no data movement - the caller has
        //  already compacted the live records down to a contiguous run).
        //  `retn 0`.
        void Compact();
        //  0x6F33FC10 - SetCount's own bucket-size chooser: floor `count`
        //  to the nearest power of two (0 floors to 1) below the 0x15
        //  ceiling; at or above it, every call answers 0x15 and latches
        //  that into m_bucketCeiling so a later SetCount can skip
        //  recomputing it.  `retn 4`, thiscall (the growing caller passes
        //  `this` even though only the ceiling branch reads it).
        unsigned int PickBucketCount(unsigned int count);

        unsigned int m_alloc;         // +0x00
        unsigned int m_count;         // +0x04
        char*        m_data;          // +0x08  0x0C bytes per entry
        //  +0x0C - 0 until SetCount's own bucket count first saturates at
        //  the 0x15 ceiling (PickBucketCount is what writes it); read back
        //  by every later SetCount so the ceiling isn't recomputed.  This
        //  used to be modelled as CModelAnim's own m_reservedE0, one field
        //  over: the address is the same either way, but PickBucketCount
        //  addresses it through *this* array's `this`, and 0xD4 + 0x0C is
        //  0xE0 - so it belongs here.
        unsigned int m_bucketCeiling;  // +0x0C
    }               m_active;          // +0xD4
    int             m_timelineLength;  // +0xE4
};

//----------------------------------------------------------------------------
//  0x6F50C4A0 (+ 0x6F5149B0).  `track` indexes a table of 12-byte
//  descriptors whose third dword's low byte is a block index; 0xFF means
//  "no block".  The block itself is 0x8C bytes and the dirty flag is bit 1
//  of its own +0x0C.
//----------------------------------------------------------------------------
int __fastcall MarkModelTrackDirty(CModelInstance* self, int track)
{
    CModelAnim* anim = self->m_pAnim;
    if (anim == 0)
        return 0;

    //  The indirection the shipped body opens the far chunk with
    //  (`mov ecx, [ecx+44h]`).  Both reads below are off this object.
    CModelTrackSet* tracks = anim->m_pTrackSet;

    unsigned int block =
        *(unsigned char*)((char*)tracks->m_pTrackTable + track * 12 + 8);
    if (block == 0xFF)
        return 0;

    *(unsigned int*)(tracks->m_pTrackBlocks + block * 0x8C + 0x0C) |= 2;
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F50C3D0 (+ 0x6F513940).  Install the "animation finished" callback.
//  Nine instructions, of which the last three are the far chunk; with no
//  animation there is nothing to install and the callback is dropped
//  silently.
//----------------------------------------------------------------------------
void __fastcall SetModelAnimCallback(CModelInstance* self, void* callback,
                                     void* context)
{
    if (self->m_pAnim == 0)
        return;

    self->m_pAnim->m_pFinishedCallback = callback;
    self->m_pAnim->m_pFinishedContext = context;
}

//----------------------------------------------------------------------------
//  0x6F50C2E0.  Push a value at one animation track, and say whether it
//  landed.
//
//  The only one of this family that *reports* a missing animation rather
//  than answering zero quietly: it sets ERROR_INVALID_HANDLE (3) through
//  Storm's error channel first.  The `!= 0` on the answer is the shipped
//  `neg`/`sbb`/`neg` triple, which is how MSVC normalises a non-zero value
//  to 1 - the callee already answers 0 or 1, so the triple is the
//  compiler's, not a second test.
//----------------------------------------------------------------------------
int __fastcall SetModelTrackValue(CModelInstance* self, int track, int value)
{
    CModelAnim* anim = self->m_pAnim;
    if (anim == 0)
    {
        SErrSetLastError(3);
        return 0;
    }

    return anim->SetTrackValue(track, value) != 0;
}

//----------------------------------------------------------------------------
//  0x6F501D40 (+ 0x6F514950).  Rescale every non-zero key time from the old
//  timeline length to the new one.  The multiply is signed and the divide
//  unsigned, exactly as the shipped `imul` / `xor edx,edx` / `div` pair
//  says; the key-time count is reloaded from the frame inside the loop
//  because `div` clobbers edx, and the array base is reloaded through the
//  member every iteration rather than hoisted.
//----------------------------------------------------------------------------
int __fastcall RescaleModelTimeline(CModelInstance* self, int newLength)
{
    CModelAnim* anim = self->m_pAnim;
    if (anim == 0)
        return 0;

    if ((anim->m_flags54 & 0x80) == 0)
        return 0;

    int oldLength = anim->m_timelineLength;
    if (oldLength != newLength)
    {
        unsigned int count = anim->m_keyTimeCount;
        anim->m_timelineLength = newLength;
        for (unsigned int i = 0; i < count; ++i)
        {
            unsigned int* slot = &anim->m_pKeyTimes[i];
            if (*slot != 0)
                *slot = (unsigned int)((int)*slot * newLength) / (unsigned int)oldLength;
        }
    }
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F50C460 (+ 0x6F515810).  Take one sequence out of the animation's
//  active list.
//
//  The same two-chunk shape as the three above - a two-instruction null
//  guard at the low address and the body 0x94B0 bytes away - written as one
//  function with an early return, so the `jmp` and the guard's own `retn`
//  are the two instructions that cannot line up.
//
//  Answering 0 means "nothing to do": no animation, no slot for this
//  sequence (-1), the rescale gate down, or a slot whose entry is not
//  running (bit 1 of its +0x24 clear).  Otherwise the entry is marked
//  stopped - `(flags & 0x19) | 4`, which keeps three bits and replaces the
//  rest - its record is removed from the active list by sliding everything
//  after it down twelve bytes, the list is shortened and re-indexed, and,
//  when the animation carries bit 4, the sequence's key time is pinned to
//  the timeline length.
//
//  The slide is the shipped `memmove` and not a copy loop; both sides
//  canonicalise the call to `call SYM` (the shipped side goes through
//  `ds:__imp_memmove`), which is exactly why this file calls it through the
//  C declaration and never out of an `__asm` block - see
//  docs/notes/import-call-indirection.md.
//----------------------------------------------------------------------------
extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int n);

int __fastcall StopModelSequence(CModelInstance* self, unsigned int sequence)
{
    CModelAnim* anim = self->m_pAnim;
    if (anim == 0)
        return 0;

    //  One condition and not two statements: the shipped body jumps to the
    //  *same* `xor eax, eax` tail from both tests, which is what a
    //  short-circuiting `||` produces and what two separate `return 0`s
    //  does not.
    int slot = anim->m_pTrackSet->m_pSlotBySequence[sequence];
    if (slot == -1 || (anim->m_flags54 & 0x80) == 0)
        return 0;

    unsigned char* entry = (unsigned char*)anim->m_pSlotEntries[slot];

    unsigned char flags = entry[0x24];
    if ((flags & 2) == 0)
        return 1;

    entry[0x24] = (unsigned char)((flags & 0x19) | 4);

    //  The record's own position, and the index of the last live record.
    unsigned int position = entry[0x25];
    unsigned int last = anim->m_active.m_count - 1;

    if (position < last)
    {
        char* record = anim->m_active.m_data + position * 12;
        memmove(record, record + 12, (last - position) * 12);
    }

    anim->m_active.SetCount(last);
    anim->m_active.Compact();

    if ((anim->m_flags54 & 0x10) != 0)
        anim->m_pKeyTimes[slot] = anim->m_timelineLength;

    return 1;
}

//----------------------------------------------------------------------------
//  0x6F5125A0.  Resolve `track` to its block through the track set and
//  push `value` at it, answering whether the track had a block at all.
//  `retn 4`, __fastcall: the track arrives in edx, the value is the one
//  stack argument.
//
//  A missing track (index out of range, or a -1/0xFF slot) is reported
//  through Storm's error channel (ERROR_INVALID_HANDLE = 1) rather than
//  silently, unlike RescaleModelTimeline/MarkModelTrackDirty above - the
//  shipped body really does call SErrSetLastError(1) on this one path and
//  nowhere else in the family.
//----------------------------------------------------------------------------
//  0x6F511F10 - apply `value` to the resolved block, +8Ch stride, `edx`
//  still the track set from the caller's own `mov edx,[ecx+44h]` and never
//  reloaded here.  120 instructions of block/flag bookkeeping outside this
//  batch's scope; the real body runs (still a redirect, glue.cpp-shaped
//  args: `this` in ecx, the track set in edx, the block index and the
//  value as the two stack dwords, `retn 8`).
class CModelTrackBlockSetter
{
public:
    void __fastcall ApplyTrackValue(void* trackSet, unsigned int block, int value);
};

int __fastcall CModelAnim::SetTrackValue(int track, int value)
{
    CModelTrackSet* tracks = m_pTrackSet;

    //  Not a range-checked "invalid track" default: the shipped body
    //  shares one `cmp eax, 0FFh` between both paths, so an out-of-range
    //  `track` skips the descriptor lookup and falls into the comparison
    //  with the raw track index still in eax.  Reproduced rather than
    //  "fixed" - block only reads as 0xFF (and reports the error) when the
    //  caller's own out-of-range index happens to equal 0xFF; any other
    //  out-of-range track is applied as if it were the block index.
    unsigned int block = (unsigned int)track;
    if ((unsigned int)track < tracks->m_trackCount)
        block = *(unsigned char*)((char*)tracks->m_pTrackTable + track * 12 + 8);

    if (block == 0xFF)
    {
        SErrSetLastError(1);
        return 0;
    }

    ((CModelTrackBlockSetter*)this)->ApplyTrackValue(tracks, block, value);
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F33FC10.  See the declaration in CModelAnim::SActiveSequenceArray
//  above for the ceiling-latch behaviour.
//----------------------------------------------------------------------------
unsigned int __thiscall
CModelAnim::SActiveSequenceArray::PickBucketCount(unsigned int count)
{
    if (count < 0x15)
    {
        unsigned int floored = count;
        unsigned int cleared = floored & (floored - 1);
        while (cleared != 0)
        {
            floored = cleared;
            cleared = floored & (floored - 1);
        }
        if (floored < 1)
            floored = 1;
        return floored;
    }

    m_bucketCeiling = 0x15;
    return 0x15;
}

//----------------------------------------------------------------------------
//  0x6F303580 - the same growable-array-of-C3Vector reallocation
//  SFilterSlotArray::SetAlloc (GameUI/camerafilterslotarray.cpp) already
//  reconstructs: try an in-place SMemReAlloc first, and only on failure
//  allocate fresh and bytewise-copy the surviving records across.  Not a
//  second definition of that function - a second name for the identical
//  shipped address would be exactly the collision `thunk_inventory.py`
//  flags - but the *same* symbol, reached through a locally re-declared
//  copy of the class it already belongs to (MSVC mangles only the class
//  name, the way modelinstance_thunks.cpp's own CModelAnim re-declaration
//  does), because SActiveSequenceArray's own m_alloc/m_count/m_data layout
//  is the identical three words that function reads and writes.
//----------------------------------------------------------------------------
struct SFilterSlotArray
{
    void SetAlloc(unsigned int alloc);

    unsigned int m_alloc;
    unsigned int m_count;
    void*        m_data;
};

//----------------------------------------------------------------------------
//  0x6F35C6D0.  See the declaration above for the bucket-rounding shape:
//  grow to a multiple of PickBucketCount(count)'s answer (cached once it
//  has saturated at the 0x15 ceiling), then zero-fill every new record.
//----------------------------------------------------------------------------
void CModelAnim::SActiveSequenceArray::SetCount(unsigned int count)
{
    if (count <= m_count)
    {
        m_count = count;
        return;
    }

    if (count > m_alloc)
    {
        unsigned int bucket = m_bucketCeiling;
        if (bucket == 0)
            bucket = PickBucketCount(count);

        unsigned int newAlloc = count;
        unsigned int rem = count % bucket;
        if (rem != 0)
            newAlloc = count + (bucket - rem);

        ((SFilterSlotArray*)this)->SetAlloc(newAlloc);
    }

    for (unsigned int i = m_count; i < count; i++)
    {
        float* record = (float*)(m_data + i * 0x0C);
        if (record != 0)
        {
            record[0] = 0.0f;
            record[1] = 0.0f;
            record[2] = 0.0f;
        }
    }

    m_count = count;
}

//----------------------------------------------------------------------------
//  0x6F5154C0.  Once a removal has left the array with a new count: free
//  everything and zero the object if that count is now 0, otherwise shrink
//  the allocation down to exactly what's left through the same SetAlloc as
//  SetCount's own grow path - no data movement, because the caller has
//  already compacted the surviving records into a contiguous run before
//  calling this.  `retn 0`, thiscall, no stack arguments.
//----------------------------------------------------------------------------
void CModelAnim::SActiveSequenceArray::Compact()
{
    if (m_count == 0)
    {
        if (m_data != 0)
            SMemFree(m_data, "class NTempest::C3Vector", -2, 0);

        m_alloc = 0;
        m_count = 0;
        m_data = 0;
        return;
    }

    ((SFilterSlotArray*)this)->SetAlloc(m_count);
}
