//============================================================================
//  NIpse::CBaseMap<NIpse::CPmCell> (`??_7?$CBaseMap@VCPmCell@NIpse@@@NIpse@@6B@`,
//  0x6F953484) and NIpse::CBaseMap<NIpse::CPaCell> (0x6F952224) - the two
//  template instantiations underlying the "spatial map" cluster's own
//  concrete classes (NIpse::CProximityMap, NIpse::CAcceleratorMap), all
//  branching off NTempest::CPresenceTagged (docs/targets/
//  NTempestNIpse_BaseChain.md's own chain) rather than through
//  NIpse::CPrRelation - confirmed by these classes' own 17-slot vtables
//  reusing CPresenceTagged's slots 15/16 (FoldLinkFieldsIntoHash,
//  DumpDebugState) unchanged rather than CAgentRelation's own slot-15
//  override the CPrRelation-derived chain carries.
//
//  INHERITANCE, established from raw disassembly rather than guessed
//  (this session's own brief called for exactly that where the vtable
//  slot-target table alone was ambiguous):
//
//    NIpse::CAcceleratorMap  : public NIpse::CBaseMap<NIpse::CPaCell>
//    NIpse::CProximityMap    : public NIpse::CBaseMap<NIpse::CPmCell>
//
//  CAcceleratorMap's own vtable slots 3 (+0x0C) and 4 (+0x10) carry the
//  LITERAL SAME addresses (0x6F487ED0/0x6F487F70) as CBaseMap<CPaCell>'s -
//  the same "inherited unchanged" signature docs/targets/
//  NTempestNIpse_BaseChain.md already used to prove CRlAgent inherits
//  CPrRelation's own slot 15 - so CAcceleratorMap does not override either
//  slot and there is exactly one reconstruction of each (below), not two.
//  CProximityMap, by contrast, has its OWN distinct addresses at slots 3/4
//  (0x6F4A04D0/0x6F49FF20) - but each one's own body opens/closes by
//  calling straight into CBaseMap<CPmCell>'s own slot-3/4 bodies
//  (0x6F49F5D0/0x6F49F670 - see proximitymap.h), which is what actually
//  proves the inheritance edge (a shared vtable *address* proves "did not
//  override"; a caller *reaching into* the base's own body proves "is a
//  base of", the same way NIpse::CRlAgent::Apply's own call chain proved
//  CAgentRelation is CPrRelation in agentbaseabs_related.h). No other
//  ambiguity was found in this cluster's own slot-target tables - see
//  acceleratormap.h/proximitymap.h/pmregion.h for the remaining three
//  classes, none of which shares an address with either template here.
//
//  Neither template is modeled as a real C++ base class of the two
//  concrete ones (same reasoning as crlagent.h's own file header: no real
//  C++ inheritance chain exists in this repo yet for anything above
//  CAgentBaseAbs, and introducing one only for this cluster while every
//  sibling class stays flat would make the layout harder to reconcile
//  later, not easier) - both are flat, offset-addressed placeholders, and
//  CAcceleratorMap simply reuses these two bodies by address rather than
//  by C++ `: public` inheritance.
//============================================================================
#ifndef BASEMAP_H
#define BASEMAP_H

#include "handletable.h"   // SHandleRegistrationRequest, SHandleTableEntry
#include "CFloat.h"        // CFloat, g_CFloatZero

//  g_slopeThreshold1 (0x6FAAE4F4, encoded 1.0) - declared directly rather
//  than via `#include "pathmove.h"`, the same way most of its other users
//  do (Unit/unit_armordamagemultiplier.cpp, Item/item_radius.cpp, ...):
//  pathmove.h pulls in game.h, whose own `SHandleTable` would collide with
//  handletable.h's (see Agent/agentrelationteardown.cpp's own note on the
//  identical conflict).
extern const CFloat g_slopeThreshold1;

