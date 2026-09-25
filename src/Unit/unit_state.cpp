//============================================================================
//  CUnit::GetState / CUnit::SetState - the two `unitstate` dispatchers the
//  JASS natives GetUnitState and SetUnitState call.
//
//  These are what name the life pool.  JASS's `unitstate` constants are
//  0 = UNIT_STATE_LIFE, 1 = UNIT_STATE_MAX_LIFE, 2 = UNIT_STATE_MANA,
//  3 = UNIT_STATE_MAX_MANA, and the two bodies below map them onto:
//
//      state 0  ->  vtable +0x124 (SetLife)     / +0x12C (GetLife)
//      state 1  ->                              / +0x130 (GetMaxLife)
//      state 2  ->  m_mana.SetValue()           / m_mana.GetValue()
//      state 3  ->                              / m_mana.GetHigh()
//
//  So slot 73 is life, slot 75 is life, slot 76 is *max* life - and the
//  SUnitTrackedRef at CUnit+0x98 is the life pool while the one at +0xB8 is
//  mana.  `GetHigh` being what MAX_MANA reads is also what fixes "high" as
//  the maximum end of the range rather than the other way round.
//
//  Own translation unit: both are called from the natives in
//  src/Jass/jassnatives_life.cpp for real, and SetState calls slot 73
//  virtually, so nothing here may be folded into either.
//============================================================================
#include "unit.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F27AE90 - GetState.  `retn 8`: the out-buffer and the state index.
//
//  A real `switch`, not an if-chain: the shipped body is a bounds check
//  (`cmp eax,3` / `ja default`) and a `jmp ds:jpt_6F27AE9A[eax*4]` through a
//  four-entry jump table.  Every arm returns the caller's buffer, and the
//  default writes encoded zero into it.
//
//  Cases 0 and 1 forward to a *virtual* slot on this same object rather
//  than touching m_life directly - CItem and CDestructable override those
//  slots and keep their life somewhere else entirely, so the indirection is
//  load-bearing, not redundant.  Cases 2 and 3 go straight to the mana ref
//  because nothing overrides mana.
//----------------------------------------------------------------------------
CFloat* CUnit::GetState(CFloat* out, int state)
{
    switch (state)
    {
    case 0:
        GetLife(out);
        return out;
    case 1:
        GetMaxLife(out);
        return out;
    case 2:
        //  Staged through a local and copied out, the way slot 75 stages
        //  its own: the shipped arm calls GetValue on a frame slot and then
        //  does `mov [eax], ecx` into the caller's buffer.
        {
            CFloat value(kCFloatNoInit);
            m_mana.GetValue(&value);
            *out = value;
        }
        return out;
    case 3:
        m_mana.GetHigh(out);
        return out;
    }

    *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F27AF20 - SetState.  `retn 8`: the state index and the value.
//
//  Only two of the four states are writable, and the shipped code says so
//  by falling straight through to `retn 8` for the others - MAX_LIFE and
//  MAX_MANA are not settable through this path.  Two live cases out of a
//  sparse set is what MSVC lowers to the `sub eax,0` / `jz` / `sub eax,2` /
//  `jnz` subtract chain rather than to a jump table.
//----------------------------------------------------------------------------
void CUnit::SetState(int state, const CFloat* value)
{
    switch (state)
    {
    case 0:
        SetLife(value);
        break;
    case 2:
        m_mana.SetValue(value);
        break;
    }
}
