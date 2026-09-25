//============================================================================
//  0x6F4D3CA0 - SAnimSequenceRecord::ScoreAgainstTokens.
//  0x6F4D3D40 - SAnimSequenceRecord::IsSameGroupAs.
//
//  The two fields these read inside the record's own 0x18-byte opaque head
//  (Widget/animsequence.h) are established here for the first time: a word
//  count at +0x04 and a pointer at +0x08 to that many "sequence word"
//  dwords, each an ordinary token id in the low bits with bit 31 marking it
//  optional/wildcard.  The remaining +0x00 and +0x0C..+0x18 bytes are still
//  untouched by anything in this call tree.
//
//  ScoreAgainstTokens walks the record's own words; for each one it scans
//  the caller's token list for a match and, if found, sets the
//  corresponding bit (counted down from tokenCount-1, so token 0 is the
//  *high* bit of the mask) in *outMask.  A word that never matches AND does
//  not carry the optional bit increments *outRank* - a required-word miss
//  count the caller uses to rank candidates.
//
//  IsSameGroupAs walks both records' word arrays in lockstep up to the
//  shorter length: two words "agree" if either carries the optional bit or
//  they are equal, and any mismatch anywhere - or a required (non-optional)
//  word left over past the shorter array's own length, on either side -
//  fails the whole comparison.  The two special-cased zero-length checks
//  at the top (both empty -> true) are the shipped code's, not an
//  afterthought: with `n1==0` the general loop below never executes and the
//  tail checks alone would already answer correctly, but the shipped body
//  spells the empty/empty case out explicitly rather than falling into that
//  loop.
//
//  Own translation unit: two real leaves.
//============================================================================
//  Not including animsequence.h: it declares its own (currently opaque)
//  copy of this same class, and a second, differently-shaped declaration of
//  the same struct name in one translation unit is a redefinition, not an
//  ODR-safe re-declaration the way separate translation units get.  MSVC
//  mangles a member only by the class *name*, so this local copy - carrying
//  the two newly-established fields - is the same symbol as animsequence.h's
//  own SAnimSequenceRecord in every other file.
struct SAnimSequenceRecord
{
    void ScoreAgainstTokens(unsigned int tokenCount, const unsigned int* tokens,
                            unsigned int* outMask, unsigned int* outRank);
    int  IsSameGroupAs(const SAnimSequenceRecord* other);

    char          m_reserved00[4];  // +0x00
    unsigned int  m_wordCount;      // +0x04
    unsigned int* m_pWords;         // +0x08
    char          m_reserved0C[0xC];// +0x0C .. +0x18
};

void SAnimSequenceRecord::ScoreAgainstTokens(unsigned int tokenCount,
                                             const unsigned int* tokens,
                                             unsigned int* outMask,
                                             unsigned int* outRank)
{
    *outMask = 0;
    *outRank = 0;

    for (unsigned int w = 0; w < m_wordCount; w++)
    {
        unsigned int word = m_pWords[w];
        int matched = 0;

        if (tokenCount != 0)
        {
            unsigned int bit = tokenCount - 1;
            for (unsigned int t = 0; t < tokenCount; t++, bit--)
            {
                if (word == tokens[t])
                {
                    *outMask |= (1u << bit);
                    matched = 1;
                }
            }
        }

        if (!matched && (int)word >= 0)
            (*outRank)++;
    }
}

int SAnimSequenceRecord::IsSameGroupAs(const SAnimSequenceRecord* other)
{
    unsigned int n1 = m_wordCount;
    unsigned int n2 = other->m_wordCount;

    if (n1 == 0 && n2 == 0)
        return 1;

    const unsigned int* w1 = m_pWords;
    const unsigned int* w2 = other->m_pWords;
    int same = (n1 != 0 && n2 != 0);

    if (n1 != 0)
    {
        while (n2 != 0)
        {
            if (!same)
                break;

            if (((*w1 | *w2) & 0x80000000u) == 0)
                same = (*w1 == *w2);

            n1--;
            n2--;
            w1++;
            w2++;

            if (n1 == 0)
                break;
        }

        if (n1 != 0 && (*w1 & 0x80000000u) != 0)
            same = 0;
    }

    if (n2 != 0 && (*w2 & 0x80000000u) != 0)
        same = 0;

    return same;
}
