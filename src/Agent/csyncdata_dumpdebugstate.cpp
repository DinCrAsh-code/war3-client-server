//============================================================================
//  NTempest::CSyncData::DumpDebugState, 0x6F536C30, vtable slot 4 (+0x10).
//  See csyncdata.h for the class-level header comment.
//
//  Three LogFormatted lines: an empty header line, "[CSyncData] (%s)" with
//  m_name, then "\tcsum[%08X], count[%u]" with a checksum value read
//  through THIS object's own vtable slot 1 (GetChecksumField) - a genuine
//  indirect dispatch, not a direct call (the dump's own `mov edx,[esi] ;
//  mov eax,[edx+4] ; call eax`), written the same
//  `((Fn)(*(void***)this)[N])(this)` idiom every other self-vtable-slot
//  dispatch in this codebase uses (Agent/agent_notify.cpp,
//  Agent/agentdaynight.cpp, ...) rather than a real C++ `virtual` call -
//  this whole class stays a flat, non-polymorphic-in-the-C++-sense
//  placeholder the same way every sibling in this family does (see
//  Pathfinding/crlagent.h's own file header for why).
//
//  `count` (m_count, +0x08) is pushed onto the stack BEFORE the indirect
//  call and only consumed afterward, as the trailing LogFormatted vararg -
//  legal because GetChecksumField's own real body takes zero stack
//  arguments (plain `retn`, confirmed from its own raw bytes) and never
//  touches esp, so the pre-staged push survives across the call
//  undisturbed. Written as a single LogFormatted call with the virtual
//  dispatch inlined as an argument expression (not a named local read
//  afterward) so the compiler evaluates `m_count` - the simpler of the two
//  varargs - first and pushes it immediately, right-to-left, before ever
//  touching the virtual call; a named `unsigned int count = m_count;`
//  local instead forces it into a callee-saved register with its own
//  push/pop pair across the call, which the dump does not have.
//============================================================================
#include "csyncdata.h"
#include "textsink.h"

namespace NTempest {

typedef unsigned int (__thiscall *GetChecksumFieldFn)(void*);

//  0x6F536C30 - vtable slot 4 (+0x10).
void CSyncData::DumpDebugState(void* sink, unsigned int /*unused4*/)
{
    LogFormatted(sink, "");
    LogFormatted(sink, "[CSyncData] (%s)", m_name);

    //  `m_count` is a plain field read with nothing to wait on, so the
    //  compiler pushes it (the LAST LogFormatted vararg, right-to-left)
    //  before it even sets up the virtual call below - no local variable
    //  needed to keep it alive across that call the way an intermediate
    //  named local would (that was this file's own first, wrong, attempt:
    //  it forced `count` into a callee-saved register with its own
    //  push/pop pair the dump does not have). Left inline for exactly that
    //  reason.
    LogFormatted(sink, "\tcsum[%08X], count[%u]",
                 ((GetChecksumFieldFn)(*(void***)this)[1])(this), m_count);
}

}  // namespace NTempest
