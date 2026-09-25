//============================================================================
//  0x6F41B8E0 - refresh the cached best-hero snapshot at +0x2E4.
//
//  The snapshot is only rebuilt when it has actually gone stale, and the
//  staleness test is deliberately one-sided: the cache is kept if it has the
//  same number of heroes as the fresh list *and* no hero's level or
//  experience has gone **up**.  A level or an experience that went down
//  leaves the cache alone, which is what makes this a high-water mark rather
//  than a mirror.
//
//  The "nothing to compare against" case - an empty cache - is folded into
//  the same flag: `(cached == 0)` is computed branchlessly up front and is
//  what forces the rebuild the first time round.
//
//  What this build cannot reproduce is the frame: VC8's unified `__try`
//  frame for the hero list's destructor.
//============================================================================
#include "playerheroeval.h"
#include "agent.h"
#include "storm.h"

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

void SPlayerHeroes::RefreshBestHeroes()
{
    TSGrowableArray<CAgentPtr> heroes = SelectHeroesByRank();

    unsigned int cached = m_bestHeroes.m_count;
    unsigned int fresh = heroes.m_count;

    //  `(cached == 0) ? 1 : 0`, branchlessly - the shipped
    //  `cmp edx,ebp` / `sbb eax,eax` / `add eax,1` against a zeroed edx.
    int stale = (0u < cached) ? 0 : 1;

    if (cached != 0)
    {
        if (fresh == cached)
        {
            unsigned int index = 0;
            unsigned int off = 0;
            if (cached > 0)
            {
                for (;;)
                {
                    CAgent* unit = heroes.m_data[index].m_ptr;

                    BestHeroData* row =
                        (BestHeroData*)((char*)m_bestHeroes.m_data + off);
                    if ((unsigned int)HeroLevel(unit) > (unsigned int)row->m_level)
                    {
                        stale = 1;
                        break;
                    }

                    row = (BestHeroData*)((char*)m_bestHeroes.m_data + off);
                    if ((unsigned int)HeroExperience(unit) > (unsigned int)row->m_experience)
                    {
                        stale = 1;
                        break;
                    }

                    index++;
                    off += 12;
                    if (index >= cached)
                        break;
                }
            }
        }
        else if (fresh > cached)
        {
            stale = 1;
        }
    }

    if (stale != 0)
    {
        if (fresh != m_bestHeroes.m_count)
        {
            if (fresh == 0)
            {
                if (m_bestHeroes.m_data != 0)
                    SMemFree(m_bestHeroes.m_data, ".?AUBestHeroData@@", -2, 0);

                m_bestHeroes.m_alloc = 0;
                m_bestHeroes.m_count = 0;
                m_bestHeroes.m_data = 0;
            }
            else
            {
                m_bestHeroes.SetAlloc(fresh);
                m_bestHeroes.m_count = fresh;
            }
        }

        memset(m_bestHeroes.m_data, 0,
               m_bestHeroes.m_count * sizeof(BestHeroData));

        unsigned int index = 0;
        unsigned int off = 0;
        if (fresh > 0)
        {
            do
            {
                CAgent* unit = heroes.m_data[index].m_ptr;

                ((BestHeroData*)((char*)m_bestHeroes.m_data + off))->m_typeId =
                    *(unsigned int*)((char*)unit + 0x30);

                BestHeroData* row =
                    (BestHeroData*)((char*)m_bestHeroes.m_data + off);
                row->m_level = HeroLevel(unit);

                row = (BestHeroData*)((char*)m_bestHeroes.m_data + off);
                row->m_experience = HeroExperience(unit);

                index++;
                off += 12;
            }
            while (index < fresh);
        }
    }
}
