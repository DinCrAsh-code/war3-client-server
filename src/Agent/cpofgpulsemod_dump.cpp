//============================================================================
//  0x6F48BB90 - NIpse::CPoFgPulseMod::DumpDebugState, vtable slot 16.
//  CAgentRelation::DumpDebugState(sink) via the base, then "[FgPulseMod]",
//  "\tmsk[0x%X] dlt[0x%X]" (m_mask, m_delta) and
//  "\tfrq[%s] dur[%u] plf[%u]" (m_frequency via FormatCFloat, m_duration,
//  m_playfield).
//============================================================================
#include "cpofgpulsemod.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

void CPoFgPulseMod::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[FgPulseMod]");
    LogFormatted(sink, "\tmsk[0x%X] dlt[0x%X]", m_mask, m_delta);
    LogFormatted(sink, "\tfrq[%s] dur[%u] plf[%u]",
                 FormatCFloat(&m_frequency, 1, -1), m_duration, m_playfield);
}

}  // namespace NIpse
