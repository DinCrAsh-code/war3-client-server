//============================================================================
//  0x6F495010 - NIpse::CPoSeparate::Apply, vtable slot 3 ("Apply" - column
//  name, see poseparate.h's own header comment on the family's
//  per-column naming convention). Resets the separation state: both
//  m_sepvX/m_sepvY back to g_CFloatZero's own encoded bits (loaded once,
//  stored twice - the dump's own single `mov eax, dword_6FAAE470` feeding
//  two stores), m_opt to 0. `flags` is never read in the body - the same
//  "declared but unused" stack argument this family's own slot-1-column
//  overrides already share (Agent/presence.h's own CPresence::Slot4
//  comment).
//============================================================================
#include "poseparate.h"

namespace NIpse {

void CPoSeparate::Apply(unsigned int /*flags*/)
{
    //  Written as a single raw-bits load reused for both stores (rather
    //  than two separate `m_sepvX = g_CFloatZero;` copy-assignments) to
    //  match the dump's own single `mov eax, dword_6FAAE470` feeding two
    //  stores - two independent CFloat copy-assignments each reloaded the
    //  global instead of reusing the register.
    int bits = g_CFloatZero.m_bits;
    m_sepvX.m_bits = bits;
    m_sepvY.m_bits = bits;
    m_opt = 0;
}

}  // namespace NIpse
