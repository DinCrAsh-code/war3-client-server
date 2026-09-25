//============================================================================
//  Shared struct declarations for the four address-derived-name members of
//  the "type-checked smart pointer to a CAgent-derived object" family
//  (agenttypedslots.h) that batch-25 and batch-26 added -
//  cunit_vtable_closure_worklist.json, CUnit batch-25/batch-26.
//
//  Pulled out to its own header (rather than staying local to
//  checkedslot_batch25.cpp, as batch-25 first wrote them) so that a second
//  translation unit can add more members - a converting constructor for two
//  of the four (checkedslot_batch26.cpp), the same
//  checkedunitslot.cpp/unittypedassign.cpp split the rest of the family
//  already keeps - without redeclaring the whole struct and risking the two
//  declarations drifting apart.
//
//      0x6F2700B0  "used"  SCheckedSlot_6F2700B0  Assign @0x6F2728A0, Make @0x6F293010
//      0x6F270130  "uddd"  SCheckedSlot_6F270130  Assign @0x6F272940, Make @0x6F293090
//      0x6F40BD90  "pusd"  SCheckedSlot_6F40BD90  Assign @0x6F40E430, ctor @0x6F412E30
//      0x6F40BE10  "psel"  SCheckedSlot_6F40BE10  Assign @0x6F40E4B0, ctor @0x6F412E90
//============================================================================
#ifndef CHECKEDSLOT_BATCH2526_H
#define CHECKEDSLOT_BATCH2526_H

#include "agent.h"

struct SCheckedSlot_6F2700B0
{
    //  0x6F2728A0 - checkedslot_batch25.cpp.
    SCheckedSlot_6F2700B0* Assign(CAgent* candidate);

    //  0x6F293010 - checkedslot_batch26.cpp.  Build a fresh '+used' agent
    //  from the type registry and park it here - the same "MakeXXX" shape
    //  SCheckedRectSlot::MakeRect has (jassrectmake.cpp).
    SCheckedSlot_6F2700B0* Make(int, int, int kind);

    CAgent* m_value;
};

struct SCheckedSlot_6F270130
{
    //  0x6F272940 - checkedslot_batch25.cpp.
    SCheckedSlot_6F270130* Assign(CAgent* candidate);

    //  0x6F293090 - checkedslot_batch26.cpp.
    SCheckedSlot_6F270130* Make(int, int, int kind);

    CAgent* m_value;
};

struct SCheckedSlot_6F40BD90
{
    //  0x6F412E30 - checkedslot_batch26.cpp.  The same three-statement
    //  converting constructor every scoped member of the family has
    //  (agenttypedslots.h) - zero the value, Assign inside a `__try`,
    //  Release in a `__finally` guarded by AbnormalTermination().
    SCheckedSlot_6F40BD90(CAgent* candidate);

    ~SCheckedSlot_6F40BD90() { Release(); }

    //  0x6F40E430 - checkedslot_batch25.cpp.
    SCheckedSlot_6F40BD90* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedSlot_6F40BE10
{
    //  0x6F412E90 - checkedslot_batch26.cpp.
    SCheckedSlot_6F40BE10(CAgent* candidate);

    ~SCheckedSlot_6F40BE10() { Release(); }

    //  0x6F40E4B0 - checkedslot_batch25.cpp.
    SCheckedSlot_6F40BE10* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

#endif
