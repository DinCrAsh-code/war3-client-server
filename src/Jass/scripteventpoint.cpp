//============================================================================
//  CScriptEventData's three remaining accessors: the order's target point
//  and the event's ability.
//
//      0x6F26FD30  GetOrderPointX   0x6F26FD60  GetOrderPointY
//      0x6F26FDB0  GetAbility
//
//  One translation unit - all three are in the 0x6F26Fxxx module and none
//  calls another.  GetOrder (0x6F26FD10) is deliberately elsewhere; see
//  scripteventorder.cpp.
//
//  The point is copied through a named local and then assigned out, which is
//  what the shipped `mov [esp+var_4],eax` / `lea eax,[esp+var_4]` /
//  `mov ecx,[eax]` round trip is: CFloat's copy constructor into the local
//  and its assignment out of it, both by reference.  Storing straight
//  through the out pointer loses the round trip, and the "no order" arm's
//  `mov eax, offset g_CFloatZero` / `mov ecx,[eax]` is the same assignment
//  with a global on the right.
//============================================================================
#include "jassevents.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F26FD30 - +0x48 of the order.
//----------------------------------------------------------------------------
CFloat* CScriptEventData::GetOrderPointX(CFloat* out)
{
    SEventOrder* order = GetOrder();

    const CFloat* src;
    CFloat value(kCFloatNoInit);

    if (order)
    {
        value = order->m_pointX;
        src = &value;
    }
    else
    {
        src = &g_CFloatZero;
    }

    *out = *src;

    return out;
}

//----------------------------------------------------------------------------
//  0x6F26FD60 - +0x50 of the same order.
//----------------------------------------------------------------------------
CFloat* CScriptEventData::GetOrderPointY(CFloat* out)
{
    SEventOrder* order = GetOrder();

    const CFloat* src;
    CFloat value(kCFloatNoInit);

    if (order)
    {
        value = order->m_pointY;
        src = &value;
    }
    else
    {
        src = &g_CFloatZero;
    }

    *out = *src;

    return out;
}

//----------------------------------------------------------------------------
//  0x6F26FDB0 - the +0x2C handle ref resolved, same inlined guard and tail
//  jump as jasseventowner.cpp's pair.
//----------------------------------------------------------------------------
void* CScriptEventData::GetAbility()
{
    if ((m_abilityTypeTag & m_abilityHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_abilityHandle);
}
