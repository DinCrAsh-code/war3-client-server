//============================================================================
//  0x6F41B620 - the player's heroes, permuted into rank order.
//
//  Collect them, score each one, count how many of the others each one
//  beats, and use that count as its position - a rank-by-counting sort, not
//  a comparison sort: nothing is ever swapped, each hero is simply written
//  into `count - beats - 1`.  A tie on score is broken by the *lower*
//  handle winning, which is what makes the order deterministic across
//  machines - the same reason everything else in this module is careful.
//
//  Two or fewer heroes skip the whole thing: with one candidate the ranking
//  is a no-op and the list is copied straight out.
//
//  The score is `(level * 100000 + experience) * 10` plus the three
//  per-hero value terms, each asked for in "summary only" mode.  The
//  hundred thousand is what keeps level dominant over experience.
//
//  What this build cannot reproduce is the frame: VC8's unified `__try`
//  frame for the two arrays' destructors, which this repo's fixed
//  `/GS- /EHs-c-` never emits.
//============================================================================
#include "playerheroeval.h"
#include "agent.h"
#include "agiletype.h"
#include "game.h"

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

//  0x6F26C1C0 - Misc/misc_rawcode_getters.cpp's '+w3u'.
unsigned int GetUnitTypeFourCC();

//  The candidate, viewed for the four words the filter reads.  CAgent's own
//  handle pair is at +0x0C/+0x10 (agent.h); the type FourCC and the flags
//  word are CWidget's.
struct SHeroCandidate
{
    char          m_reserved000[0x30];
    unsigned int  m_typeId;      // +0x30
    char          m_reserved034[0x5C - 0x34];
    unsigned int  m_flags5C;     // +0x5C
};

//: The handle object's finer-grained tag a hero's own handle has to carry.
const int kAgentKindTagAgl = 0x2B61676C;

TSGrowableArray<CAgentPtr> SPlayerHeroes::SelectHeroesByRank()
{
    TSGrowableArray<CAgentPtr> result;
    TSGrowableArray<CAgentPtr> heroes = CollectHeroes();

    unsigned int count = heroes.m_count;

    if (count < 2)
    {
        result.m_alloc = 0;
        result.m_count = 0;
        result.m_data = 0;
        result.Assign(count, heroes.m_data);
        result.m_chunk = heroes.m_chunk;
    }
    else
    {
        TSFixedArray<HeroEval> evals;
        evals.m_alloc = 0;
        evals.m_count = 0;
        evals.m_data = 0;
        evals.SetCount(count);
        memset(evals.m_data, 0, evals.m_count * sizeof(HeroEval));

        //  Pass one: score every candidate that is still a live, unflagged
        //  hero.  A candidate that fails any test keeps the zeroed row it
        //  was given, which is what makes it lose every comparison below.
        unsigned int index = 0;
        unsigned int off = 0;
        if (count > 0)
        {
            do
            {
                CAgent* unit = heroes.m_data[index].m_ptr;
                if (unit != 0)
                {
                    CHandleObject* object =
                        LookupHandle(*(unsigned int*)((char*)unit + 0x0C),
                                     *(int*)((char*)unit + 0x10));
                    if (object != 0)
                    {
                        //  The tag test and the null test share one branch,
                        //  which is the checked-cast idiom
                        //  (docs/msvc-vc8-idioms.md).
                        int kindTag = object->m_kindTag;
                        int isAgl = (kindTag == kAgentKindTagAgl);
                        SItemHandleObject* checked =
                            isAgl ? (SItemHandleObject*)object : 0;
                        if (checked != 0 && checked->m_pendingFlag == 0)
                        {
                            SHeroCandidate* hero = (SHeroCandidate*)unit;
                            unsigned int typeId = hero->m_typeId;
                            if ((unsigned int)((typeId >> 24) + (unsigned int)-'A') <= 25
                                && (hero->m_flags5C & 0x40000000) == 0)
                            {
                                HeroEval* row =
                                    (HeroEval*)((char*)evals.m_data + off);
                                row->m_unit = unit;

                                int score = (HeroLevel(unit) * 100000
                                             + HeroExperience(unit)) * 10;
                                score += ((SHeroValueSource*)unit)->TermA(1);
                                score += ((SHeroValueSource*)unit)->TermB(1);

                                ((HeroEval*)((char*)evals.m_data + off))->m_score =
                                    ((SHeroValueSource*)unit)->TermC(1) + score;
                            }
                        }
                    }
                }

                index++;
                off += 12;
            }
            while (index < count);
        }

        //  Pass two: count, for every row, how many of the others it beats.
        if (count > 0)
        {
            unsigned int outer = 0;
            unsigned int outerLeft = count;
            do
            {
                HeroEval* a = (HeroEval*)((char*)evals.m_data + outer);
                int scoreA = a->m_score;

                unsigned int inner = 0;
                unsigned int innerLeft = count;
                do
                {
                    HeroEval* b = (HeroEval*)((char*)evals.m_data + inner);
                    int scoreB = b->m_score;

                    if (scoreA > scoreB
                        || (scoreA == scoreB
                            && *(unsigned int*)((char*)a->m_unit + 0x0C)
                               < *(unsigned int*)((char*)b->m_unit + 0x0C)))
                    {
                        a->m_beats++;
                    }

                    inner += 12;
                    innerLeft--;
                }
                while (innerLeft != 0);

                outer += 12;
                outerLeft--;
            }
            while (outerLeft != 0);
        }

        //  Pass three: write each candidate into the slot its rank names,
        //  keeping only the ones that really are units.
        if (count > 0)
        {
            unsigned int at = 0;
            unsigned int left = count;
            do
            {
                HeroEval* row = (HeroEval*)((char*)evals.m_data + at);
                unsigned int slot = count - row->m_beats;
                CAgent* unit = row->m_unit;
                CAgentPtr* dst = &heroes.m_data[slot - 1];

                CAgent* keep;
                if (unit != 0
                    && AgileTypeIsDerivedFrom(unit->GetAgileTypeId(),
                                              GetUnitTypeFourCC()))
                    keep = unit;
                else
                    keep = 0;

                if (dst->m_ptr != keep)
                {
                    if (dst->m_ptr != 0)
                    {
                        if (--dst->m_ptr->m_refcount == 0)
                            dst->m_ptr->ReleaseSelf();
                    }
                    if (keep != 0 && unit != 0)
                        unit->m_refcount++;
                    dst->m_ptr = keep;
                }

                at += 12;
                left--;
            }
            while (left != 0);
        }

        result.m_alloc = 0;
        result.m_count = 0;
        result.m_data = 0;
        result.Assign(heroes.m_count, heroes.m_data);
        result.m_chunk = heroes.m_chunk;

        if (evals.m_data != 0)
            SMemFree(evals.m_data, ".?AUHeroEval@@", -2, 0);
    }

    return result;
}
