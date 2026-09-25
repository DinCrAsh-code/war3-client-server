//============================================================================
//  0x6F40F6F0 / 0x6F40F770 / 0x6F40F710 and 0x6F40F760 / 0x6F40F7E0 /
//  0x6F40F750 / 0x6F40F7D0 - seven published counters on the player record,
//  each read through its own {handle, typeTag} pair by
//  HandleRefFieldOwner_6F473170::QueryField78 (Misc/misc_handle_lookups.cpp).
//
//  Two shapes, and the difference is real:
//
//    * the three at +0x130 / +0x140 / +0x150 divide the answer by ten,
//      unsigned (`mov eax, 0CCCCCCCDh` / `mul` / `shr edx, 3`), so they are
//      a `call` followed by the magic-multiply;
//    * the four at +0x190 / +0x1A0 / +0x1B0 / +0x1C0 hand it back
//      untouched, which /O2 turns into `add ecx, <offset>` / `jmp` - the
//      member tail call docs/msvc-vc8-idioms.md records, and it only comes
//      out that way because QueryField78's definition is in another
//      translation unit.
//
//  The division is what types the field: a signed `/ 10` would have emitted
//  the `cdq` / `sub` / `sar` round-toward-zero sequence instead.
//============================================================================
#include "playerscore.h"

unsigned int SPlayerScore::QueryTenth130()
{
    return (unsigned int)m_counter130.QueryField78() / 10;
}

unsigned int SPlayerScore::QueryTenth140()
{
    return (unsigned int)m_counter140.QueryField78() / 10;
}

unsigned int SPlayerScore::QueryTenth150()
{
    return (unsigned int)m_counter150.QueryField78() / 10;
}

int SPlayerScore::QueryField190() { return m_counter190.QueryField78(); }
int SPlayerScore::QueryField1A0() { return m_counter1A0.QueryField78(); }
int SPlayerScore::QueryField1B0() { return m_counter1B0.QueryField78(); }
int SPlayerScore::QueryField1C0() { return m_counter1C0.QueryField78(); }
