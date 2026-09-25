//============================================================================
//  NIpse::CAcceleratorMap (`??_7CAcceleratorMap@NIpse@@6B@`, 0x6F9526E4,
//  17 slots) : public NIpse::CBaseMap<NIpse::CPaCell> - see basemap.h's own
//  file header for how the inheritance edge (and the fact that this class
//  never overrides vtable slots 3/4) was established from raw disassembly.
//  Flat, offset-addressed placeholder, same convention as basemap.h.
//============================================================================
#ifndef ACCELERATORMAP_H
#define ACCELERATORMAP_H

namespace NIpse {

class CAcceleratorMap
{
public:
    //  0x6F4888F0 - vtable slot 0.
    const char* GetTypeName();

    //  0x6F488960 - vtable slot 1. Own real static pool this time (unlike
    //  CBaseMap<CPaCell>'s own always-null one) - see
    //  AcceleratorMap_GetStaticPool below.
    void ReturnToStaticPool();

    //  0x6F4889B0 - vtable slot 2, scalar deleting destructor. Calls the
    //  SAME real base destructor (0x6F486A40) CBaseMap<CPaCell>'s own slot 2
    //  calls (basemap_cpacell.cpp) - this class adds no further destructible
    //  state of its own.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  Vtable slots 3/4 (+0x0C/+0x10) are NOT overridden here - see
    //  basemap.h's own file header. No CAcceleratorMap::InitGrid/Teardown
    //  exists; the shipped vtable entry is the literal address of
    //  NIpse::CBaseMap_CPaCell::InitGrid/Teardown.

    void*        m_vfptr;              // 0x00 - shared layout with CBaseMap<CPaCell>
    char         m_reserved04[0x6C];   // 0x04-0x6F, unmodeled beyond basemap.h's own fields
};

//  0x6F488900 / 0x6F488910 - vtable slots 9/10 (+0x24/+0x28), no-ops. Own
//  addresses, distinct from CBaseMap<CPaCell>'s own copies of the same
//  trivial body (basemap.h's own SBaseMapNoOpArg comment already explains
//  why neither template shares this slot's address with its own concrete
//  derived class).
struct SAcceleratorMapNoOpArg
{
    void NoOp_6F488900(void* /*unused*/);
    void NoOp_6F488910(void* /*unused*/);
};

}  // namespace NIpse

#endif  // ACCELERATORMAP_H
