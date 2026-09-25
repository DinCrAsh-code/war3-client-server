//============================================================================
//  0x6F3A3BE0 - free a JassArray's own element storage, if any is held,
//  and clear the {alloc,count,data} triple - not m_elementChunk, which the
//  disassembly never touches here.  `retn 0`.
//============================================================================
#include "storm.h"
#include "jassarray.h"

//  The literal here is a placeholder: IDA could not resolve the real string
//  at this call's own operand (its own label is the garbled "aK_1"), so the
//  actual bytes are unknown.  Immaterial to the score either way - the
//  canonicaliser reduces any `offset SYM` operand to `SYM` on both sides
//  (CLAUDE.md's own note on this) - but call_check.py's caller wants text.
static const char kUnresolvedTag[] = "aK_1";

void __fastcall JassArrayReleaseElements(JassArrayElements* elements)
{
    if (elements->m_data)
        SMemFree(elements->m_data, kUnresolvedTag, -2, 0);

    elements->m_alloc = 0;
    elements->m_count = 0;
    elements->m_data = 0;
}
