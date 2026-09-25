//============================================================================
//  NIpse::CPoPosCl - vtable slots 9 (Save) and 10 (Load), shared with
//  NTempest::CLinkedList<NIpse::CPoPosCl>. See poposcl.h for the
//  class-level header comment.
//============================================================================
#include "poposcl.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

struct SPathRefReader { void* ReadEntry(); };

typedef void (__thiscall *PoSeparateSaveFn)(void*, CDataStoreScratch*);
typedef void (__thiscall *PoSeparateLoadFn)(void*, CDataStore*);

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F494C00 - vtable slot 9, Save. Base `CPoPos::Save` first, then
//  +0x9C/+0xA0 (plain WriteDword), +0xA4 (WriteDwordAlt2), the seven
//  CFloats +0xB0..+0xCC (WriteCFloat), +0xD0/+0xD4/+0xD8 (WriteDwordAlt2),
//  a "this is linked into some list" flag (WriteDwordAlt3), a "m_poSeparate
//  is live" flag (WriteDwordAlt3), the raw +0xA8 pointer value
//  (WritePathRefEntry) and finally - if `m_poSeparate` is live - its own
//  Save through its own vtable slot 11 (a real indirect dispatch in the
//  shipped code, Agent/poseparate.h).
//----------------------------------------------------------------------------
void CPoPosCl::Save(CDataStoreScratch* store)
{
    ((CPoPos*)this)->Save(store);

    char* base = (char*)this;
    unsigned int linked = (*(unsigned int*)(base + 4) != 0 || *(unsigned int*)(base + 8) != 0) ? 1u : 0u;
    unsigned int hasPoSeparate = (m_poSeparate != 0) ? 1u : 0u;

    store->WriteDword(*(unsigned int*)(base + 0x9C));
    store->WriteDword(*(unsigned int*)(base + 0xA0));
    store->WriteDwordAlt2(*(unsigned int*)(base + 0xA4));

    WriteCFloat(store, (const CFloat*)(base + 0xB0));
    WriteCFloat(store, (const CFloat*)(base + 0xB4));
    WriteCFloat(store, (const CFloat*)(base + 0xB8));
    WriteCFloat(store, (const CFloat*)(base + 0xBC));
    WriteCFloat(store, (const CFloat*)(base + 0xC0));
    WriteCFloat(store, (const CFloat*)(base + 0xC4));
    WriteCFloat(store, (const CFloat*)(base + 0xC8));

    store->WriteDwordAlt2(*(unsigned int*)(base + 0xD0));
    store->WriteDwordAlt2(*(unsigned int*)(base + 0xD4));
    store->WriteDwordAlt2(*(unsigned int*)(base + 0xD8));

    store->WriteDwordAlt3(linked);
    store->WriteDwordAlt3(hasPoSeparate);

    WritePathRefEntry(store, m_relatedAgent);

    if (hasPoSeparate)
    {
        PoSeparateSaveFn fn = (PoSeparateSaveFn)(*(void***)m_poSeparate)[11];
        fn(m_poSeparate, store);
    }
}

//----------------------------------------------------------------------------
//  0x6F495FC0 - vtable slot 10, Load. LEFT TODO this session, unclaimed:
//  NOT a plain mirror of Save. Past the seven ReadCFloat calls and the
//  +0xD0/+0xD4/+0xD8 ReadDwordAlt3 trio (straightforward), it reads one
//  flag via ReadDwordAlt2, then calls ANOTHER vtable slot (this+0,
//  offset +0x14 - not one this cluster reconstructed) and compares its
//  result against a threshold (0x1771) to decide whether to read a
//  SECOND flag at all; reads `m_relatedAgent` back via
//  `SPathRefReader::ReadEntry`; conditionally links `this` into a global
//  list via `SPathOrderLink::InsertAfter` (0x6F493810, already real) when
//  not already linked; unconditionally calls the SAME CPoSeparate
//  teardown/recreate helper CPoPosCl::Slot4 uses (0x6F495ED0, this
//  session's own TODO there too - see the Slot4-family note); and only
//  THEN, if both the threshold flag and the second flag are set,
//  dispatches `m_poSeparate`'s own Load (vtable slot 12). Guessing this
//  shape (this session's own first, since-discarded attempt) would have
//  been confidently wrong - left for a follow-up session with room to
//  trace the +0x14 vtable slot and 0x6F495ED0 properly.
//----------------------------------------------------------------------------

}  // namespace NIpse
