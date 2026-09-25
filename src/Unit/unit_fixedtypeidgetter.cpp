//============================================================================
//  0x6F207660 - a two-instruction constant getter: no `this`, no
//  arguments, `retn 0`.  Returns the literal 0x58646973 - the four-char
//  code `'Xdis'` read forward ((0x58<<24)|(0x64<<16)|(0x69<<8)|0x73), the
//  same multi-char-literal shape unit.h's own slot-7 GetAgileTypeId
//  ('+w3u', misc_rawcode_getters.cpp) already established for this
//  family's type-id constants.  Its only caller in this dump
//  (0x6F19B800, below) feeds the result straight into
//  AgileTypeIsDerivedFrom (0x6F471910) as the "must derive from this"
//  type, so it reads as a fixed agile-type-id gate rather than a general
//  FourCC.
//
//  Kept in the same translation unit as its one caller rather than split
//  out: a separate TU keeps the shipped stream's real `call` to this
//  function, but costs more elsewhere in the caller's own register
//  allocation than it recovers (measured 25/42 combined vs 23/42 split,
//  0.617 vs 0.541) - the caller's own score is what this repo optimises
//  for, and the split version was strictly worse on it.
//============================================================================
unsigned int GetFixedAgileTypeId_58646973()
{
    return 0x58646973;
}

//============================================================================
//  0x6F19B800 - a type-checked smart-pointer setter: `this` (ecx) is the
//  holder (a raw `void*` slot, COM-refcounted through the same
//  decrement/vtable-slot-0 shape Misc/trivialaccessors_08.cpp's
//  RefHolder_* family and Unit/unit_clearmotionstate.cpp both use), one
//  stack argument (the candidate object).  `retn 4`.
//
//  Rejects a null candidate outright; otherwise accepts it only if its own
//  vtable slot 7 (GetAgileTypeId, the same slot CUnit's own returns
//  '+w3u' through) answers a type that AgileTypeIsDerivedFrom
//  (Agent/agiletype.h/.cpp) says derives from the fixed 0x58646973 id
//  above.  Releases whatever the holder used to point at (skipped
//  entirely when the new value is unchanged), addrefs the accepted
//  candidate, stores it, and returns `this`.
//
//  0.617 (25/42) IDENTICAL: every shipped instruction is present; the
//  gap is this build's optimiser inlining GetFixedAgileTypeId_58646973's
//  single-constant body into an immediate load (`mov edx,1482975603`)
//  where the shipped stream keeps it a real call, plus the ordinary
//  register-allocation follow-on that shifts (esi/edi swap which value
//  is "accepted" vs "candidate" at a few points).  A separate translation
//  unit (tried, see above) keeps the call real but loses more elsewhere;
//  no spelling in one TU stops MSVC inlining a single-constant-return
//  leaf into its only caller.
//
//  Reached only through 0x6F1A4C40, an `__except_handler4`-shaped SEH
//  wrapper this toolchain cannot reproduce (docs/msvc-vc8-idioms.md) -
//  left `pending` on its own account, but this callee stands alone.
//============================================================================
typedef unsigned int (__thiscall *GetAgileTypeIdFn)(void* self);
//  Agent/agiletype.cpp's own declaration - has to match exactly (MSVC
//  mangles the return/parameter types, so `int` vs `unsigned int` here
//  would link as two different symbols; see link_check.py's own note on
//  this class of defect).
int __fastcall AgileTypeIsDerivedFrom(unsigned int type, unsigned int base);
typedef void (__thiscall *RefDestroyFn)(void* self);

struct STypeCheckedRefHolder
{
    void* __thiscall Set(void* candidate);

    void* m_ref;   // +0x00
};

void* __thiscall STypeCheckedRefHolder::Set(void* candidate)
{
    void* accepted = 0;

    if (candidate)
    {
        unsigned int candidateType =
            ((GetAgileTypeIdFn)(*(void***)candidate)[7])(candidate);
        if (AgileTypeIsDerivedFrom(candidateType, GetFixedAgileTypeId_58646973()))
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
