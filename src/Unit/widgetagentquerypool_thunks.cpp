//============================================================================
//  0x6F2E4F00 / 0x6F2E4FC0 - the widget-agent-query pool allocator G4's own
//  report flagged as having "~6 more unresolved callees/globals"
//  (createunit-H2 follow-up).  Investigated for real rather than trusting
//  that count: most of it was already reconstructed elsewhere.
//
//  0x6F2E4F00 hashes a 'hgw+'-style agile type id (AGILE_TYPE_ID::Hash,
//  0x6F4C8520), looks the type record up (TSHashTable<AGILE_TYPE_DATA,
//  AGILE_TYPE_ID>::Ptr, 0x6F001EC0), fills an SWidgetAgentQuery block from
//  it (InitWidgetAgentQuery, 0x6F472C20) and submits it (SubmitWidgetAgent
//  Query, 0x6F48AB80) - all four already real, EXACT/DIFFERS reconstructions
//  in Widget/widgetagentquery.cpp/.h and Storm/storm.h (the same four calls
//  cunit_agent_seh_abilityfactories.cpp's own header already names as this
//  family's shared shape).  It also assigns the built agent into a checked
//  slot via 0x6F2D04D0 - already a declared-not-defined naked redirect
//  (Unit/checkedslot_6f2d04d0.h, from the original G3 integration pass) -
//  and finally hands off to 0x6F2E3640.  So of the "~6" callees, four were
//  already resolved and one (0x6F2D04D0) was already a deliberate thunk;
//  what is genuinely new and unresolved is:
//
//    * 0x6F2E4A30 - a full `__except_handler4`-shaped SEH-wrapped growth
//      routine over a *global* table (`dword_6FAB4E88`, passed as `this` -
//      not a per-call local), the same rounding-to-chunk arithmetic
//      TSGrowableArray<T>::SetCount already has, over two more
//      unresolved callees of its own (0x6F2D07F0/0x6F2D69E0 - a second
//      ComputeChunk/SetAlloc-shaped pair, not yet reconstructed under any
//      name);
//    * 0x6F2E3640 - constructs a new pool-registry entry by calling four
//      virtual methods through the new object's own vtable slots at
//      +0x28/+0x30/+0x38/+0x40, each with real per-slot arguments (not a
//      generic loop) - the entry's own class is not established anywhere
//      in this call tree.
//
//  Both are real, substantial, self-contained pieces of work - not phantom
//  externs - and neither is a leaf, so both 0x6F2E4F00 and 0x6F2E4FC0 (the
//  second of which turns out to depend on 0x6F2E4F00 plus a *third* new
//  piece: an argument arriving in `edi` from its caller with no MSVC
//  spelling that reaches it, the same "arguments in eax/esi/edi" shape
//  Render/filecachefind.cpp's own header documents, feeding a vtable-slot
//  +0xB8 accessor that - unlike Widget/widget_slot68.cpp's own use of that
//  same slot - passes it an output buffer, a call shape that family's own
//  file does not cover) are naked redirects to the real, unhooked shipped
//  bodies rather than a rushed guess at either. A genuine reconstruction
//  belongs with whoever next takes on the global pool-registry table
//  (`dword_6FAB4E74`/`dword_6FAB4E88`/`dword_6FAB4E90`) as its own target.
//============================================================================
#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig             \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  Both stack params, no register args read at entry (retn 8) - __stdcall
//  is the plain callee-cleanup shape that matches without inventing a
//  register convention nothing in the body reads.
void* __stdcall AllocateWidgetAgentQueryPoolEntry(int arg0, int arg4);
ADDR_THUNK(void* __stdcall AllocateWidgetAgentQueryPoolEntry(int arg0,
                                                              int arg4),
           0x6F2E4F00)

//  Genuinely reads its object out of `edi`, carried over from its own
//  caller rather than passed by any MSVC convention (the same
//  "arguments in eax/esi/edi" shape docs/msvc-vc8-idioms.md and
//  Render/filecachefind.cpp already document) - modelled as a
//  __thiscall member purely so the declared stack-byte count (one `int`,
//  retn 4) matches what the real shipped body cleans; the `this` a caller
//  loads into ecx for this declaration is never read by the real body.
struct SWidgetAgentQueryPoolLookup
{
    void* GetOrMakeEntry(int flag);
};
ADDR_THUNK(void* __thiscall SWidgetAgentQueryPoolLookup::GetOrMakeEntry(int flag),
           0x6F2E4FC0)
