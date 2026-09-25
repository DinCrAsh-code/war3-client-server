//============================================================================
//  0x6F36D970 - a one-instruction `jmp sub_6F36D130` alias IDA gives its
//  own heading, the same convention as Agent/handleobject.cpp's
//  GetHandleOrZeroAlias: naked, because there is no C++ that produces a
//  single relative jump and nothing else.
//============================================================================
#include "storm.h"

//  Declared with the same parameter type GameUI/selectionhostreleasefields.cpp
//  itself uses - a mismatched pointee type here would mangle to a different
//  symbol and the two would never link (tools/link_check.py's first defect
//  class), the same trap GameUI/selectionhostdtor.cpp's own comment
//  documents.
struct SSelectionHostSixRefs;
void __fastcall ReleaseSelectionHostSixRefsAndBaseDtor(SSelectionHostSixRefs* self);

__declspec(naked) void __fastcall
    ReleaseSelectionHostSixRefsAndBaseDtorAlias(void*)
{
    __asm { jmp ReleaseSelectionHostSixRefsAndBaseDtor }
}
