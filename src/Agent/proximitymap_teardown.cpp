//============================================================================
//  NIpse::CProximityMap::Teardown (0x6F49FF20, vtable slot 4, +0x10). See
//  proximitymap.h for the field/sub-object layout this reaches into.
//============================================================================
#include "proximitymap.h"
#include "basemap.h"   // NIpse::CBaseMap_CPmCell

namespace NIpse {

//  0x6F49FF20 - vtable slot 4 (+0x10).
void CProximityMap::Teardown(int unused)
{
    ((SProximityMapTeardownPrelude*)this)->Flush_6F49F260();

    void* registration = *(void**)((char*)this + 0xB8);
    *(unsigned int*)((char*)registration + 0x10) |= 0x10000;
    *(void**)((char*)this + 0xB8) = 0;

    SProximityMapSubContainer* subA = (SProximityMapSubContainer*)((char*)this + 0x6C);
    unsigned int countA = *(unsigned int*)((char*)this + 0x88);
    if (countA != 0)
        subA->Shift_6F49EED0(0, countA);
    subA->Reset_6F49EBD0();

    SProximityMapSubContainer* subB = (SProximityMapSubContainer*)((char*)this + 0x8C);
    unsigned int countB = *(unsigned int*)((char*)this + 0xA8);
    if (countB != 0)
        subB->Shift_6F49EDF0(0, countB);
    subB->Reset_6F49EB40();

    ((CBaseMap_CPmCell*)this)->Teardown(unused);
}

}  // namespace NIpse
