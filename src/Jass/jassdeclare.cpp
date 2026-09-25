//============================================================================
//  `.\Jass.cpp` - declaring a name into one of the two plain (non-interned)
//  identifier tables, and the parallel "declared in order" array a scope
//  keeps beside its own name table.
//============================================================================
#include "jasssymtables.h"
#include "allochash.inl"

//  Storm ordinal 590 (string hash), 507 (heap-duplicating strdup), 403
//  (free) - see Containers/allochashtable.h for the first two; the third is
//  declared there as SMemFree already (storm.h), so only its ordinal-named
//  spelling is added here to match the dump's own `call Storm_403`.
//
//  Both Declare functions below push the identical two Storm memory-tag
//  constants ahead of their free-then-strdup pair: the source-file operand
//  is `aEDrive1TempBui`, the exact string Containers/allochash.inl's own
//  ALLOCHASH_HEADER_TAG already names ("e:\Drive1\temp\buildwar3x\Storm\
//  H\stpl.h" - a Storm header, not Jass.cpp itself, so this tag records
//  where the *template* body both hand-written Declare functions below
//  mirror was compiled from, not either dump's own call site), and the
//  line numbers are 2616 (the free call) / 2617 (the strdup call) in both
//  dumps alike - allochash.inl's own comment already independently names
//  these as "lines 0xA38 and 0xA39", the strongest evidence available that
//  both functions really do inline one shared "free the old key unless
//  it's the same object, then strdup the new one" helper at those two
//  fixed lines, not two independent hand-copies with independently-guessed
//  tags.  Repeated as a local literal rather than pulling in
//  allochash.inl (documented there as "included only by the translation
//  units that own an instantiation, never by a caller").  An earlier pass
//  left this as `SMemFree(p, "", -2, 0)` / `Storm_507(name, "", 0)` - a
//  placeholder tag that compiled, scored, and linked fine (a
//  memory-tracking argument observes nothing else in this program) but
//  left both Declare functions several instructions short of their target
//  because the tag string's own address and the two immediate line
//  numbers are real operands the instruction stream carries.
#define JASSDECLARE_STORM_HEADER_TAG \
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h"

static inline void FreeStormKey(void* p)
{
    SMemFree(p, JASSDECLARE_STORM_HEADER_TAG, 2616, 0);
}

//----------------------------------------------------------------------------
//  0x6F44B2C0 - JassThreadLocal's own scope keeps a JASSNAMENODE* array
//  beside its JassNameTable, indexed by declaration slot; this answers the
//  name a given slot was declared under.  Slot 0 is the reserved "no name"
//  entry and short-circuits before either array is touched.
//
//  `this` is not JassThreadLocal itself, only the closure this dump
//  establishes: an object whose own +0x08 is a pointer to a composite
//  {TSGrowableArray<JASSNAMENODE*> m_bySlot; JassNameTable m_names;} - the
//  same composite 0x6F454930 (JASSSCOPE, below) builds one field of.
//----------------------------------------------------------------------------
struct JASSSCOPEREF
{
    char        m_reserved00[8]; // +0x00  unread in this closure
    JASSSCOPE*  m_scope;         // +0x08

    const char* GetDeclaredName(unsigned int index);
};

const char* JASSSCOPEREF::GetDeclaredName(unsigned int index)
{
    if (index == 0)
        return 0;

    return m_scope->m_bySlot[index]->m_key;
}

