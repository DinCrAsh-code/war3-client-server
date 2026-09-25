//============================================================================
//  0x6F3DA8B0 - CCameraSetup::Method_0x24(int context), vtable slot 9.  The
//  unregistration twin of Method_0x20 (camerasetupregisterproperties.cpp):
//  the same seventeen properties in the same order, each unregistered
//  through vtable slot 3 (+0x0C/4) with only (id, context) - no float
//  pointers, no flag, no count.  Same order quirk as the register side:
//  +0x84 (property 6) comes before +0x64/+0x74 (properties 4/5).
//============================================================================
#include "camerasetup.h"

static __forceinline void UnregisterProp(SUnitTrackedRef* prop, int id,
                                         int context)
{
    typedef void (__thiscall *Fn)(void*, int, int);
    ((Fn)(*(void***)prop)[0x0C / 4])(prop, id, context);
}

int CCameraSetup::Method_0x24(int context)
{
    //  sub_6F471BF0 - CAgent::Method_0x24(context), always 0.
    int id = CAgent::Method_0x24(context);

    UnregisterProp(&m_prop0, id, context);   ++id;
    UnregisterProp(&m_prop1, id, context);   ++id;
    UnregisterProp(&m_prop2, id, context);   ++id;
    UnregisterProp(&m_prop3, id, context);   ++id;
    UnregisterProp(&m_prop6, id, context);   ++id;
    UnregisterProp(&m_prop4, id, context);   ++id;
    UnregisterProp(&m_prop5, id, context);   ++id;
    UnregisterProp(&m_prop7, id, context);   ++id;
    UnregisterProp(&m_prop8, id, context);   ++id;
    UnregisterProp(&m_prop9, id, context);   ++id;
    UnregisterProp(&m_prop10, id, context);  ++id;
    UnregisterProp(&m_prop11, id, context);  ++id;
    UnregisterProp(&m_prop12, id, context);  ++id;
    UnregisterProp(&m_prop13, id, context);  ++id;
    UnregisterProp(&m_prop14, id, context);  ++id;
    UnregisterProp(&m_prop15, id, context);  ++id;
    UnregisterProp(&m_prop16, id, context);  ++id;

    return id;
}
