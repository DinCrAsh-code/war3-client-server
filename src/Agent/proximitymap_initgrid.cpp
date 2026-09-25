//============================================================================
//  NIpse::CProximityMap::InitGrid (0x6F4A04D0, vtable slot 3, +0x0C). See
//  proximitymap.h for the field/sub-object layout this reaches into.
//============================================================================
#include "proximitymap.h"
#include "basemap.h"       // NIpse::CBaseMap_CPmCell
#include "residuekey.h"    // SResidueKey::Set (0x6F4AB660, already reconstructed)

//  g_pTimeSync (0x6FAB73D8, tools/funcmap.py's own DATA row) - forward-
//  declared against an incomplete `CTimeSync` rather than via
//  `#include "game.h"`, same trick as Agent/acceleratormap_returntopool.cpp
//  (a pointer's own C++ mangling only encodes its pointee's name, not its
//  full definition, so this binds to the same symbol without pulling in
//  game.h's own conflicting `SHandleTable`). Declared at FILE scope, not
//  inside `namespace NIpse` below: the real global is global-scope, and a
//  namespace here would mangle to a different (unresolved) symbol -
//  `link_check.py` caught exactly this the first time this file was
//  written (`?g_pTimeSync@NIpse@@...` instead of `?g_pTimeSync@@...`).
class CTimeSync;
extern CTimeSync* g_pTimeSync;

//  Two more globals this call tags the registration with - `g_unk6FAB7764`
//  (new DATA row this session added) and `g_unk6F95344C`, this class's own
//  type-descriptor-shaped data right after its own vtable (agent_worktrees/
//  classes/0x6F9534CC.json). Neither has a real name yet beyond that, so
//  both stay `g_unk`-prefixed per this codebase's own convention for an
//  unresolved global (tools/funcmap.py's own DATA table comment block) -
//  out of this session's own scope to resolve further. Also file scope,
//  same reason as g_pTimeSync above (these are new globals introduced by
//  this session, not previously namespaced anywhere, so it costs nothing
//  to declare them the same way every other funcmap.DATA global in this
//  codebase already is).
extern char g_unk6FAB7764;
extern char g_unk6F95344C;

namespace NIpse {

//  0x6F4A04D0 - vtable slot 3 (+0x0C).
void CProximityMap::InitGrid(SHandleRegistrationRequest* request)
{
    ((CBaseMap_CPmCell*)this)->InitGrid(request);

    CBaseMap_CPmCell* base = (CBaseMap_CPmCell*)this;
    unsigned int area = base->m_height * base->m_width;
    unsigned int extra0C = *(unsigned int*)((char*)request + 0x0C);

    unsigned int bucketCount = (area >> 5) + 1;
    *(unsigned int*)((char*)this + 0xA0) = bucketCount;

    unsigned int zeroedDefault = 0;
    ((SProximityMapSubContainer*)((char*)this + 0x8C))->Resize_6F49ED70(&zeroedDefault, bucketCount);

    ((SResidueKey*)((char*)this + 0xBC))->Set(extra0C);

    *(unsigned int*)((char*)this + 0xAC) = 0xFFFFFF;
    *(unsigned int*)((char*)this + 0xB4) = 0;

    void* handle = ((SProximityMapRegistry*)((char*)(void*)g_pTimeSync + 0x164))
        ->Register_6F4A0460(this, &g_unk6F95344C, &g_unk6FAB7764);
    *(void**)((char*)this + 0xB8) = handle;
    *(unsigned int*)((char*)handle + 0x10) |= 1;
}

}  // namespace NIpse
