//============================================================================
//  0x6F31C960 - resolve a sound label string against two lookup tables (a
//  primary one, then a shared fallback), reached through
//  CWidget::NegateAndForwardOffset's closure by way of the widget offset
//  vtable's "apply offset" overrides eventually driving a sound-play call.
//  The exact enclosing function (0x6F32F...) that calls this one is out of
//  this batch's scope; what is certain here is this leaf's own contract,
//  read straight off the dump: two hash-table lookups (`sub_6F3199F0`,
//  itself a `TSHashTable<T,K>`-shaped Find - mask at +0x24, bucket array at
//  +0x1C, same shape Containers/tshash.inl already documents - out of scope
//  and thunked), an out-parameter recording which table answered, and an
//  optional one-shot debug warning for a label neither table has.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  0x6F3199F0 - `retn 4`, __thiscall(ecx=table, stack=label): only one
//  register argument (the table itself, in ecx), so this is a real member
//  function - most likely TSHashTable<T,K>::Find - not a two-register
//  __fastcall.  Out of this batch's scope.
struct SSoundLabelTableThunk
{
    int __thiscall LookupSoundLabelTable(const char* label);
};
ADDR_THUNK(int __thiscall SSoundLabelTableThunk::LookupSoundLabelTable(
               const char*),
           0x6F3199F0)

//  0x6F4CC1E0 - `retn 0`, __cdecl variadic: format `fmt` with the trailing
//  arguments and forward to Storm's own logger (sub_6F4CC180, which itself
//  goes through vsnprintf and a /GS-cookied buffer this repo's fixed /GS-
//  build cannot reproduce).  Out of this batch's scope.
ADDR_THUNK(void __cdecl SLogWarningV(int level, const char* fmt, ...),
           0x6F4CC1E0)

//  Opaque: only the two tables' own addresses are ever taken here, never
//  their layout (sub_6F3199F0's own +0x1C/+0x24 reads are inside its own,
//  out-of-scope body).  Declared, not defined - the real objects live in
//  the shipped image at these addresses.
struct SSoundLabelTable;
extern SSoundLabelTable g_soundLabelTablePrimary;   //: 0x6FAB57A4
extern SSoundLabelTable g_soundLabelTableFallback;  //: 0x6FAB576C
//: 0x6FAB50D4 - bit 0 gates the "unrecognized sound label" warning; a
//  debug/verbose-logging flag byte, not something this tree ever sets.
extern unsigned char g_soundLabelWarnEnabled;

//----------------------------------------------------------------------------
//  0x6F31C960.  `retn 4`, __fastcall: the label in ecx, the "found in the
//  primary table" out-flag in edx, and one stack argument that suppresses
//  the warning when nonzero (every caller in this tree that reaches it
//  passes 0, i.e. "warn").
//----------------------------------------------------------------------------
int __fastcall ResolveSoundLabelFlag(const char* label, int* outFoundPrimary,
                                     int suppressWarning)
{
    *outFoundPrimary = 1;
    int found = ((SSoundLabelTableThunk*)&g_soundLabelTablePrimary)
                    ->LookupSoundLabelTable(label);
    if (found != 0)
        return found;

    *outFoundPrimary = 0;
    found = ((SSoundLabelTableThunk*)&g_soundLabelTableFallback)
                ->LookupSoundLabelTable(label);
    if (found != 0)
        return found;

    if (suppressWarning == 0 && (g_soundLabelWarnEnabled & 1) != 0)
        SLogWarningV(4, "Warning, unrecognized sound label %s!", label);

    return 0;
}
