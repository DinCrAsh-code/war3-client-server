//============================================================================
//  CAbility depth<=5 closure verification pass (2026-09-02): the last three
//  addresses cability_depth5_closure_worklist.json's own walk of every real
//  CAbility vtable slot's callee closure still had no funcmap.py entry for -
//  all three genuinely out of scope (a target-flag string tokenizer, a
//  Storm-backed growable int array's own reserve helper, and one more
//  member of SGenericFieldValueTable, abilitygenericfieldrecord.cpp's own
//  family, one level deeper than that file's own four).  Naked redirects,
//  not reconstructions: every argument count is the shipped call site's
//  own push count, read off each callee's own `retn` in its asm/ dump
//  (tools/dumpfn.py), not inferred.
//============================================================================

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F255250 (`retn` bare, 0 stack args) - ecx = a "target flags" string
//  (space/comma/tab/dash-separated tokens split with Storm_504, each
//  matched against a fixed table at off_6FA6F478 with Storm_509 and OR'd
//  into a running bitmask from dword_6FA6F47C); returns that bitmask.
ADDR_THUNK(unsigned int __fastcall ParseTargetFlagString(const char* flagsText), 0x6F255250)

//  0x6F25A760 (`retn 4`) - ecx = a Storm-backed growable "I_4"-typed int
//  array header (count/capacity/data triple at +0/+4/+8), one stack arg
//  (the new element count): grows the backing store through Storm_401/
//  Storm_403/Storm_405 and copies the live elements across, matching the
//  reserve step every TSGrowableArray<T>::SetAlloc instantiation elsewhere
//  in this repo performs for its own T, just through Storm's own array
//  primitives instead of tsarray.inl's.  A member of a stub class rather
//  than a free function - the same opaque-receiver idiom
//  abilitygenericfieldrecord.cpp's own SGenericValueDecodeTarget uses -
//  since this toolchain rejects __thiscall on a free function.
class STargetFlagIndexArray
{
public:
    void Reserve(unsigned int newCount);
};
ADDR_THUNK(void STargetFlagIndexArray::Reserve(unsigned int), 0x6F25A760)

//  0x6F25B200 (`retn 4`) - one level deeper than
//  abilitygenericfieldrecord.cpp's own SGenericFieldValueTable family
//  (Method_6F25B230/2A0/340/3A0): `this`->+0x08 gates the whole answer to 0
//  the same way Method_6F25B230 does, decodes its one stack arg's tag
//  through sub_6F25B100 (DecodeSingleTag, abilitygenericfieldrecord_thunks.cpp)
//  and tail-jumps into a fifth, undumped decode target member
//  (sub_6F6FA490) - out of scope, no dump anywhere in this repo.
class SGenericFieldValueTable5th
{
public:
    void* Method_6F25B200(unsigned int tag);
};
ADDR_THUNK(void* SGenericFieldValueTable5th::Method_6F25B200(unsigned int), 0x6F25B200)

#undef ADDR_THUNK
