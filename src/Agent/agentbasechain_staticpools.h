//============================================================================
//  Four one-instruction leaves the base-chain's own `ReturnToStaticPool`
//  vtable slots each call once: `mov eax, <global>; add eax, <offset>;
//  retn` - no `this` read at all, just "here is where this class's own
//  static free-list pool header lives".  Same fixed-block free-list shape
//  CAgentBaseAbs's own pool getter already establishes
//  (Agent/cagentbaseabs_module_batch2.cpp's `AgentBase_GetOrCreatePool` /
//  `SAgentBasePool`, +0x14 free-list head, +0x18 free count) - reused here
//  as `SAgentChainPoolHeader` rather than duplicated per file, though unlike
//  that one these four are eager offsets into an existing global rather
//  than a lazily-allocated block, so there is no allocate-on-first-use path
//  to reconstruct.
//
//  `g_pTimeSync` (CTimeSync*, Game/game.h, 0x6FAB73D8) and `g_unk6FAB778C`
//  (RTTI-confirmed elsewhere as a live NTempest::CDynTable<CPrRgEntry> -
//  see docs/notes/checksum-provider-registry.md - declared `void*` here the
//  same way every other user of it in this codebase does) each apparently
//  hold more than one class's own static pool at a fixed byte offset; only
//  the offset each leaf actually reads is named, nothing about either
//  global's own full layout.
//
//  Own translation unit, deliberately: each of these is called from a
//  different one of this batch's own `ReturnToStaticPool` bodies
//  (Agent/presence_returntopool.cpp, Agent/presencetagged_returntopool.cpp,
//  Agent/agentrelation_returntopool.cpp, Pathfinding/crlagent_returntopool.cpp),
//  and the real dump shows each call staying out of line - CLAUDE.md's own
//  "put a callee in its own TU" rule.
//============================================================================
#ifndef AGENTBASECHAIN_STATICPOOLS_H
#define AGENTBASECHAIN_STATICPOOLS_H

//  Same shape as Agent/cagentbaseabs_module_batch2.cpp's own
//  SAgentBasePool: only the two fields any of this batch's own
//  ReturnToStaticPool bodies touch are named.
struct SAgentChainPoolHeader
{
    char            m_reserved00[0x14];
    void*           m_freeListHead;      // +0x14
    unsigned int    m_freeCount;         // +0x18
};

//  0x6F4B7A10 - NTempest::CPresence's own static pool: g_unk6FAB778C + 0x5C.
void* CPresence_GetStaticPool();

//  0x6F4AEE50 - NTempest::CPresenceTagged's own static pool: g_unk6FAB778C + 0x9C.
void* CPresenceTagged_GetStaticPool();

//  0x6F4A5EB0 - NIpse::CPrRelation's own static pool: g_pTimeSync + 0x5F8.
void* CPrRelation_GetStaticPool();

//  0x6F4A45D0 - NIpse::CRlAgent's own static pool: g_pTimeSync + 0x6B8.
void* CRlAgent_GetStaticPool();

//  0x6F497430 - NIpse::CRlProperty's own static pool: g_pTimeSync + 0x698.
//  Same one-instruction-leaf shape as the four above (NTempestNIpse_BaseChain
//  cluster A batch, 2026-09-08) - reused here by raw address rather than a
//  fifth naked thunk, per the same convention.
void* CRlProperty_GetStaticPool();

//  0x6F4A4740 - NIpse::CRlAgentDef's own static pool: g_pTimeSync + 0x6D8.
void* CRlAgentDef_GetStaticPool();

//  0x6F494DC0 - NIpse::CPoSeparate's own static pool: g_pTimeSync + 0x7F8.
//  Same one-instruction leaf shape as the others above (NTempestNIpse
//  cluster H batch, 2026-09-08) - CPoSeparate is a different, deeper
//  positioning class (Agent/poseparate.h), not part of this file's
//  original base-chain batch.
void* CPoSeparate_GetStaticPool();

//  0x6F4AA330 - NIpse::CPrBehavior's own static pool: g_pTimeSync + 0x618.
//  Same one-instruction leaf shape as the others above (NTempestNIpse
//  cluster B batch, 2026-09-08 - docs/targets/NTempestNIpse_CPrBehavior.md).
void* CPrBehavior_GetStaticPool();

//  Three more of the same one-instruction shape, added for the
//  NIpse::CPrCluster / CPrClusterGen / NIpse::CPrClusterGroup clustering
//  family's own idx1 ReturnToStaticPool bodies (Agent/prcluster.h) - these
//  three classes' vtables restamp straight to NTempest::CPresence's own on
//  destruction (Agent/prcluster.h's own header comment), so they sit off
//  g_pTimeSync exactly like CPrRelation/CRlAgent above rather than off
//  g_unk6FAB778C the way CPresence/CPresenceTagged do.

//  0x6F48ED20 - NIpse::CPrClusterGroup's own static pool: g_pTimeSync + 0x658.
void* CPrClusterGroup_GetStaticPool();

//  0x6F48EE30 - NIpse::CPrClusterGen's own static pool: g_pTimeSync + 0x638.
void* CPrClusterGen_GetStaticPool();

//  0x6F48F260 - NIpse::CPrCluster's own static pool: g_pTimeSync + 0x678.
void* CPrCluster_GetStaticPool();

//  Five more, for the "spatial map" cluster's own idx1 ReturnToStaticPool
//  bodies (Agent/basemap.h/acceleratormap.h/proximitymap.h/pmregion.h) -
//  NIpse::CAcceleratorMap, NIpse::CProximityMap, NIpse::CPmRegion, and the
//  two templates underneath the first two (NIpse::CBaseMap<CPaCell>/
//  <CPmCell> - their own getters, 0x6F497660/0x6F49E0E0, always return
//  null; see each one's own comment below for why they live here and not
//  alongside their own ReturnToStaticPool bodies).