//----------------------------------------------------------------------------
//  0x6F454930 - declare `name` in this scope's JassNameTable, reusing an
//  existing node if the name is already there.  A freshly made node's key
//  is duplicated through Storm; a re-declared one's is freed first unless
//  it is literally the same string object (SetKey's own guard, spelled out
//  by hand here rather than through TAllocatorHashTable::SetKey).
//
//  `slot` selects how the node is filed into the parallel array: a nonzero
//  slot writes it at that fixed index (globals, whose final count is known
//  ahead of time - SetCount(slot+1) grows to fit); a zero slot instead
//  *appends* through Grow() and records `m_arrayHigh - 1` as the node's own
//  slot, i.e. locals are allocated top-down out of a shrinking count that
//  `this` (JASSSCOPE) keeps at +0x04 - `m_bySlot.m_count` doing double duty
//  as "next local slot" while the array itself only ever grows by one at a
//  time from the front.
//----------------------------------------------------------------------------
const char* JASSSCOPE::Declare(const char* name, unsigned int slot)
{
    JASSNAMENODE* node = m_names.Ptr(name);

    if (node == 0)
    {
        unsigned int hash = Storm_590(name);
        node = m_names.NewNode(hash, 0, 0);
        node->m_hashval = hash;

        if (node->m_key != name)
        {
            if (node->m_key != 0)
                FreeStormKey((void*)node->m_key);

            node->m_key = Storm_507(name, JASSDECLARE_STORM_HEADER_TAG, 2617);
        }

        if (slot != 0)
        {
            m_bySlot.SetCount(slot + 1);
            m_bySlot[slot] = node;
            node->m_slot = slot;
        }
        else
        {
            *m_bySlot.Grow() = node;
            node->m_slot = m_bySlot.m_count - 1;
        }
    }

    return node->m_key;
}

//----------------------------------------------------------------------------
//  0x6F454E30 - declare `name` in this scope's JassVarTable and stamp its
//  type and slot, hands the node back through `outResult` rather than in
//  eax (the caller of DeclareName above keeps its answer in eax; this one's
//  caller keeps it live across the call in a stack temp instead, and the
//  shipped tail leaves eax holding the slot argument it just stored, not
//  the node).  `this` is only ever read at +0x18 in this closure - a
//  JassVarTable embedded in a larger per-scope object, the shifted-by-4
//  sibling of JASSSCOPE above.
//----------------------------------------------------------------------------
struct JASSVARSCOPE
{
    char         m_reserved00[0x18]; // +0x00  unread in this closure
    JassVarTable m_vars;             // +0x18

    void Declare(const char* name, JassNativeSlot** outResult, void* type,
                unsigned int slot);
};

void JASSVARSCOPE::Declare(const char* name, JassNativeSlot** outResult,
                           void* type, unsigned int slot)
{
    JassNativeSlot* node = m_vars.Ptr(name);
    *outResult = node;

    if (node == 0)
    {
        unsigned int hash = Storm_590(name);
        node = m_vars.NewNode(hash, 0, 0);
        node->m_hashval = hash;

        if (node->m_key != name)
        {
            if (node->m_key != 0)
                FreeStormKey((void*)node->m_key);

            node->m_key = Storm_507(name, JASSDECLARE_STORM_HEADER_TAG, 2617);
        }

        *outResult = node;
    }

    (*outResult)->m_type = type;
    (*outResult)->m_slot = slot;
}

//----------------------------------------------------------------------------
//  0x6F454510 - `JassNameTable::NewNodeWithKey` (Containers/allochash.inl),
//  called with a hash the caller already has.
//  0x6F454820 - `JassNameTable::Declare`, the thin wrapper around it: hash
//  `name` and forward.  Only ever reached as a table method (`this` is the
//  `JassNameTable` itself, not a `JASSSCOPE` wrapping one), distinct from
//  `JASSSCOPE::Declare` above (which looks the name up first).
//
//  0x6F4545D0 - `JassVarTable::NewNodeWithKey`.
//  0x6F4548B0 - `JassVarTable::Declare`, the same shape on `JassVarTable`
//  instead.
//----------------------------------------------------------------------------
template JASSNAMENODE* JassNameTable::NewNodeWithKey(unsigned int,
                                                      const char*, void*,
                                                      void*);
template JASSNAMENODE* JassNameTable::Declare(const char*, void*, void*);
template JassNativeSlot* JassVarTable::NewNodeWithKey(unsigned int,
                                                       const char*, void*,
                                                       void*);
template JassNativeSlot* JassVarTable::Declare(const char*, void*, void*);