namespace NIpse {

//----------------------------------------------------------------------------
//  Trivial "empty virtual, one stack dword, retn 4" no-ops - same shape and
//  same reasoning as Agent/cagentbaseabs_noop_slots.cpp's own SVtableNoOpArg:
//  a free function cannot be spelled __thiscall in this compiler, so these
//  are ordinary members of a plain struct instead. Unlike that file's own
//  family, none of these four addresses is *shared* across levels of this
//  cluster's own chain - CBaseMap<CPaCell>'s own vtable slots 9/10 (+0x24/
//  +0x28) sit at 0x6F484780/0x6F484790, while CBaseMap<CPmCell>'s own copies
//  of the identical one-instruction body sit at the entirely different
//  addresses 0x6F49F380/0x6F49F390 - two independently compiled template
//  instantiations of what reads like the same source-level no-op, not one
//  address inherited by both. NIpse::CAcceleratorMap/CProximityMap each
//  override the same two slots again with their own further pair of
//  one-instruction no-op bodies (acceleratormap.h/proximitymap.h) - so all
//  four "columns" of this vtable slot stay a no-op start to finish across
//  this entire cluster; nothing here ever gives slots 9/10 a real body.
//----------------------------------------------------------------------------
struct SBaseMapNoOpArg
{
    void NoOp_6F484780(void* /*unused*/);   // CBaseMap<CPaCell> slot 9  (+0x24)
    void NoOp_6F484790(void* /*unused*/);   // CBaseMap<CPaCell> slot 10 (+0x28)
    void NoOp_6F49F380(void* /*unused*/);   // CBaseMap<CPmCell> slot 9  (+0x24)
    void NoOp_6F49F390(void* /*unused*/);   // CBaseMap<CPmCell> slot 10 (+0x28)
};

//----------------------------------------------------------------------------
//  Out-of-scope callees: real leaves this cluster's own slot-3/4/2 bodies
//  reach into, but that are not themselves addresses this session's own
//  brief lists - forward-declared purely so these translation units compile
//  under `verify.py` (which never links), same convention as Agent/
//  agentbaseabs_related.h's own CAgentBaseAbsSlot3Arg. The real definitions,
//  wherever they land, need to reconcile signatures with these declarations
//  (link_check.py's job, not this file's).
//----------------------------------------------------------------------------

//  this+0x1C - an opaque per-cell storage header both templates' own slot-3
//  (grow) and slot-4 (teardown) bodies drive through three small helpers.
//  Real layout unowned by this session (nothing here reads a field of it
//  directly - only ever passes its address to one of these three calls).
struct SBaseMapCellStorage
{
    //  0x6F484810 (CPaCell) / 0x6F49EC90 (CPmCell) - (re)allocate the
    //  per-cell array for `count` elements, filling each with the element
    //  pointed to by `defaultElement`. Each caller builds that default value
    //  as a local temporary and passes its address (an 8-byte, all-zero
    //  record for CPaCell's own InitGrid; a single all-`0xFFFFFF` dword for
    //  CPmCell's) - read as "the cell type's own zero/empty value", one per
    //  instantiation, though the callee's own real per-element size is
    //  unconfirmed from here (out of this session's scope).
    void AllocateCells_6F484810(const void* defaultElement, unsigned int count);
    void AllocateCells_6F49EC90(const void* defaultElement, unsigned int count);

    //  0x6F484890 (CPaCell) / 0x6F49ED10 (CPmCell) - shift/compact the
    //  array starting at `index` by `count` elements (memmove-shaped,
    //  `retn 8`).
    int ShiftCells_6F484890(unsigned int index, unsigned int count);
    int ShiftCells_6F49ED10(unsigned int index, unsigned int count);

    //  0x6F4847E0 (CPaCell) / 0x6F49EAB0 (CPmCell) - reset the array to
    //  empty (`retn 0`, no stack args).
    void ResetCells_6F4847E0();
    void ResetCells_6F49EAB0();
};

//  The two templates' own real (non-vtable) destructors - both restamp
//  straight to `NTempest::CPresence`'s own vftable with no base teardown at
//  all, the same open finding docs/targets/NTempestNIpse_BaseChain.md
//  already flags for CPresence/CPresenceTagged/CPrRelation's own scalar
//  deleting destructors (see basemap_cpacell.cpp/basemap_cpmcell.cpp for
//  where each is actually called from). Also carries an `__except_handler4`-
//  shaped SEH frame (`push -1` / `push offset SEH_<addr>` / cookie-XOR-esp /
//  scope table after the `retn`) that docs/msvc-vc8-idioms.md's own
//  "An `__except_handler4`-shaped frame this toolchain cannot reproduce"
//  section already established is unreproducible with this repo's bundled
//  compiler at `/GS- /EHs-c-` - not this session's callee to chase further.
struct SBaseMapRealDtorTarget
{
    void* RealDtor_6F486A40();   // CBaseMap<CPaCell>'s own ~CBaseMap()
    void* RealDtor_6F49F190();   // CBaseMap<CPmCell>'s own ~CBaseMap()
};

//----------------------------------------------------------------------------
//  NIpse::CBaseMap<NIpse::CPaCell> - flat, offset-addressed (see this file's
//  own header comment for why no real C++ base exists yet). Only the
//  members this cluster's own dump actually reaches are declared; ctor and
//  most fields below +0x1C are out of this session's own batch.
//----------------------------------------------------------------------------
class CBaseMap_CPaCell
{
public:
    //  0x6F484770 - vtable slot 0. Trivial "class name" accessor, same
    //  shape as CRlAgent_GetTypeName (Pathfinding/crlagent.h).
    const char* GetTypeName();