//  0x6F497CF0 - NIpse::CAcceleratorMap's own static pool: g_pTimeSync + 0x5B8.
void* AcceleratorMap_GetStaticPool();

//  0x6F49E840 - NIpse::CProximityMap's own static pool: g_pTimeSync + 0x598.
void* ProximityMap_GetStaticPool();

//  0x6F49E620 - NIpse::CPmRegion's own static pool: g_pTimeSync + 0x5D8.
void* PmRegion_GetStaticPool();

//  0x6F497660 - NIpse::CBaseMap<CPaCell>'s own static pool getter: always
//  null (nothing pools a bare CBaseMap<CPaCell> directly - only the
//  concrete NIpse::CAcceleratorMap does, via its own real getter above).
//  Belongs here, not alongside CBaseMap<CPaCell>'s own ReturnToStaticPool
//  (Agent/basemap_cpacell.cpp), for the same "own translation unit" reason
//  this whole file exists for: the real dump keeps the call out of line
//  even though the callee is trivial, and a same-TU `return 0;` this
//  `/O2` build WOULD constant-fold and inline - which is exactly what
//  happened before this fix (ReturnToStaticPool scored ~0.2, its whole
//  pool-header access folded to two absolute-address stores instead of a
//  real indirection through a null pointer it cannot prove is null across
//  a translation unit boundary).
void* BaseMapCPaCell_GetStaticPool();

//  0x6F49E0E0 - NIpse::CBaseMap<CPmCell>'s own static pool getter: same
//  story as BaseMapCPaCell_GetStaticPool above.
void* BaseMapCPmCell_GetStaticPool();

//  0x6F4AA1F0 - NIpse::CPoLkLis's own static pool: g_pTimeSync + 0x8F8
//  (NTempestNIpse "Po-modifier/listener" cluster F, 2026-09-08). Same
//  one-instruction leaf shape as the others above - kept in this shared
//  file rather than alongside its own class (Agent/cpolklis.h) because the
//  real dump shows CPoLkLis::ReturnToStaticPool calling it out of line, and
//  putting a caller and callee this small in the same translation unit lets
//  /Ob2 inline the call away (see Agent/cpolklis.h's own class file for the
//  ReturnToStaticPool body this getter serves).
void* CPoLkLis_GetStaticPool();

//  0x6F4A8C70 - NIpse::CPoFgLis's own static pool: g_pTimeSync + 0x8B8
//  (NTempestNIpse "Po-modifier/listener" cluster F, 2026-09-08). Same shape
//  as CPoLkLis_GetStaticPool above.
void* CPoFgLis_GetStaticPool();

//  Seven more of the same one-instruction leaf shape, for the remaining
//  seven siblings of the "Po-modifier/listener" cluster F family
//  (Agent/cpoflag.h, cpolink.h, cporeal.h, cporecontinuousmod.h,
//  cpofgpulsemod.h, cporethresholdlis.h, cporepulsemod.h) - cluster F's own
//  follow-up session, 2026-09-08.

//  0x6F4A8950 - NIpse::CPoFlag's own static pool: g_pTimeSync + 0x878.
void* CPoFlag_GetStaticPool();

//  0x6F4AA090 - NIpse::CPoLink's own static pool: g_pTimeSync + 0x8D8.
void* CPoLink_GetStaticPool();

//  0x6F4A90A0 - NIpse::CPoReal's own static pool: g_pTimeSync + 0x6F8.
void* CPoReal_GetStaticPool();

//  0x6F4A9510 - NIpse::CPoReContinuousMod's own static pool: g_pTimeSync + 0x718.
void* CPoReContinuousMod_GetStaticPool();

//  0x6F4A8A60 - NIpse::CPoFgPulseMod's own static pool: g_pTimeSync + 0x898.
void* CPoFgPulseMod_GetStaticPool();

//  0x6F4A9860 - NIpse::CPoReThresholdLis's own static pool: g_pTimeSync + 0x758.
void* CPoReThresholdLis_GetStaticPool();

//  0x6F4A95F0 - NIpse::CPoRePulseMod's own static pool: g_pTimeSync + 0x738.
void* CPoRePulseMod_GetStaticPool();

//  Three more of the same one-instruction leaf shape, for the "PoPos*"
//  cluster (Agent/popos.h/poposbh.h/poposcl.h - CPoSeparate's own
//  cluster-H follow-up, 2026-09-08).

//  0x6F4A6D60 - NIpse::CPoPos's own static pool: g_pTimeSync + 0x778.
void* CPoPos_GetStaticPool();

//  0x6F4AAA20 - NIpse::CPoPosBh's own static pool: g_pTimeSync + 0x818.
void* CPoPosBh_GetStaticPool();

//  0x6F493C80 - NIpse::CPoPosCl's own static pool: g_pTimeSync + 0x7D8.
void* CPoPosCl_GetStaticPool();

//  Two more of the same one-instruction leaf shape, for the
//  NIpse::CPoPoInterfLis/CPoPoVelocityMod sibling pair
//  (Agent/cpopointerflis.h/cpopovelocitymod.h) branching directly off
//  NIpse::CPrRelation.

//  0x6F4A78D0 - NIpse::CPoPoInterfLis's own static pool: g_pTimeSync + 0x7B8.
void* CPoPoInterfLis_GetStaticPool();

//  0x6F4A77C0 - NIpse::CPoPoVelocityMod's own static pool: g_pTimeSync + 0x798.
void* CPoPoVelocityMod_GetStaticPool();

#endif  // AGENTBASECHAIN_STATICPOOLS_H
