//============================================================================
//  The animation sequence table a CSprite keeps at +0x30, and the candidate
//  list the two selection passes over it build.
//
//  Two receivers, one table.  SpriteStartAnimation (0x6F4D7E50) calls
//  0x6F4D61F0 with `[sprite+0x30] + 0x18` and 0x6F4EBA40 with `[sprite+0x30]`
//  itself, and both walk 0x1C-byte records: the first through its own +0x08
//  count and +0x0C pointer, the second through +0x24.  0x18 + 0x0C is 0x24,
//  so the two are the same array reached from one level apart, which is why
//  the list below is declared as a member of the table rather than as a
//  second table.
//============================================================================
#ifndef ANIMSEQUENCE_H
#define ANIMSEQUENCE_H

//----------------------------------------------------------------------------
//  The candidate list: the same four members Widget/animkeyarray.cpp
//  declares - declared again here rather than moved into that file, the way
//  every other user of the type in this repo declares its own copy - plus
//  the two resize members these two passes need.
//----------------------------------------------------------------------------
struct SAnimKeyArray
{
    //  0x6F4D3960 - Widget/animkeyarray.cpp.
    void SetAlloc(unsigned int newCount);
    //  0x6F4D38A0 - the growth granularity.  `retn 4`, and the shipped body
    //  never reads ecx (it opens `mov eax, [esp+4]`); it is still declared a
    //  member, because the shipped call sites set ecx and a __stdcall free
    //  function would not.  Still a redirect, in
    //  Widget/animsequencefilter.cpp beside the first of its two callers.
    unsigned int ComputeChunk(unsigned int count);
    //  0x6F4D5070 - append `count` keys read from `keys`, returning the
    //  array's own count *before* the append (every call site in this
    //  closure ignores it, but the shipped stream computes and returns it
    //  regardless).  `retn 8`.  Widget/animsequenceappend.cpp.
    unsigned int Append(unsigned int count, const unsigned int* keys);

    //  Inlined at both of its call sites, so it has no address of its own.
    //  Not Storm's TSGrowableArray::SetCount: there is no element
    //  construction or destruction (a key is a plain dword) and the chunk
    //  rounding only happens above two elements, because two or fewer live
    //  in the header's own inline storage - the rule animkeyarray.cpp
    //  records.
    __forceinline void SetCount(unsigned int count)
    {
        if (count > m_count && count > m_capacity)
        {
            unsigned int alloc = count;
            if (count > 2)
            {
                unsigned int chunk = ComputeChunk(count);
                unsigned int over = count % chunk;
                if (over != 0)
                    alloc = count + (chunk - over);
            }
            SetAlloc(alloc);
        }

        m_count = count;
    }

    unsigned int  m_capacity;   // +0x00
    unsigned int  m_count;      // +0x04
    unsigned int* m_keys;       // +0x08
    int           m_inline[2];  // +0x0C
};

//----------------------------------------------------------------------------
//  One sequence record: 0x1C bytes.  Two fields are reached from here - the
//  rank at +0x18, and whatever 0x6F4D3D40 compares two records on, which is
//  inside the first 0x18 and is not broken out.
//----------------------------------------------------------------------------
struct SAnimSequenceRecord
{
    //  0x6F4D3CA0 - score this record against the token list: how many
    //  tokens it matched, as a bit per token, and the record's own rank.
    //  `retn 10h` - four pushed dwords and `this` alone in ecx, which is
    //  what makes it a __thiscall member and not a __fastcall free function
    //  taking the record (that would take the count in edx and clean four
    //  bytes too few).  Widget/animsequencescore.cpp, which is also where
    //  the word-count/word-pointer fields this reads inside +0x00..+0x18
    //  are established.
    void ScoreAgainstTokens(unsigned int tokenCount,
                            const unsigned int* tokens,
                            unsigned int* outMask, unsigned int* outRank);
    //  0x6F4D3D40 - do these two records belong to the same sequence group?
    //  `retn 4`, same reasoning.  Widget/animsequencescore.cpp.
    int IsSameGroupAs(const SAnimSequenceRecord* other);

    char  m_reserved00[0x18];   // +0x00
    //  +0x18 - what the two selection passes rank a sequence on.  A
    //  normalised weight: the extreme search starts at 0.0f when the caller
    //  wants the highest and 1.0f when it wants the lowest, and the token
    //  scorer's own tie-break prefers the *smaller* one.  Nothing in this
    //  call tree says what the weight means, so the name says how it is used.
    float m_rank;               // +0x18
};

struct SAnimSequenceList
{
    //  0x6F4D61F0 - Widget/animsequenceselect.cpp.  `retn 10h`.
    void SelectMatchingSequences(unsigned int tokenCount,
                                 const unsigned int* tokens, int exactOnly,
                                 SAnimKeyArray* out);

    char                 m_reserved00[0x08]; // +0x00
    unsigned int         m_count;            // +0x08
    SAnimSequenceRecord* m_pRecords;         // +0x0C
};

struct SAnimSequenceTable
{
    //  0x6F4EBA40 - Widget/animsequencefilter.cpp.  `retn 8`.
    void KeepExtremeRankedSequences(int preferHighest,
                                    SAnimKeyArray* candidates);

    char              m_reserved00[0x18];   // +0x00
    SAnimSequenceList m_sequences;          // +0x18 .. +0x28
};

//  0x6F4D3740 - Misc/trivialpredicates.cpp: the address of the array's last
//  element, or null when it is empty.
void* __fastcall GetLastArrayElement_6F4D3740(const void* self);

#endif
