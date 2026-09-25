//============================================================================
//  0x6F48B570 - NIpse::CPoReThresholdLis::DumpDebugState, vtable slot 16.
//  CAgentRelation::DumpDebugState(sink) via the base, then "[ReThrLis]"
//  and "\tthr[%s] mod[%10u]" (m_threshold via FormatCFloat, m_mode).
//============================================================================
#include "cporethresholdlis.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

void CPoReThresholdLis::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[ReThrLis]");
    LogFormatted(sink, "\tthr[%s] mod[%10u]",
                 FormatCFloat(&m_threshold, 0x0A, 3), m_mode);
}

}  // namespace NIpse
