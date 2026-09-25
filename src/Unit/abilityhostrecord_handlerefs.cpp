//============================================================================
//  0x6F2CFEB0 / 0x6F2CFED0 / 0x6F2D6500 - three more
//  SAbilityHostRecord handle-ref accessors (abilityhostrecord.h), the same
//  sentinel-gated {handle,typeTag}-pair shape
//  Misc/misc_handle_resolvers.cpp's SHandleRefAtXXXOwner family already
//  catalogues, at three different fixed offsets - raw offset casts rather
//  than the named SOptionalHandleRef member, which is what keeps `this`
//  itself (not a computed field address) live across the null test.
//============================================================================
#include "abilityhostrecord.h"
#include "itemhandleresolve.h"
#include "agenttypedslots.h"

void* SAbilityHostRecord::GetField0x54At0x2AC()
{
    int typeTag = *(int*)((char*)this + 0x2B0);
    unsigned int handle = *(unsigned int*)((char*)this + 0x2AC);
    if ((typeTag & (int)handle) == -1)
        return 0;
    return QueryHandleField0x54((SOptionalHandleRef*)((char*)this + 0x2AC));
}

void* SAbilityHostRecord::GetField0x54At0x2B8()
{
    int typeTag = *(int*)((char*)this + 0x2BC);
    unsigned int handle = *(unsigned int*)((char*)this + 0x2B8);
    if ((typeTag & (int)handle) == -1)
        return 0;
    return QueryHandleField0x54((SOptionalHandleRef*)((char*)this + 0x2B8));
}

CAgent* SAbilityHostRecord::GetUnitIfResolvedAt0x2C4()
{
    int typeTag = *(int*)((char*)this + 0x2C8);
    unsigned int handle = *(unsigned int*)((char*)this + 0x2C4);
    void* resolved = ((typeTag & (int)handle) == -1)
                      ? 0 : QueryHandleField0x54((SOptionalHandleRef*)((char*)this + 0x2C4));
    SCheckedUnitSlot checked((CAgent*)resolved);
    return checked.m_value;
}
