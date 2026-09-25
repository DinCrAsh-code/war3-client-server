//============================================================================
//  0x6F2D6DA0 - SAbilityHostRecord::TestOrArmFlagBit20 (abilityhostrecord.h).
//
//  If the world singleton's own +0x3E0 flag is set, force-set bit 20 of
//  this record's own +0x2D0 and report true unconditionally; otherwise
//  report whether bit 20 of +0x2D0 is currently clear.  A sticky one-shot
//  gate: once the world flag has been observed set, every later call
//  reports true too (the bit it forced stays set).
//============================================================================
#include "abilityhostrecord.h"

extern void* g_unk6FAB65F4;

struct SWorldFlagsHolder_6F2D6DA0
{
    char m_reserved[0x3E0];
    int  m_flag3E0;
};

int SAbilityHostRecord::TestOrArmFlagBit20()
{
    if (((SWorldFlagsHolder_6F2D6DA0*)g_unk6FAB65F4)->m_flag3E0 != 0)
    {
        m_flags2D0 |= 0x100000;
        return 1;
    }
    return !((m_flags2D0 >> 0x14) & 1);
}
