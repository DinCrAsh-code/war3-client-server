//============================================================================
//  0x6F4D61F0 - SAnimSequenceList::SelectMatchingSequences.
//
//  The first half of SpriteStartAnimation's choice: score every sequence in
//  the table against the caller's token list and leave `out` holding the
//  indices of the best-matching ones.
//
//  The scorer hands back two numbers per sequence - a *mask* of which tokens
//  matched, one bit each, and the sequence's own rank - and better means
//  "more tokens matched, and on a tie the smaller rank".  A sequence that
//  ties on both is appended to the list; one that beats it replaces the whole
//  list with itself.  That is why the list is emptied by a SetCount(1) and a
//  single store rather than by clearing and re-appending.
//
//  Two shapes here are the shipped code's and not a reading of it:
//
//    * the first test of a candidate is one `cmp mask, best` whose flags are
//      reused by both arms - the equal arm falls into the same `ja` the
//      unequal one jumps to (0x6F4D6274) - so the source is one comparison
//      chain and not two independent ifs;
//    * the shrink in the dedupe pass below is SAnimKeyArray's own SetCount
//      inlined, exactly as in Widget/animsequencefilter.cpp; there is no
//      out-of-line SetCount for this array in the image.
//
//  After the scoring pass come three rejections, all of which empty the list
//  rather than answering: the best mask does not carry the *last* token's
//  bit; or, when the caller asked for an exact match, the mask is not all
//  the tokens or the rank is not zero.
//
//  The dedupe pass keeps only the candidates that belong to the same
//  sequence group as the first one, removing the rest by overwriting each
//  with the array's last element - which is why its index does not advance
//  on the removal path.
//
//  Own translation unit: five real calls out of it.
//============================================================================
#include "animsequence.h"

void SAnimSequenceList::SelectMatchingSequences(unsigned int tokenCount,
                                                const unsigned int* tokens,
                                                int exactOnly,
                                                SAnimKeyArray* out)
{
    out->m_count = 0;

    unsigned int bestMask = 0;
    unsigned int bestRank = 0xFFFFFFFF;

    for (unsigned int i = 0; i < m_count; i++)
    {
        unsigned int mask = 0;
        unsigned int rank = 0;

        m_pRecords[i].ScoreAgainstTokens(tokenCount, tokens, &mask, &rank);

        if (mask == bestMask && rank == bestRank)
        {
            out->Append(1, &i);
            continue;
        }

        if (mask > bestMask || (mask == bestMask && rank < bestRank))
        {
            bestMask = mask;
            bestRank = rank;

            out->SetCount(1);
            out->m_keys[0] = i;
        }
    }

    if (out->m_count == 0)
        return;

    if ((bestMask & (1u << (tokenCount - 1))) == 0)
    {
        out->m_count = 0;
        return;
    }

    if (exactOnly != 0)
    {
        if (bestMask != (1u << tokenCount) - 1 || bestRank != 0)
        {
            out->m_count = 0;
            return;
        }
    }

    SAnimSequenceRecord* first = &m_pRecords[out->m_keys[0]];

    for (unsigned int j = 1; j < out->m_count; )
    {
        if (first->IsSameGroupAs(&m_pRecords[out->m_keys[j]]))
        {
            j++;
            continue;
        }

        out->m_keys[j] = *(unsigned int*)GetLastArrayElement_6F4D3740(out);
        out->SetCount(out->m_count - 1);
    }
}
