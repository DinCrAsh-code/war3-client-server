//============================================================================
//  Two more instantiations of the "type-checked smart pointer to a
//  CAgent-derived object" family (agenttypedslots.h) - reached from
//  CUnit::NotifyAbilityHostChanged's own closure (0x6F2E52F0,
//  unit_abilityhostnotify.cpp), which calls the resolved ability-host
//  object's own Notify_6F2DBB90(this) - previously a self-redirect thunk,
//  now sub_6F2DBB90's real body (cunit_abilityhostnotifytarget.cpp).
//
//  Neither required constant decodes to a member of the known agile-type-id
//  family (misc_rawcode_getters.cpp's own '+w3x'/'item' list), so - per
//  that file's own rule for a constant with no established meaning - both
//  the getter and the slot type keep an address-derived name:
//
//      0x6F26C3E0  "uref" (0x75726566)  ->  sub_6F085E40  SCheckedSlot_6F26C3E0::Assign
//                                            sub_6F095DD0  SCheckedSlot_6F26C3E0::SCheckedSlot_6F26C3E0
//      0x6F2E59B0  "AI-U" (0x41492D55)  ->  sub_6F0C58F0  SCheckedSlot_6F2E59B0::Assign
//                                            sub_6F0C76E0  SCheckedSlot_6F2E59B0::SCheckedSlot_6F2E59B0
//============================================================================
#ifndef CHECKEDSLOT_CREATEUNIT_H
#define CHECKEDSLOT_CREATEUNIT_H

#include "agent.h"

struct SCheckedSlot_6F26C3E0
{
    //  0x6F095DD0 - checkedslot_createunit_ctor.cpp.
    SCheckedSlot_6F26C3E0(CAgent* candidate);

    //  Inlined into every caller's own scope-exit code; no address.
    ~SCheckedSlot_6F26C3E0() { Release(); }

    //  0x6F085E40 - checkedslot_createunit_assign.cpp.
    SCheckedSlot_6F26C3E0* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedSlot_6F2E59B0
{
    //  0x6F0C76E0 - checkedslot_createunit_ctor.cpp.
    SCheckedSlot_6F2E59B0(CAgent* candidate);

    ~SCheckedSlot_6F2E59B0() { Release(); }

    //  0x6F0C58F0 - checkedslot_createunit_assign.cpp.
    SCheckedSlot_6F2E59B0* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//  0x6F26C3E0 - checkedslot_createunit_getters.cpp.
unsigned int GetFixedRawcode_6F26C3E0();
//  0x6F2E59B0 - checkedslot_createunit_getters.cpp.
unsigned int GetFixedRawcode_6F2E59B0();

#endif
