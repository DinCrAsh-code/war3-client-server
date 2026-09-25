//============================================================================
//  CPlayerWar3::CPlayerWar3()'s own sub-object construction helpers
//  (playerwar3_ctor.cpp), split out here per CLAUDE.md's "one translation
//  unit per original module" - these bodies come from a different address
//  neighbourhood (0x6F40Fxxx/0x6F42xxxx/0x6F401xxx/0x6F416xxx) than the
//  constructor itself (0x6F4193xx).
//
//  Two of the addresses below are deliberate redirects rather than
//  reconstructions, for the same reason Misc/citemclosure_batch_m_
//  framelayer.cpp's own ConstructShortcutRegTable already is:
//
//  0x6F4221B0 (CSelectionWar3's own constructor) and 0x6F401390 (CTechTree's
//  own constructor) each stamp `??_7CSelectionWar3@@6B@` / `??_7CTechTree@@
//  6B@` over embedded sub-objects this repo has never modelled (CUnitSet,
//  TSHashTable<TTechTreeReg,HASHKEY_4CC>, TSHashTable<TAbilReg,HASHKEY_4CC>)
//  - and CSelectionWar3's own array-element constructor (sub_6F3F4B40, the
//  CUnitSet ctor two `eh vector constructor iterator` calls reach) has no
//  dump body anywhere in this session's dump
//  (asm/sub_6F4193E0_0x6F4193E0_calltree_asm.md), so a faithful depth-3
//  reconstruction of this branch is not achievable from what this session
//  has.  Reconstructing CSelectionWar3/CTechTree concretely here would
//  stamp a vtable of this build's own making over a class this repo has
//  never modelled - exactly ctor_vtable_audit.py's own crash class, the
//  same reasoning storm.h's own header comment gives for never stamping
//  TSHashTable<T,K>'s template vtable concrete.  Redirected rather than
//  guessed, for the same reason and with the same 0% instruction match by
//  construction; see the BEHAVIOUR entries in tools/funcmap.py.
//
//  The +0x2AC "hero list" array's own three helpers (0x6F334410/0x6F0860C0/
//  0x6F09F890) are NOT here - see the comment lower in this file for why
//  (already reconstructed elsewhere, under SCheckedUnitSlot's own name).
//  The three that remain (+0x2BC's TSGrowableArray<SIntMiniValue> own
//  SetCount/ComputeChunk/grow-core) are real reconstructions: none touches
//  a vtable and the whole tree is self-contained within this dump's own
//  depth-3 window.  Each is a *duplicate* compiled body of what is
//  semantically TSGrowableArray<T>::SetCount/ComputeChunk/SetAlloc -
//  storm.h already declares SetCount/ComputeChunk for this template, each
//  at its OWN single address, and the image was linked without /OPT:ICF
//  (agenttypedslots.h's own header note), so the *same* template method
//  compiled into a different translation unit's object file survives as a
//  second, textually-identical body at a second address rather than being
//  folded.  Written here as free functions with their own names rather
//  than as a second `template<>` specialisation of the shared method (which
//  would collide with whatever TU eventually reconstructs the canonical
//  0x6F46F0C0/0x6F467B00 addresses storm.h already names) - CPlayerWar3's
//  own private duplicates, not the shared template body.
//============================================================================
#include "playerwar3.h"
#include "storm.h"

//----------------------------------------------------------------------------
//  0x6F4221B0 - CSelectionWar3::CSelectionWar3().  `retn 0`.  See the file
//  header above for why this is a redirect.
//----------------------------------------------------------------------------
struct CSelectionWar3Ctor
{
    void Construct();
};

