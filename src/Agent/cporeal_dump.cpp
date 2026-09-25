//============================================================================
//  0x6F48C550 - NIpse::CPoReal::DumpDebugState, vtable slot 16. Same
//  CAgentRelation::DumpDebugState + CRlProperty's own "[Prop]" section
//  duplicated inline shape as Agent/cpoflag_dump.cpp's own version (see
//  that file's header for why), then "[Real]", "\tbas[%s] chn[%s]"
//  (+0x78, +0x7C, FormatCFloat mode 10/precision 3) and
//  "\tbnds l[%s] h[%s]" (+0x80, +0x84, same mode/precision).
//============================================================================
#include "cporeal.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

void CPoReal::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Prop]");
    LogFormatted(sink, "\tbase[%s]:[%u]",
                 FormatCFloat((const CFloat*)((char*)this + 0x70), 1, -1),
                 *(const unsigned int*)((char*)this + 0x74));

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Real]");
    LogFormatted(sink, "\tbas[%s] chn[%s]",
                 FormatCFloat(&m_value, 0x0A, 3),
                 FormatCFloat(&m_chn, 0x0A, 3));
    LogFormatted(sink, "\tbnds l[%s] h[%s]",
                 FormatCFloat(&m_rangeLo, 0x0A, 3),
                 FormatCFloat(&m_rangeHi, 0x0A, 3));
}

}  // namespace NIpse
