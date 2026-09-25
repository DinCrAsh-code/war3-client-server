//============================================================================
//  0x6F48C880 - NIpse::CPoPoInterfLis::DumpDebugState, vtable slot 16.
//  CAgentRelation::DumpDebugState(sink) via the base, then the shared empty
//  ValueName line, this class's own literal tag "[PoLis]" - NOT
//  "[PoInterfLis]", confirmed byte-for-byte off aPolis - and
//  "\trad[%s] opt[0x%X]" of m_radius/m_opt.
//============================================================================
#include "cpopointerflis.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

void CPoPoInterfLis::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[PoLis]");
    LogFormatted(sink, "\trad[%s] opt[0x%X]",
                 FormatCFloat(&m_radius, 1, -1),
                 m_opt);
}

}  // namespace NIpse
