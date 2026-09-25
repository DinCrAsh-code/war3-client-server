//============================================================================
//  0x6F01F5A0 / 0x6F01F760 - two lazily-constructed, module-global Storm
//  objects, both tagged with the same opaque allocation string: IDA could
//  not resolve `unk_6F876330` to printable text, so its real bytes are not
//  recoverable from a disassembly-only dump and it is declared here as an
//  opaque extern rather than guessed at.
//
//  0x6F01F5A0 has a real __try/__except frame around the allocate-and-
//  construct step (a security-cookie-XORed SEH frame, `push offset
//  SEH_6F01F5A0` / `mov large fs:0` on entry).  Built at /GS- /EHs-c-, as
//  this whole reconstruction is, that frame cannot appear - the same
//  mismatch docs/msvc-vc8-idioms.md documents in "An `__except_handler4`-
//  shaped frame this toolchain cannot reproduce"; the logic either side of
//  it is otherwise the same allocate/construct/store sequence.
//
//  The larger object (0x22F0 bytes) is handed to a constructor this call
//  tree never reaches the body of (0x6F768D10, deep in Storm's own address
//  range - out of this target's scope per CLAUDE.md, "work outward... not
//  into Storm's internals"); declared only so the call site resolves.
//============================================================================
#include "storm.h"
#include "stormsingletona.h"
#include "stormsingletonb.h"

//  unk_6F876330 - IDA could not resolve this to printable text, and
//  matching_decomp_injection's binder only binds funcmap.py globals whose
//  IDA name carries a dword_/byte_/word_/flt_/dbl_/qword_ prefix (see
//  storm.h's g_dwTlsIndex note for the reason and its cost the last time
//  that was missed), so an `extern` here would silently link to unbound,
//  unrelated storage in the injected build rather than the real string.
//  Given a real local definition instead - the bytes stored do not affect
//  the generated code, only what a debugger would show for Storm's own
//  "logfilename" argument.
static const char g_tag_unk6F876330[] = "unk_6F876330";

//  0x6F768D10 - out of this target's scope (deep in Storm's own address
//  range - see CLAUDE.md, "work outward... not into Storm's internals").
//  Not a reconstruction: a direct redirect to the game's own code at its
//  real address, the same pattern src/pathplacementfilter_thunk.cpp uses,
//  so a build hooking GetStormSingletonA still constructs a real object
//  instead of link-failing on a symbol nothing here defines.
__declspec(naked) void* __fastcall ConstructStormSingletonA(void*)
{
    __asm
    {
        mov eax, 06F768D10h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F01F5A0
//----------------------------------------------------------------------------
SStormSingletonA* __fastcall GetStormSingletonA()
{
    if (g_pStormSingletonA != 0)
        return g_pStormSingletonA;

    void* raw = SMemAlloc(0x22F0, g_tag_unk6F876330, -1, 0);
    g_pStormSingletonA = raw ? (SStormSingletonA*)ConstructStormSingletonA(raw) : 0;
    return g_pStormSingletonA;
}

//  The four words are named for what they are in stormsingletonb.h: a
//  TSGrowableArray's alloc/count/data/chunk, which is how 0x6F0116E0 walks
//  the object.  Only the names changed; the four zero stores are the same.
extern SStormSingletonB* g_pStormSingletonB;   // dword_6FAAE79C

//----------------------------------------------------------------------------
//  0x6F01F760
//----------------------------------------------------------------------------
SStormSingletonB* __fastcall GetStormSingletonB()
{
    if (g_pStormSingletonB != 0)
        return g_pStormSingletonB;

    SStormSingletonB* obj =
        (SStormSingletonB*)SMemAlloc(0x10, g_tag_unk6F876330, -1, 0);
    if (obj != 0)
    {
        obj->m_alloc = 0;
        obj->m_count = 0;
        obj->m_data = 0;
        obj->m_chunk = 0;
    }
    g_pStormSingletonB = obj;
    return obj;
}
