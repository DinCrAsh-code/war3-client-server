//============================================================================
//  0x6F41BA80 - the player's total hero value, times ten.
//
//  Two ways to the same number, picked by `recollect`.  Non-zero walks the
//  ranked list afresh and sums each hero's experience; zero sums the cached
//  snapshot's own experience column instead, refreshing it first if it has
//  never been built.  Only the second is reachable from the end-of-game
//  blob, which passes 0.
//
//  The `* 10` at the end is one `lea` and one `add`, shared by both paths -
//  which is why they meet at a join rather than each returning.
//============================================================================
#include "playerheroeval.h"
#include "agent.h"

int SPlayerHeroes::TotalHeroValue(int recollect)
{
    int total = 0;

    if (recollect != 0)
    {
        TSGrowableArray<CAgentPtr> heroes = SelectHeroesByRank();

        unsigned int count = heroes.m_count;
        unsigned int index = 0;
        if (count > 0)
        {
            do
            {
                CAgent* unit = heroes.m_data[index].m_ptr;
                if (unit != 0)
                    total += HeroExperience(unit);
                index++;
            }
            while (index < count);
        }
    }
    else
    {
        unsigned int count = m_bestHeroes.m_count;
        if (count == 0)
        {
            RefreshBestHeroes();
            count = m_bestHeroes.m_count;
            if (count == 0)
                return 0;
        }

        const int* cursor = &m_bestHeroes.m_data[0].m_experience;
        do
        {
            total += *cursor;
            cursor += 3;
            count--;
        }
        while (count != 0);
    }

    return total * 10;
}
