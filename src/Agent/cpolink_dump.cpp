//============================================================================
//  0x6F48D2D0 - NIpse::CPoLink::DumpDebugState, vtable slot 16. Same
//  CRlProperty::DumpDebugState-duplicated-inline shape as
//  Agent/cpoflag_dump.cpp's own version (see that file's header for why a
//  real call to `((CRlProperty*)this)->DumpDebugState(sink)` does not
//  match - the compiler keeps it a real call rather than inlining it), then
//  "[Link]" and "\ttgt t[%u] b[%u]" with t=m_targetHandle (+0x78),
//  b=m_targetSub (+0x7C).
//============================================================================
#include "cpolink.h"
#include "agentrelation.h"
#include "textsink.h"
#include "CFloat.h"

namespace NIpse {

static const char ValueName[] = "";

void CPoLink::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Prop]");
    LogFormatted(sink, "\tbase[%s]:[%u]",
                 FormatCFloat((const CFloat*)((char*)this + 0x70), 1, -1),
                 *(const unsigned int*)((char*)this + 0x74));

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Link]");
    LogFormatted(sink, "\ttgt t[%u] b[%u]", m_targetHandle, m_targetSub);
}

}  // namespace NIpse
