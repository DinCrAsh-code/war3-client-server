//============================================================================
//  0x6F2075E0 - the agile type id every hero carries: 'AHer' (0x41486572),
//  Warcraft III's own rawcode for the "Hero" ability.
//
//  Two instructions, and the same shape as the sixteen other agile-type-id
//  leaves this repo already has (misc_rawcode_getters.cpp).  Its own
//  translation unit because its one caller
//  (Player/playerherocollect.cpp) reaches it with a real call.
//============================================================================

unsigned int __cdecl HeroAgileTypeId()
{
    return 0x41486572;
}
