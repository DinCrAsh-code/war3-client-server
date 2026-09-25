//============================================================================
//  0x6F26EB30 - a unit's experience, out of the record at +0x1F0.
//
//  Six instructions.  The null answer is the *fall-through* and it costs no
//  `xor`: the pointer that was just tested is already the zero being
//  returned, which is what makes this the "if (p != 0) return p->f; return
//  0" spelling rather than the guarded one (docs/msvc-vc8-idioms.md, "An
//  explicit `xor eax,eax` says which branch was written second" - here
//  there is none to explain).
//
//  Own translation unit: three functions in the hero cluster call it for
//  real, and six instructions is well inside what /Ob2 would inline.
//============================================================================

struct SUnitTypeHolder
{
    char m_reserved000[0x1F0];
    char* m_pRecord;      // +0x1F0
};

int __fastcall HeroExperience(void* unit)
{
    char* record = ((SUnitTypeHolder*)unit)->m_pRecord;
    if (record == 0)
        return 0;

    return *(int*)(record + 0x8C);
}
