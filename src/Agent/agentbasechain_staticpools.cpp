//============================================================================
//  See agentbasechain_staticpools.h.  Four one-instruction pool-address
//  leaves, each `mov eax, <global>; add eax, <offset>; retn` with no `this`
//  read.
//============================================================================
#include "agentbasechain_staticpools.h"
#include "game.h"

//  Declared locally as `void*` the same way every other user of this global
//  in this codebase does (Net/checksumproviderdebugprint.cpp,
//  Net/netcommand_dispatch.cpp, Net/tempestproviderlisthash.cpp,
//  Pathfinding/gridregistrationteardown.cpp) - RTTI-confirmed elsewhere as a
//  live NTempest::CDynTable<CPrRgEntry>, not modelled as one here.
extern void* g_unk6FAB778C;

//  0x6F4B7A10
void* CPresence_GetStaticPool()
{
    return (char*)g_unk6FAB778C + 0x5C;
}

//  0x6F4AEE50
void* CPresenceTagged_GetStaticPool()
{
    return (char*)g_unk6FAB778C + 0x9C;
}

//  0x6F4A5EB0
void* CPrRelation_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x5F8;
}

//  0x6F4A45D0
void* CRlAgent_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x6B8;
}

//  0x6F497430 - NIpse::CRlProperty's own (NTempestNIpse_BaseChain cluster A,
//  2026-09-08).
void* CRlProperty_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x698;
}

//  0x6F4A4740 - NIpse::CRlAgentDef's own.
void* CRlAgentDef_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x6D8;
}

//  0x6F494DC0 - NIpse::CPoSeparate's own (NTempestNIpse cluster H, 2026-09-08).
void* CPoSeparate_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x7F8;
}

//  0x6F4AA330 - NIpse::CPrBehavior's own (NTempestNIpse cluster B, 2026-09-08).
void* CPrBehavior_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x618;
}

//  0x6F48ED20
void* CPrClusterGroup_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x658;
}

//  0x6F48EE30
void* CPrClusterGen_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x638;
}

//  0x6F48F260
void* CPrCluster_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x678;
}

//  0x6F497CF0 - NTempestNIpse "spatial map" cluster (2026-09-08).
void* AcceleratorMap_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x5B8;
}

//  0x6F49E840
void* ProximityMap_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x598;
}

//  0x6F49E620
void* PmRegion_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x5D8;
}

//  0x6F4AA1F0 - NIpse::CPoLkLis's own (NTempestNIpse cluster F, 2026-09-08).
void* CPoLkLis_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x8F8;
}

//  0x6F4A8C70 - NIpse::CPoFgLis's own (NTempestNIpse cluster F, 2026-09-08).
void* CPoFgLis_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x8B8;
}

//  Cluster F's own follow-up session (2026-09-08): the remaining seven
//  "Po-modifier/listener" siblings.

//  0x6F4A8950
void* CPoFlag_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x878;
}

//  0x6F4AA090
void* CPoLink_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x8D8;
}

//  0x6F4A90A0
void* CPoReal_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x6F8;
}

//  0x6F4A9510
void* CPoReContinuousMod_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x718;
}

//  0x6F4A8A60
void* CPoFgPulseMod_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x898;
}

//  0x6F4A9860
void* CPoReThresholdLis_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x758;
}

//  0x6F4A95F0
void* CPoRePulseMod_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x738;
}

//  0x6F497660
void* BaseMapCPaCell_GetStaticPool()
{
    return 0;
}

//  0x6F49E0E0
void* BaseMapCPmCell_GetStaticPool()
{
    return 0;
}

//  0x6F4A6D60
void* CPoPos_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x778;
}

//  0x6F4AAA20
void* CPoPosBh_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x818;
}

//  0x6F493C80
void* CPoPosCl_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x7D8;
}

//  0x6F4A78D0 - NIpse::CPoPoInterfLis's own.
void* CPoPoInterfLis_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x7B8;
}

//  0x6F4A77C0 - NIpse::CPoPoVelocityMod's own.
void* CPoPoVelocityMod_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x798;
}
