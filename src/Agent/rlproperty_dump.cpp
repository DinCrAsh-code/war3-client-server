//============================================================================
//  0x6F4874F0 - NIpse::CRlProperty::DumpDebugState, vtable slot 16 (+0x40).
//
//  Dumps the base `CAgentRelation::DumpDebugState` first, an empty line
//  (LogFormatted with the shared empty-string global `ValueName`,
//  0x6F87529C - see Window/messageboxprompt.cpp's own header comment for
//  why every includer keeps its own file-local copy rather than a shared
//  DATA declaration), a "[Prop]" tag, then this object's own CFloat/dword
//  pair at +0x70/+0x74 as "\tbase[%s]:[%u]".
//============================================================================
#include "rlproperty.h"
#include "agentrelation.h"
#include "textsink.h"
#include "CFloat.h"

namespace NIpse {

//  IDA's own (unresolved) name for the shared empty-string global,
//  0x6F87529C - see Window/messageboxprompt.cpp's own header comment.
static const char ValueName[] = "";

void CRlProperty::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Prop]");

    //  Written as one call, arguments inline rather than through locals: a
    //  temp for `value` living across the FormatCFloat call spills into an
    //  extra saved register (ebx) the dump does not have - MSVC's own
    //  right-to-left cdecl argument evaluation pushes `value` (the
    //  rightmost argument) onto the stack *before* evaluating FormatCFloat,
    //  which is exactly the shipped order and needs no extra register.
    LogFormatted(sink, "\tbase[%s]:[%u]",
                 FormatCFloat((const CFloat*)((char*)this + 0x70), 1, -1),
                 *(const unsigned int*)((char*)this + 0x74));
}

}  // namespace NIpse
