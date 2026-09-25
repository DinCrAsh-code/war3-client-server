//============================================================================
//  0x6F26E210 - a race name to the id the rest of the game uses.
//
//  The table is a null-terminated run of {name, id} pairs at 0x6FA70FE8,
//  and its first entry is "nightelf".  Two `extern` arrays rather than one
//  array of pairs, because IDA gives the two words two separate symbols
//  (`off_6FA70FE8` and `dword_6FA70FEC`) and the canonicaliser compares
//  operand *symbols*: one array indexed `[i].m_id` would carry a `+4`
//  displacement past a name the target does not have there.  Same device
//  docs/msvc-vc8-idioms.md records for the three-byte-stride table, one
//  level up - each array's element is the full eight-byte stride, with the
//  half it does not own left unnamed.
//
//  A null name answers 7 rather than 0; the shipped code forms that
//  constant as `lea eax, [ecx+7]` off the register it has just proved is
//  zero, which has no source spelling and costs nothing but the one
//  instruction.
//============================================================================

//  Storm ordinal 508 - bounded, case-sensitive compare.  Declared exactly
//  as Containers/tshash.inl declares it so the two are one symbol.
int __stdcall Storm_508(const char* a, const char* b, unsigned int count);

struct SRaceNameSlot { const char* m_name; int m_unused; };
struct SRaceIdSlot   { int m_id;           int m_unused; };

extern const SRaceNameSlot g_raceNames[];     // off_6FA70FE8
extern const SRaceIdSlot   g_raceNameIds[];   // dword_6FA70FEC

int __fastcall RaceNameToId(const char* name)
{
    if (name == 0)
        return 7;

    //  A head-tested `while`, not a `for(;;)` with the exit at the bottom.
    //  MSVC rotates this one itself - the guard that falls out of the
    //  rotation is the shipped `cmp g_raceNames, esi` against the
    //  already-materialised zero index - where the bottom-exit spelling
    //  makes it *peel* the first iteration and emit the compare twice.
    //  Indexed, with no pointer cursor of its own: a named `slot` pointer
    //  gives MSVC a second induction variable to strength-reduce (`add
    //  esi, 8` beside the index) where the shipped code keeps only the
    //  index and re-forms the address with a `lea` each time round.
    int index = 0;
    while (g_raceNames[index].m_name != 0)
    {
        if (Storm_508(g_raceNames[index].m_name, name, 0x7FFFFFFF) == 0)
            return g_raceNameIds[index].m_id;

        index++;
    }

    return 0;
}
