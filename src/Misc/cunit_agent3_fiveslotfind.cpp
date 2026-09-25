//============================================================================
//  0x6F207CC0 - `retn 4`: linear-search a fixed five-element key array at
//  `this+0xF4` for `key`, and return the parallel value at the same index
//  out of `this+0x10C`, or -1 if none matched.
//
//  Not CUnit's own layout: CUnit's own +0xF0..+0xFC is `m_reservedF0`
//  (0x0C bytes, unit.h) immediately followed by the SUnitTrackedRef at
//  +0xFC, which a five-dword array starting at +0xF4 would run straight
//  through - so this call tree's receiver is some other class entirely.
//  Nothing here names it, so it stays a generic offset-addressed struct
//  rather than a guessed CUnit member.
//============================================================================

struct SFiveSlotKeyValueTable
{
    //  0x6F207CC0
    int FindValue(int key);

    char m_reserved000[0xF4];
    int  m_keys[5];      // +0xF4
    char m_reserved108[0x10C - 0x108];
    int  m_values[5];    // +0x10C
};

int SFiveSlotKeyValueTable::FindValue(int key)
{
    for (int i = 0; i < 5; i++)
    {
        if (m_keys[i] == key)
            return m_values[i];
    }
    return -1;
}
