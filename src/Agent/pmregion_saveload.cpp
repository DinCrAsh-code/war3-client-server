//============================================================================
//  NIpse::CPmRegion - vtable slots 9 (Save, 0x6F49E7C0) and 10 (Load,
//  0x6F4A0100). See pmregion.h for the class-level header comment and the
//  per-field reasoning.
//============================================================================
#include "pmregion.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

//  0x6F4AEF40 - WritePathRefEntry, already reconstructed elsewhere
//  (Pathfinding/pathref_write.cpp) as `void __fastcall
//  WritePathRefEntry(CDataStoreScratch*, const void*)`.
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

//  0x6F4AF090 - SPathRefReader::ReadEntry, already reconstructed elsewhere
//  (Pathfinding/pathref_load.cpp / popos_saveload.cpp) as a thiscall member
//  with no arguments, returning the raw pointer it read.
struct SPathRefReader
{
    void* ReadEntry();
};

//  0x6F49F930 - see this file's own trailing comment (below the `NIpse`
//  namespace) for the full reasoning; forward-declared here so Load can
//  call it.
static void PmRegionGridWriteback(void* linkPrevObj, void* rect, void* region);

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F49E7C0 - vtable slot 9, Save. `store` arrives as a single stack
//  argument (this thiscall's sole parameter; `retn 4` at the end - inherited
//  from the tail-called WriteDword below, which cleans the same one dword).
//
//  Two path-ref-shaped link pointers first (+0x2C, +0x30), each via a
//  standalone call to WritePathRefEntry (its own return is void, so `store`
//  itself is never reassigned by either of these two calls - matches the
//  dump's own two calls with no `mov edi,eax` between them). Then +0x34 via
//  WriteDword, whose return IS captured back into `store` (the dump's own
//  `mov edi,eax`) because the next four writes read it back through that
//  same local. Those four (+0x1C/+0x20/+0x24/+0x28) go out via
//  WriteDwordAlt2 instead, and their own returns are never captured -
//  `store` does not change identity by writing through it, so the compiler
//  has no reason to move `eax` back into the register `store` already
//  lives in. The final three (+0x38/+0x3C/+0x40) are a genuine fluent
//  chain with no named intermediate at all, ending in a tail call the
//  dump's own trailing `jmp sub_6F4C2360` shows directly.
//----------------------------------------------------------------------------
void CPmRegion::Save(CDataStoreScratch* store)
{
    WritePathRefEntry(store, m_linkPrev);
    WritePathRefEntry(store, m_linkNext);

    store = store->WriteDword(m_mask);
    store->WriteDwordAlt2(m_top);
    store->WriteDwordAlt2(m_left);
    store->WriteDwordAlt2(m_bottom);
    store->WriteDwordAlt2(m_right);

    store = store->WriteDword(m_field38);
    store = store->WriteDword(m_field3C);
    store->WriteDword(m_flags);
}

//----------------------------------------------------------------------------
//  0x6F4A0100 - vtable slot 10, Load. Save's near-mirror, with one real
//  asymmetry at the very end: after reading +0x40 back in, this function
//  tests the freshly-read `m_flags & 0x10000000` and, only when that bit is
//  clear, calls PmRegionGridWriteback (sub_6F49F930) - a genuine,
//  substantial (nested-loop, grid-indexing) routine this session did not
//  confidently re-derive; left a THUNK below rather than guessed at. Every
//  other line of Load itself is fully reconstructed.
//----------------------------------------------------------------------------
void CPmRegion::Load(CDataStore* store)
{
    m_linkPrev = ((SPathRefReader*)store)->ReadEntry();
    m_linkNext = ((SPathRefReader*)store)->ReadEntry();

    //  Read into a stack local first, then copy into the field - matches
    //  the dump's own `lea eax,[esp+local]` / later `mov ecx,[esp+local] ;
    //  mov [esi+52],ecx` shape exactly, unlike every other field here
    //  (which the dump reads straight into `this` through a `lea` off
    //  `esi`/`ebx` directly).
    unsigned int mask;
    store->ReadDword(&mask);
    m_mask = mask;

    //  A single base pointer, `[base+4]`/`[base+8]`/`[base+12]` for the
    //  next three - matches the dump's own `lea ebx,[esi+28]` once, then
    //  `[ebx+4]`/`[ebx+8]`/`[ebx+0xC]`, rather than a fresh `lea` off
    //  `esi` each time (the shape every OTHER field group in this function
    //  uses, Save's own WriteDwordAlt2 group included) - a real asymmetry
    //  this session did not further explain beyond "the compiler kept one
    //  base register live across these four instead of four".
    unsigned int* rect = &m_top;
    store->ReadDwordAlt3(rect);
    store->ReadDwordAlt3(rect + 1);
    store->ReadDwordAlt3(rect + 2);
    store->ReadDwordAlt3(rect + 3);

    store = store->ReadDword(&m_field38);
    store = store->ReadDword(&m_field3C);
    store->ReadDword(&m_flags);

    if ((m_flags & 0x10000000) == 0)
        PmRegionGridWriteback(m_linkPrev, rect, this);
}

}  // namespace NIpse

//----------------------------------------------------------------------------
//  0x6F49F930 - a real, substantial function: reads a bounding box off the
//  `rect` argument, clamps it against four of `this`'s own fields
//  (+0x54/+0x58/+0x5C/+0x60), then double-nested-loops the clamped range
//  calling sub_6F49F8A0 (itself unreconstructed) once per cell with an
//  index computed off two more of `this`'s own fields (+0x28/+0x3C/+0x40).
//  Both callees reach well past this session's own CPmRegion field-layout
//  scope (they touch a DIFFERENT object's fields entirely - `this` here is
//  `m_linkPrev`, not the CPmRegion being saved/loaded - and grid/cell
//  bookkeeping neither Save nor Load nor DumpDebugState needed any model
//  of). Left a THUNK, same convention as
//  Pathfinding/pmregionpoolalloc.cpp's own SPmRegionPoolAllocator::
//  AllocElement: a plain struct wrapping one `__declspec(naked)` method,
//  because this compiler rejects `__declspec(naked)` on a free function's
//  calling convention here (__thiscall) without going through a member.
//
//  Call site (Load, above): `ecx` = m_linkPrev (a pointer this session does
//  not model further); stack args pushed `this` (the CPmRegion being
//  loaded) THEN `&m_top` (the just-read bounding box) - right-to-left
//  argument order means `&m_top` is the FIRST declared parameter (pushed
//  last, closest to the call) and `this` the second - `retn 8` cleans both.
//----------------------------------------------------------------------------
struct SPmRegionGridWritebackTarget
{
    void __thiscall Invoke(void* rect, void* region);
};

__declspec(naked) void SPmRegionGridWritebackTarget::Invoke(void*, void*)
{
    __asm
    {
        mov eax, 06F49F930h
        jmp eax
    }
}

static void PmRegionGridWriteback(void* linkPrevObj, void* rect, void* region)
{
    ((SPmRegionGridWritebackTarget*)linkPrevObj)->Invoke(rect, region);
}
