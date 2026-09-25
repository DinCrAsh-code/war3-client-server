//============================================================================
//  0x6F7127A0 - the generic SLK/typed-field registration engine every
//  CWar3SlkDatabase-derived sibling's own SetRecordSet override
//  (Item/itemdatabase.h, Game/upgradedatabase.h, Unit/destructabledatabase.h)
//  calls once per named field it wires up - checksum-provider registry
//  batch (see docs/notes/checksum-provider-registry.md).  ~180
//  instructions: a five-argument thiscall (`this` = a lazily-created field
//  pool object, then name/type/zero/out/zero on the stack, `retn 14h`)
//  that resolves `name` against the pool, decodes it through an 11-case
//  switch on the field's own type tag, formats a fallback name with
//  Storm_578 ("%s%d") when the direct lookup misses, and writes the
//  resolved {index, flags, extra} triple through the caller's own `out`
//  pointer.  No `push -1`/SEH_.../`fs:0` unified-EH prologue (just
//  `mov eax, 1040h` / `call __alloca_probe` and the plain `/GS`
//  cookie-into-a-stack-slot this build's own `/GS-` disables at the
//  C++-source level) - not blocked by the toolchain the way
//  docs/msvc-vc8-idioms.md's __CxxFrameHandler3 frame is, just genuinely
//  large: its own eleven further callees
//  (sub_6F6F3FC0/6F706BF0/6F6F1A20/6F711720/6F6F3AB0/6F6F3700/6F6FA9C0/
//  6F70ED80, Storm_509/506/507/590) are a whole further module of their
//  own, unrelated to the checksum-provider registry this batch is about.
//  Thunked rather than reconstructed - a real, valuable, well-scoped
//  follow-up for a dedicated batch, not a boring MSVC internal.
//============================================================================

//  Own receiver type purely for the calling convention - see
//  Item/itemdatabase.h's own note on why this family's field-pool objects
//  stay opaque here.
class SFieldRegistrationPool
{
public:
    //  0x6F7127A0 - `retn 14h`.  `out` points at a caller-owned, zeroed
    //  5-dword scratch buffer; the second dword (`out[1]`) is the one
    //  value every call site actually reads back.
    void __thiscall RegisterTypedField(const char* name, int type,
                                       int zero1, unsigned int* out, int zero2);
};

__declspec(naked) void __thiscall SFieldRegistrationPool::RegisterTypedField(
    const char*, int, int, unsigned int*, int)
{
    __asm
    {
        mov eax, 06F7127A0h
        jmp eax
    }
}
