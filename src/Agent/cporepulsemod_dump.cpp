//============================================================================
//  0x6F48B2F0 - NIpse::CPoRePulseMod::DumpDebugState, vtable slot 16.
//  CAgentRelation::DumpDebugState(sink) via the base, then "[RePulsMod]",
//  "\tint[%s] frq[%s]" (m_interval, m_frequency) and
//  "\tdur[%10u] plf[%10u]" (m_opt as "dur", m_pulseCount as "plf").
//============================================================================
#include "cporepulsemod.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

void CPoRePulseMod::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[RePulsMod]");
    LogFormatted(sink, "\tint[%s] frq[%s]",
                 FormatCFloat(&m_interval, 0x0A, 3),
                 FormatCFloat(&m_frequency, 0x0A, 3));
    LogFormatted(sink, "\tdur[%10u] plf[%10u]", m_opt, m_pulseCount);
}

}  // namespace NIpse