__declspec(naked) void CSelectionWar3Ctor::Construct()
{
    __asm
    {
        mov     eax, 06F4221B0h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F401390 - CTechTree::CTechTree().  `retn 0`.  Same reasoning.
//----------------------------------------------------------------------------
struct CTechTreeCtor
{
    void Construct();
};

__declspec(naked) void CTechTreeCtor::Construct()
{
    __asm
    {
        mov     eax, 06F401390h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F3F4B40 - CUnitSet's own per-element constructor, the target the
//  `eh vector constructor iterator` two calls inside CSelectionWar3Ctor
//  reach through a raw function-pointer argument.  A later session's own
//  fresh `ida_query calltree_asm` on this exact address (the previous
//  session's blocker: "IDA's call-tree walk does not follow a function
//  pointer argument into its target") finally surfaced a body - ten
//  instructions, no prologue, no other call: a hand-inlined default
//  constructor that stamps `??_7CUnitSet@@6B@` at +0, zeroes +4/+0x10, and
//  self-links an intrusive circular list head at +8/+0xC (the classic
//  "prev = this, next = ~this" sentinel-init idiom, same shape
//  `SCheckedAllySlot`'s own family and `TSExplicitList` use elsewhere in
//  this codebase).
//
//  Reconstructed as `__declspec(naked)`, matching CLAUDE.md's own rule
//  that a naked body should score 1.000 by construction: no `virtual`
//  keyword appears anywhere in this file, so nothing here asks the
//  compiler to synthesise *our own* vtable the way a real C++ constructor
//  for a class with virtual methods would (the crash class
//  ctor_vtable_audit.py exists for) - this literally reproduces the
//  shipped store, byte for byte, of the *real* `??_7CUnitSet@@6B@`
//  vtable's own address (0x6F94C9F4, `tools/ida_query.py names CUnitSet`).
//  `g_CUnitSetVftable` is declared but never defined here (funcmap.py
//  DATA), the same "extern, resolved by the real image" shape
//  `g_foodCeilingByDifficulty` and every other DATA row in this codebase
//  already is - MASM's inline `__asm` cannot reference a mangled C++
//  symbol like `??_7CUnitSet@@6B@` directly (the `?`/`@` characters are
//  not valid MASM identifier characters), and a bare numeric literal in
//  its place would canonicalise as a plain immediate rather than the
//  `offset SYM` form the dump's own instruction stream has, so this is
//  the only spelling that both matches instruction-for-instruction *and*
//  ends up pointing at the same address once linked.
//
//  Not yet reachable from anything else in this build - CSelectionWar3's
//  own constructor (immediately above) stays a redirect to the real,
//  unhooked 0x6F4221B0, so it calls the *real* 0x6F3F4B40 in the shipped
//  image regardless of this reconstruction's own presence here. Kept as
//  a real, named, independently addressable and scoreable leaf now that a
//  body exists for it, the same way this repo keeps any other leaf whose
//  only caller is still thunked.
//----------------------------------------------------------------------------
extern void* g_CUnitSetVftable;   // ??_7CUnitSet@@6B@, funcmap.py DATA

struct CUnitSetElementCtor
{
    void Construct();
};

__declspec(naked) void CUnitSetElementCtor::Construct()
{
    __asm
    {
        mov     eax, ecx
        mov     [eax], offset g_CUnitSetVftable
        lea     edx, [eax+8]
        xor     ecx, ecx
        mov     [edx+4], ecx
        mov     [edx], edx
        not     edx
        mov     [eax+4], ecx
        mov     [eax+0Ch], edx
        mov     [eax+10h], ecx
        retn
    }
}

void PlayerWar3ConstructSelectionWar3(void* mem)
{
    ((CSelectionWar3Ctor*)mem)->Construct();
}

void PlayerWar3ConstructTechTree(void* mem)
{
    ((CTechTreeCtor*)mem)->Construct();
}

//----------------------------------------------------------------------------
//  The +0x2AC "hero list" array's own SetCount(7) call (0x6F334410) is NOT
//  written here.  It, its ComputeChunk (0x6F0860C0) and its SetAlloc
//  (0x6F09F890) were already reconstructed by an earlier session as
//  TSGrowableArray<SCheckedUnitSlot>::SetCount/ComputeChunk/SetAlloc
//  (Containers/unitagentptrarraycount.cpp / unitagentptrarray.cpp) - grep
//  turned this up (CLAUDE.md's own "grep the callees too" rule) after a
//  first draft duplicated all three under private names and collided with
//  the existing funcmap.py FUNCTIONS rows for the same three addresses.
//  SCheckedUnitSlot *is* the binary's CAgentPtr<CUnit> (agenttypedslots.h's
//  own note on that struct), so this is the same array
//  CPlayerWar3::~CPlayerWar3() already tears down through the *other* name
//  for the identical layout (TSGrowableArray<CAgentPtr>, agentptrarray.cpp)
//  - two shipped instantiations of one class at two different addresses,
//  the same "no /OPT:ICF" duplication agenttypedslots.h's own header note
//  already documents for eighteen Assign bodies.  playerwar3_ctor.cpp calls
//  TSGrowableArray<SCheckedUnitSlot>::SetCount(7) on the +0x2AC field
//  directly.
//----------------------------------------------------------------------------
//  The three "IntegerMini" list (+0x2BC, TSGrowableArray<SIntMiniValue>)
//  helpers - the identical shape as the hero-array trio above with the
//  refcount release/addref dropped, because SIntMiniValue is a plain int
//  with no destructor.
//----------------------------------------------------------------------------

//  0x6F40F030.
static unsigned int PlayerWar3IntMiniArrayComputeChunk(void* arr, unsigned int count)
{
    if (count >= 0x40)
    {
        ((unsigned int*)arr)[3] = 0x40;
        return 0x40;
    }

    unsigned int c = count;
    for (;;)
    {
        unsigned int cleared = (c - 1) & c;
        if (cleared == 0)
            break;
        c = cleared;
    }
    if (c < 1)
        c = 1;
    return c;
}

//  0x6F40F090.
static void PlayerWar3IntMiniArrayGrow(void* arrv, unsigned int newAlloc)
{
    unsigned int* alloc = (unsigned int*)arrv;
    unsigned int* data  = (unsigned int*)(alloc + 2);

    void* oldData = *(void**)data;
    void* newData = SMemReAlloc(oldData, newAlloc * 4, "AVIntegerMini@@", -2, 0x10);
    *alloc = newAlloc;
    *(void**)data = newData;
    if (newData != 0)
        return;

    newData = SMemAlloc(newAlloc * 4, "AVIntegerMini@@", -2, 0);
    *(void**)data = newData;
    if (oldData == 0)
        return;

    unsigned int copyCount = alloc[1];
    if (newAlloc < copyCount)
        copyCount = newAlloc;

    for (unsigned int i = 0; i < copyCount; i++)
        ((unsigned int*)newData)[i] = ((unsigned int*)oldData)[i];

    SMemFree(oldData, "AVIntegerMini@@", -2, 0);
}

//  0x6F416EE0.  `retn 4`.  Unlike the hero array's own SetCount above, a
//  shrink here (count <= m_count) does nothing but set m_count - a plain
//  int has no destructor to run, so the shipped body skips straight past
//  both the grow branch and the zero-fill loop below.
//
//  External linkage (not `static`, as this file's earlier, ctor-only
//  revision had it): CPlayerWar3::Load's own IntegerMini-array reader
//  (playerwar3_loadhelpers.cpp) is this function's second caller, reached
//  with a dynamic count read off the stream rather than the constructor's
//  own literal 7 - see PlayerWar3IntMiniArrayReserve below for why that
//  one stays a thin wrapper instead of folding the two together.
void PlayerWar3IntMiniArraySetCount(void* arrv, unsigned int count)
{
    unsigned int* alloc = (unsigned int*)arrv;
    unsigned int* pcount = alloc + 1;
    unsigned int* data  = alloc + 2;

    if (count > *pcount)
    {
        if (count > *alloc)
        {
            unsigned int c = alloc[3];
            if (c == 0)
                c = PlayerWar3IntMiniArrayComputeChunk(arrv, count);

            unsigned int newAlloc = count;
            unsigned int over = count % c;
            if (over != 0)
                newAlloc = count + (c - over);

            PlayerWar3IntMiniArrayGrow(arrv, newAlloc);
        }

        for (unsigned int i = *pcount; i < count; i++)
            ((unsigned int*)*(void**)data)[i] = 0;
    }

    *pcount = count;
}

void PlayerWar3IntMiniArrayReserve(void* arr)   { PlayerWar3IntMiniArraySetCount(arr, 7); }
