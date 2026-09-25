//============================================================================
//  0x6F48D080 - NIpse::CPoFlag::DumpDebugState, vtable slot 16.
//
//  Calling `((CRlProperty*)this)->DumpDebugState(sink)` and hoping /Ob2
//  would inline it was tried first and scored 0.610: the compiler kept it
//  as a real call (`call NIpse::CRlProperty::DumpDebugState`) rather than
//  inlining, unlike the real dump, which shows `CAgentRelation::
//  DumpDebugState` called directly and CRlProperty's own "[Prop]"/
//  "base[%s]:[%u]" section reproduced inline with no call to
//  CRlProperty::DumpDebugState's own address at all. Written out here
//  explicitly instead - the exact same steps Agent/rlproperty_dump.cpp's
//  own CRlProperty::DumpDebugState body performs, duplicated rather than
//  called (this codebase's own /OPT:ICF-less-duplication idiom: the same
//  body existing at more than one address is ordinary here) - then this
//  object's own "[Flag]" tag and m_value as "\tval[0x%X]".
//============================================================================
#include "cpoflag.h"
#include "agentrelation.h"
#include "textsink.h"
#include "CFloat.h"

namespace NIpse {

//  IDA's own (unresolved) name for the shared empty-string global,
//  0x6F87529C - see Agent/rlproperty_dump.cpp's own file header (same
//  precedent - every includer keeps its own file-local copy).
static const char ValueName[] = "";

void CPoFlag::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    //  CRlProperty::DumpDebugState's own "[Prop]" section, duplicated here
    //  rather than called - see this file's own header.
    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Prop]");
    LogFormatted(sink, "\tbase[%s]:[%u]",
                 FormatCFloat((const CFloat*)((char*)this + 0x70), 1, -1),
                 *(const unsigned int*)((char*)this + 0x74));

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Flag]");
    LogFormatted(sink, "\tval[0x%X]", m_value);
}

}  // namespace NIpse
