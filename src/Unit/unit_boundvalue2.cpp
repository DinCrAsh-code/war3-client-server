//============================================================================
//  0x6F29E850 - CUnit's vtable slot 58 (+0xE8).  See unit.h.
//
//  Three answers for one question, picked by the same pair of tests slot
//  109 (unit_boundvalue.cpp) uses: bit 0x800 of CAgentWar3::m_flags, and
//  the state word at +0x1FC.
//
//    flagged, or state 2  -> the +0x214 tracked value itself
//    state 8              -> the unit type's own value from the type table
//    anything else        -> zero
//
//  The two non-zero arms differ in one telling way.  The type-table arm
//  dereferences the *returned* pointer (`fld dword ptr [eax]`); the
//  tracked-value arm reads the local it passed in (`fld [esp+4+var_4]`)
//  and ignores what came back.  Both are written that way here rather than
//  normalised, because writing either one the other way costs the load its
//  addressing mode.
//============================================================================
#include "unit.h"
#include "CFloat.h"

//  0x6F29C710 (`retn 0`) - out buffer in ecx, FourCC in edx, and it hands
//  the buffer back.  Same family as unit_placement.cpp's four.
CFloat* __fastcall UnitTypeStateValue_6F29C710(CFloat* out, unsigned int fourCC);

float CUnit::Method_0xE8()
{
    CFloat value(kCFloatNoInit);

    //  The state word is read into a local before the flag test rather
    //  than at each use: the shipped code loads it up front, ahead of the
    //  branch that may not need it at all.
    unsigned int state = m_int1FC;

    if ((m_flags & 0x800) == 0 && state != 2)
    {
        //  `!= 8` and not `== 8`: the type-table arm is the fall-through
        //  in the shipped code and the zero is at a label, which is the
        //  way round this puts them.
        if (state != 8)
            return 0.0f;
        return *(const float*)UnitTypeStateValue_6F29C710(&value, m_footprintType);
    }

    m_ref214.GetValue(&value);
    return *(const float*)&value;
}
