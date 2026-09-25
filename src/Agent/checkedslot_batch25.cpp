//============================================================================
//  Four more instantiations of the "type-checked smart pointer to a
//  CAgent-derived object" family agenttypedslots.h documents - reached
//  from CUnit slots 116/117's own closure
//  (cunit_vtable_closure_worklist.json, CUnit batch-25).  Byte-for-byte the
//  same body as every other member of the family (SCheckedOrderSlot::Assign
//  etc., ordertypedassign.cpp) with one operand changed: which fixed FourCC
//  the candidate's own virtual GetAgileTypeId() is checked against.
//
//  None of the four required constants decodes to a member of the known
//  agile-type-id family (misc_rawcode_getters.cpp's own '+w3x'/'item'
//  list), so - matching that file's own rule for a constant with no
//  established meaning - both the getter and the slot type keep an
//  address-derived name rather than a guessed one, even though each reads
//  as an ordinary word MSB-first:
//
//      0x6F2700B0  "used"  ->  sub_6F2728A0  SCheckedSlot_6F2700B0::Assign
//      0x6F270130  "uddd"  ->  sub_6F272940  SCheckedSlot_6F270130::Assign
//      0x6F40BD90  "pusd"  ->  sub_6F40E430  SCheckedSlot_6F40BD90::Assign
//      0x6F40BE10  "psel"  ->  sub_6F40E4B0  SCheckedSlot_6F40BE10::Assign
//============================================================================
//  The struct declarations moved to checkedslot_batch2526.h with batch-26,
//  which adds a converting constructor for two of the four and a "MakeXXX"
//  factory for the other two - see that header for why a shared declaration
//  is now needed instead of the four structs staying local to this file.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "checkedslot_batch2526.h"

//  Defined in checkedslot_batch25_getters.cpp - kept in a separate TU so
//  the optimiser cannot inline the constant into the call site below (see
//  that file's own header).
unsigned int GetFixedRawcode_6F2700B0();
unsigned int GetFixedRawcode_6F270130();
unsigned int GetFixedRawcode_6F40BD90();
unsigned int GetFixedRawcode_6F40BE10();

#define CHECKED_SLOT(name, getter)                                          \
    name* name::Assign(CAgent* candidate)                                   \
    {                                                                       \
        CAgent* checked;                                                    \
        if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),\
                                                 getter()))                  \
            checked = candidate;                                            \
        else                                                                \
            checked = 0;                                                    \
                                                                              \
        CAgent* old = m_value;                                              \
        if (old != checked)                                                 \
        {                                                                   \
            if (old)                                                       \
            {                                                              \
                if (--old->m_refcount == 0)                                \
                    old->ReleaseSelf();                                     \
            }                                                              \
            if (checked)                                                   \
            {                                                              \
                if (candidate)                                             \
                    candidate->m_refcount++;                               \
            }                                                              \
            m_value = checked;                                             \
        }                                                                  \
                                                                              \
        return this;                                                        \
    }

//  0x6F2728A0
CHECKED_SLOT(SCheckedSlot_6F2700B0, GetFixedRawcode_6F2700B0)
//  0x6F272940
CHECKED_SLOT(SCheckedSlot_6F270130, GetFixedRawcode_6F270130)
//  0x6F40E430
CHECKED_SLOT(SCheckedSlot_6F40BD90, GetFixedRawcode_6F40BD90)
//  0x6F40E4B0
CHECKED_SLOT(SCheckedSlot_6F40BE10, GetFixedRawcode_6F40BE10)

#undef CHECKED_SLOT
