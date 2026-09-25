//============================================================================
//  0x6F3022B0 - CCameraWar3::Method_0x24(int context), vtable slot 9.  The
//  unregistration twin of Method_0x20 (cameraregisterproperties.cpp):
//  walks the exact same fourteen offsets in the exact same order (the nine
//  SCameraProp fields, m_targetZ, m_panZ, the two Position::RecordSlot8
//  receivers and m_localAngle) and calls vtable slot 3 (+0x0C/4) of each
//  with only (id, context) - no float pointers, no flag word, no count.
//  See cameraregisterproperties.cpp's own header for why the receiver is
//  the sub-object's own vtable pointer rather than `this`.
//============================================================================
#include "gameui.h"
#include "position.h"

static __forceinline void UnregisterProp(SUnitTrackedRef* prop, int id,
                                         int context)
{
    typedef void (__thiscall *Fn)(void*, int, int);
    ((Fn)(*(void***)prop)[0x0C / 4])(prop, id, context);
}

int CCameraWar3::Method_0x24(int context)
{
    //  sub_6F471BF0 - CAgent::Method_0x24(context), always 0.
    int id = CAgent::Method_0x24(context);

    UnregisterProp(&m_listenerDistance.m_value, id, context);   ++id;
    UnregisterProp(&m_listenerAngle.m_value,    id, context);   ++id;
    UnregisterProp(&m_targetDistance.m_value,   id, context);   ++id;
    UnregisterProp(&m_farZ.m_value,             id, context);   ++id;
    UnregisterProp(&m_nearZ.m_value,            id, context);   ++id;
    UnregisterProp(&m_fieldOfView.m_value,      id, context);   ++id;
    UnregisterProp(&m_roll.m_value,             id, context);   ++id;
    UnregisterProp(&m_rotation.m_value,         id, context);   ++id;
    UnregisterProp(&m_angleOfAttack.m_value,    id, context);   ++id;
    UnregisterProp(&m_targetZ.m_value,          id, context);   ++id;
    UnregisterProp(&m_panZ.m_value,             id, context);   ++id;

    typedef void (__thiscall *Fn)(void*, int, int);
    ((Fn)(*(void***)((char*)this + 0x468))[0x0C / 4])(
        (char*)this + 0x468, id, context);
    ++id;
    ((Fn)(*(void***)&m_target)[0x0C / 4])(&m_target, id, context);
    ++id;

    UnregisterProp(&m_localAngle.m_value,       id, context);   ++id;

    return id;
}
