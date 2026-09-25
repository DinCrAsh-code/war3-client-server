//============================================================================
//  0x6F48AFE0 - NIpse::CPoReContinuousMod::DumpDebugState, vtable slot 16.
//  CAgentRelation::DumpDebugState(sink) via the base, then "[ReContMod]"
//  and "\trte[%s] dur[%s]" (m_rate, m_duration).
//============================================================================
#include "cporecontinuousmod.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

void CPoReContinuousMod::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[ReContMod]");
    LogFormatted(sink, "\trte[%s] dur[%s]",
                 FormatCFloat(&m_rate, 0x0A, 3),
                 FormatCFloat(&m_duration, 0x0A, 3));
}

}  // namespace NIpse
