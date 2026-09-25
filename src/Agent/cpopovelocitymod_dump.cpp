//============================================================================
//  0x6F48B820 - NIpse::CPoPoVelocityMod::DumpDebugState, vtable slot 16.
//  CAgentRelation::DumpDebugState(sink) via the base, then the shared empty
//  ValueName line, this class's own literal tag "[PoVelMod]", and
//  "\tvel x[%s] y[%s] dur[%s]" of m_velocityX/m_velocityY/m_duration - the
//  shipped code formats duration first, then Y, then X (right-to-left
//  argument evaluation for the final variadic call), which plain
//  left-to-right C++ argument order reproduces under this compiler's own
//  right-to-left cdecl evaluation, same as
//  Agent/cporecontinuousmod_dump.cpp's own two-field DumpDebugState.
//============================================================================
#include "cpopovelocitymod.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

void CPoPoVelocityMod::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[PoVelMod]");
    LogFormatted(sink, "\tvel x[%s] y[%s] dur[%s]",
                 FormatCFloat(&m_velocityX, 1, -1),
                 FormatCFloat(&m_velocityY, 1, -1),
                 FormatCFloat(&m_duration, 1, -1));
}

}  // namespace NIpse
