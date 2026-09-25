//============================================================================
//  0x6F0D8FA0 / 0x6F275730 - a second instantiation of the fixed-agile-
//  type-id "type-checked smart-pointer setter" Unit/unit_fixedtypeidgetter
//  .cpp already establishes for 0x6F19B800/0x6F207660 - same layout, same
//  algorithm, different required type id (0x414E6275 here vs 0x58646973
//  there).
//
//  Kept in one translation unit for the same measured reason that file's
//  own header gives (25/42 combined vs 23/42 split for the sibling): a
//  same-TU getter is small enough that this build's optimiser inlines its
//  single-constant body into the caller's own `mov edx, <imm32>` where the
//  shipped stream keeps a real `call`, and that costs less than losing a
//  translation unit's worth of the caller's own register allocation to a
//  do-nothing split.
//============================================================================

//  0x6F0D8FA0 - `retn` bare, no `this`, no arguments.
unsigned int GetFixedAgileTypeId_414E6275()
{
    return 0x414E6275;
}

//  Agent/agiletype.cpp's own declaration - has to match exactly (see
//  Unit/unit_fixedtypeidgetter.cpp's own note on why).
int __fastcall AgileTypeIsDerivedFrom(unsigned int type, unsigned int base);

typedef unsigned int (__thiscall *GetAgileTypeIdFn)(void* self);
typedef void (__thiscall *RefDestroyFn)(void* self);

//  0x6F275730 - `retn 4`.  Same shape as STypeCheckedRefHolder::Set
//  (Unit/unit_fixedtypeidgetter.cpp), a different holder instance so it
//  gets its own struct name here.
struct SAgent4TypeCheckedRefHolder_6F275730
{
    void* __thiscall Set(void* candidate);
    void* m_ref;   // +0x00
};

void* __thiscall SAgent4TypeCheckedRefHolder_6F275730::Set(void* candidate)
{
    void* accepted = 0;

    if (candidate)
    {
        unsigned int candidateType =
            ((GetAgileTypeIdFn)(*(void***)candidate)[7])(candidate);
        if (AgileTypeIsDerivedFrom(candidateType, GetFixedAgileTypeId_414E6275()))
            accepted = candidate;
    }

    void* old = m_ref;
    if (old != accepted)
    {
        if (old != 0 && --*(int*)((char*)old + 4) == 0)
            ((RefDestroyFn)(*(void***)old)[0])(old);

        if (accepted != 0 && candidate != 0)
            ++*(int*)((char*)candidate + 4);

        m_ref = accepted;
    }

    return this;
}