    //  0x6F4848F0 - vtable slot 1. Unlink-then-return-to-pool, same shape
    //  as every other ReturnToStaticPool in this base chain
    //  (Agent/presence_returntopool.cpp) - except this template's own
    //  static pool getter (0x6F497660) always returns null: nothing here
    //  ever pools a bare CBaseMap<CPaCell> instance directly (only the
    //  concrete NIpse::CAcceleratorMap, which overrides this same slot with
    //  its own real pool - acceleratormap.h).
    void ReturnToStaticPool();

    //  0x6F487D70 - vtable slot 2, scalar deleting destructor. Calls the
    //  real (non-teardown) destructor above only when the `this != 0` guard
    //  in the low bit of `flags` fires the free.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F487ED0 - vtable slot 3 (+0x0C). Shared unchanged by
    //  NIpse::CAcceleratorMap (see this file's own header comment) - this
    //  one body serves both classes' vtables. Registers `this` into the
    //  global handle table via `request`, then derives this map's own grid
    //  geometry (`request`'s own +0x10/+0x14 fields, read as width/height)
    //  and (re)allocates the per-cell array to `width * height` entries.
    //  Declared `void`, not a pointer: the dump never sets `eax` explicitly
    //  before its own epilogue, so whatever is left there (a leftover CFloat
    //  bit pattern) is dead, not a real return value.
    void InitGrid(SHandleRegistrationRequest* request);

    //  0x6F487F70 - vtable slot 4 (+0x10). Shared unchanged by
    //  NIpse::CAcceleratorMap, same as slot 3. Tears the grid back down:
    //  shifts the cell array to empty if it is not already, resets it, then
    //  unregisters `this` from the handle table
    //  (SHandleTableEntry::UnregisterAndNotify). Also declared `void` for
    //  the same "no explicit `mov eax`" reason as InitGrid above.
    void Teardown(int unused);

    //  0x1C - the opaque cell-storage sub-object (see SBaseMapCellStorage
    //  above). A real data member here would take its declaring struct's
    //  own `sizeof` (1 byte - it has no fields, only forward-declared
    //  methods), not the real object's own byte span at this offset, so it
    //  is reached through this accessor instead - the offset comment on
    //  `m_reserved04` above already accounts for the 0x14 bytes this
    //  actually occupies (0x1C-0x2F), folded into the reserved run below.
    SBaseMapCellStorage* Cells() { return (SBaseMapCellStorage*)((char*)this + 0x1C); }

