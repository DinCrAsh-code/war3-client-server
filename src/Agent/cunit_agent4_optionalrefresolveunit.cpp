//============================================================================
//  0x6F2862F0 - resolve an embedded SOptionalHandleRef (`this`'s own
//  +0x58/+0x5C, Item/itemhandleresolve.h) to a CUnit*, through the same
//  type-checked scoped slot every member of the agenttypedslots.h family
//  uses: unset (`m_handle & m_typeTag == -1`, i.e. both fields all-ones)
//  resolves to null rather than calling QueryHandleField0x54 at all,
//  otherwise its answer is handed straight to SCheckedUnitSlot's own
//  converting constructor and the slot's own value returned - the slot's
//  destructor running inline at the one return, the same shape
//  Unit/unitordermake.cpp's MakeOrderAgent already establishes for this
//  family.
//============================================================================
#include "agenttypedslots.h"
#include "itemhandleresolve.h"

//  `this` (ecx) is some outer object whose own +0x58/+0x5C is the embedded
//  SOptionalHandleRef; the caller in this tree is not in this closure, so
//  the outer type is left opaque.
CAgent* __fastcall ResolveOptionalUnitRef(void* thisObj)
{
    SOptionalHandleRef* ref = (SOptionalHandleRef*)((char*)thisObj + 0x58);

    void* candidate = 0;
    if ((ref->m_handle & ref->m_typeTag) != 0xFFFFFFFF)
        candidate = QueryHandleField0x54(ref);

    SCheckedUnitSlot held((CAgent*)candidate);
    return held.m_value;
}
