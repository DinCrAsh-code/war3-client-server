//============================================================================
//  0x6F006840 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp): read command-line switch #6 as an
//  integer (default 1) and, if switch #5 is present too, forward both
//  through sub_6F57DA00 with a fixed 0x8000 flag - read as a tuning
//  option pair (a count/index plus its associated string), name not
//  established by this call tree.
//============================================================================

//  0x6F4C19D0 - already reconstructed (Misc/gamemain_small_wrappers.cpp).
//  Its own parameter is typed `const char*` (Storm ordinal 474's own
//  argument) but this call site's literal ecx values (6, then 5) are
//  passed straight through uninterpreted, same as
//  Game/gamemainreplaycheck.cpp's own use of it.
char* __fastcall CacheTextAndReturn(const char* text);

//  0x6F57DA00 - already reconstructed (Misc/gamemain_small_wrappers.cpp).
void __fastcall CacheTextAndTwoValues(const char* text, int handle, int extra);

extern "C" long __cdecl strtol(const char* str, char** endptr, int radix);

void __fastcall GameMainApplyCmdLineTuning()
{
    int value = 1;

    char* countArg = CacheTextAndReturn((const char*)(value + 5));
    if (countArg && *countArg)
        value = strtol(countArg, 0, 10);

    char* nameArg = CacheTextAndReturn((const char*)5);
    if (nameArg && *nameArg)
        CacheTextAndTwoValues(nameArg, value, 0x8000);
}
