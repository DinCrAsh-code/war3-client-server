//============================================================================
//  NIpse::CProximityMap (`??_7CProximityMap@NIpse@@6B@`, 0x6F9534CC,
//  17 slots) : public NIpse::CBaseMap<NIpse::CPmCell> - see basemap.h's own
//  file header for the inheritance evidence (this class's own slot 3/4
//  bodies below open/close by calling straight into
//  NIpse::CBaseMap_CPmCell::InitGrid/Teardown). Flat, offset-addressed
//  placeholder, same convention as basemap.h.
//
//  This class adds three more sub-objects beyond CBaseMap<CPmCell>'s own
//  cell storage (this+0x1C): one at +0x6C (own count field at its own
//  +0x1C, i.e. absolute +0x88 - same "Resize/Shift/Reset" container shape,
//  torn down in Teardown but never touched by InitGrid, so presumably set
//  up by this class's own constructor, out of this session's scope), a
//  bucket table at +0x8C (own count field at absolute +0xA8, an estimate of
//  it cached separately at +0xA0 - both InitGrid and Teardown touch this
//  one), and a real `SResidueKey` (Misc/residuekey.h, already
//  reconstructed - the same "stamp a value's four-modulus residue
//  signature" primitive CRandData's own history buffer uses) at +0xBC
//  (InitGrid only). Neither opaque sub-object is modeled as a real member -
//  same reasoning as basemap.h's own Cells() accessor (an opaque
//  forward-declared receiver type has no real data members of its own, so
//  embedding one *by value* would take that type's own `sizeof`, not the
//  real object's actual byte span, and corrupt every field offset declared
//  after it) - each is reached through a `(char*)this + <offset>` cast at
//  the call site instead.
//============================================================================
#ifndef PROXIMITYMAP_H
#define PROXIMITYMAP_H

#include "handletable.h"   // SHandleRegistrationRequest

namespace NIpse {

//----------------------------------------------------------------------------
//  Out-of-scope callees CProximityMap's own slot 3/4 bodies reach into -
//  same convention as basemap.h's own SBaseMapRealDtorTarget/
//  SBaseMapCellStorage (forward-declared only, so this translation unit
//  compiles under `verify.py`, which never links).
//----------------------------------------------------------------------------

//  The +0x6C and +0x8C sub-objects (see this file's own header comment) -
//  same "Resize/Shift/Reset" shape as basemap.h's own SBaseMapCellStorage,
//  reached through a different pair of addresses (this class's own module,
//  not CBaseMap<CPmCell>'s).
struct SProximityMapSubContainer
{
    void Resize_6F49ED70(const void* defaultElement, unsigned int count);   // +0x8C only (InitGrid)
    int  Shift_6F49EED0(unsigned int index, unsigned int count);            // +0x6C (Teardown)
    int  Shift_6F49EDF0(unsigned int index, unsigned int count);            // +0x8C (Teardown)
    void Reset_6F49EBD0();                                                  // +0x6C (Teardown)
    void Reset_6F49EB40();                                                  // +0x8C (Teardown)
};

//  0x6F4A0460 - registers `mapObject` (tagged with the type descriptor
//  `unk_6F95344C`, plus one more untyped global, `dword_6FAB7764`) into
//  whatever registry lives at `g_pTimeSync + 0x164`, returning a handle
//  this class stores at +0xB8.
struct SProximityMapRegistry
{
    void* Register_6F4A0460(void* mapObject, void* typeTag, void* extra);
};

//  0x6F49F260 - called with no arguments at the very start of Teardown,
//  return value discarded; real purpose unconfirmed from here (out of this
//  session's own scope).
struct SProximityMapTeardownPrelude
{
    void Flush_6F49F260();
};

class CProximityMap
{
public:
    //  0x6F49FB80 - vtable slot 0.
    const char* GetTypeName();

    //  0x6F49FBF0 - vtable slot 1. Own real static pool (Agent/
    //  agentbasechain_staticpools.h's own ProximityMap_GetStaticPool).
    void ReturnToStaticPool();

    //  0x6F49FCA0 - vtable slot 2, scalar deleting destructor. Calls its
    //  own real destructor (0x6F49FC30, out of this session's scope - it
    //  tears down two sub-objects of its own before calling into
    //  NIpse::CBaseMap_CPmCell's own real destructor, 0x6F49F190) rather
    //  than CBaseMap_CPmCell's real destructor directly - unlike
    //  NIpse::CAcceleratorMap, which has no further state to add and so
    //  reuses CBaseMap<CPaCell>'s own real destructor unchanged
    //  (acceleratormap.h).
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F4A04D0 - vtable slot 3 (+0x0C). Calls straight into
    //  NIpse::CBaseMap_CPmCell::InitGrid first (this is the call that
    //  proves the inheritance edge - see basemap.h's own file header), then
    //  derives a bucket-table size from the grid area the base call just
    //  established (height * width, re-read from the fields InitGrid just
    //  wrote rather than passed through), resizes its own +0x8C bucket
    //  table to `area / 32 + 1` entries, stamps its own +0xBC `SResidueKey`
    //  auxiliary state from `request`'s own +0x0C field, and finally
    //  registers itself into the global registry at `g_pTimeSync + 0x164`.
    void InitGrid(SHandleRegistrationRequest* request);

    //  0x6F49FF20 - vtable slot 4 (+0x10). Tears its own added state down
    //  (a no-argument flush call, the pending-registration bit on whatever
    //  +0xB8 still points to, then the +0x6C and +0x8C sub-containers, each
    //  only if their own count field is nonzero) before tail-calling
    //  NIpse::CBaseMap_CPmCell::Teardown for the inherited part.
    void Teardown(int unused);

    void* m_vfptr;   // 0x00 - shared layout with CBaseMap<CPmCell>
};

}  // namespace NIpse

#endif  // PROXIMITYMAP_H