    void*        m_vfptr;              // 0x00
    char         m_reserved04[0x2C];   // 0x04-0x2F, unmodeled (0x1C-0x2F is the opaque
                                        //             cell-storage sub-object, see Cells() above)
    unsigned int m_cellCount;          // 0x30 - width * height, zeroed by Teardown
    char         m_reserved34[4];      // 0x34, unmodeled
    unsigned int m_liveCellCount38;    // 0x38 - read (never written) by Teardown as the
                                        //        cell array's own shift count; whatever
                                        //        writes it lives outside this cluster's scope
    unsigned int m_width;              // 0x3C - zeroed by Teardown
    unsigned int m_height;             // 0x40 - zeroed by Teardown
    CFloat       m_originX;            // 0x44 - g_CFloatZero at InitGrid time
    CFloat       m_originY;            // 0x48 - g_CFloatZero at InitGrid time
    CFloat       m_extentY;            // 0x4C - height, converted to CFloat
    CFloat       m_extentX;            // 0x50 - width, converted to CFloat
    unsigned int m_reserved54;         // 0x54 - zeroed at InitGrid time
    unsigned int m_reserved58;         // 0x58 - zeroed at InitGrid time
    unsigned int m_heightInt;          // 0x5C - raw int copy of m_height
    unsigned int m_widthInt;           // 0x60 - raw int copy of m_width
    CFloat       m_scaleY;             // 0x64 - g_slopeThreshold1 (encoded 1.0) at InitGrid time
    CFloat       m_scaleX;             // 0x68 - g_slopeThreshold1 (encoded 1.0) at InitGrid time
};

//----------------------------------------------------------------------------
//  NIpse::CBaseMap<NIpse::CPmCell> - identical shape to CBaseMap<CPaCell>
//  above, independently compiled (see this file's own header comment on why
//  the two templates do not share addresses even for a slot that reads
//  like an identical no-op).
//----------------------------------------------------------------------------
class CBaseMap_CPmCell
{
public:
    //  0x6F49F370 - vtable slot 0.
    const char* GetTypeName();

    //  0x6F49F3E0 - vtable slot 1. Own static pool getter (0x6F49E0E0) also
    //  always returns null - same "only the concrete class pools" story as
    //  CBaseMap<CPaCell> above (NIpse::CProximityMap overrides this slot,
    //  proximitymap.h).
    void ReturnToStaticPool();

    //  0x6F49F460 - vtable slot 2, scalar deleting destructor.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F49F5D0 - vtable slot 3 (+0x0C). NOT shared by NIpse::CProximityMap
    //  the way CBaseMap<CPaCell>'s own slot 3 is shared by CAcceleratorMap -
    //  CProximityMap has its own distinct address here (0x6F4A04D0,
    //  proximitymap.h) that calls straight into this body first and then
    //  extends it. Same shape as CBaseMap<CPaCell>::InitGrid above, just
    //  the CPmCell-sized per-cell record (the callee's own internal element
    //  stride differs; nothing about this body's own instructions does).
    //  Declared `void` - see CBaseMap_CPaCell::InitGrid's own comment.
    void InitGrid(SHandleRegistrationRequest* request);

    //  0x6F49F670 - vtable slot 4 (+0x10). Reached both directly (this
    //  template's own vtable slot) and as the tail call CProximityMap's own
    //  slot-4 override (0x6F49FF20) makes once it has torn its own extra
    //  state down. Declared `void`, same reason as CBaseMap_CPaCell::Teardown.
    void Teardown(int unused);

    //  0x1C - see CBaseMap_CPaCell::Cells()'s own comment above.
    SBaseMapCellStorage* Cells() { return (SBaseMapCellStorage*)((char*)this + 0x1C); }

    void*        m_vfptr;              // 0x00
    char         m_reserved04[0x2C];   // 0x04-0x2F, unmodeled (0x1C-0x2F is the opaque
                                        //             cell-storage sub-object, see Cells() above)
    unsigned int m_cellCount;          // 0x30
    char         m_reserved34[4];      // 0x34
    unsigned int m_liveCellCount38;    // 0x38 - see CBaseMap_CPaCell's own field comment
    unsigned int m_width;              // 0x3C
    unsigned int m_height;             // 0x40
    CFloat       m_originX;            // 0x44
    CFloat       m_originY;            // 0x48
    CFloat       m_extentY;            // 0x4C
    CFloat       m_extentX;            // 0x50
    unsigned int m_reserved54;         // 0x54
    unsigned int m_reserved58;         // 0x58
    unsigned int m_heightInt;          // 0x5C
    unsigned int m_widthInt;           // 0x60
    CFloat       m_scaleY;             // 0x64
    CFloat       m_scaleX;             // 0x68
};

//  BaseMapCPaCell_GetStaticPool (0x6F497660) / BaseMapCPmCell_GetStaticPool
//  (0x6F49E0E0) - both always null - are declared in
//  Agent/agentbasechain_staticpools.h, not here: they must stay in a
//  different translation unit from their own ReturnToStaticPool callers
//  (Agent/basemap_cpacell.cpp/basemap_cpmcell.cpp), or this build's own
//  /O2 constant-folds the provably-null return straight through the pool
//  header access - see that header's own comment on the two getters for
//  the fix and why it was needed.

}  // namespace NIpse

#endif  // BASEMAP_H
