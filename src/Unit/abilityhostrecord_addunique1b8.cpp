//============================================================================
//  0x6F2D5330 - SAbilityHostRecord::AddUniqueTo1B8Set (abilityhostrecord.h).
//============================================================================
#include "abilityhostrecord.h"

void SAbilityHostRecord::AddUniqueTo1B8Set(unsigned int value)
{
    unsigned int* base = m_set1B8;
    unsigned int* scan = base;
    for (int count = 0; count < 4; count++)
    {
        if (*scan == value)
            return;
        scan++;
    }

    unsigned int* freeSlot = base;
    for (int count = 0; count < 4; count++)
    {
        if (*freeSlot == 0xFFFFFFFFu)
        {
            *freeSlot = value;
            return;
        }
        freeSlot++;
    }
}
