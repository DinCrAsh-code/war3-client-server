//============================================================================
//  0x6F4CB8D0 - AUMsgBuffer::SetText.  See sysmessage.h.
//============================================================================
#include "sysmessage.h"
#include "storm.h"

//  Storm.dll ordinal 507 - "duplicate this string into Storm's heap", the
//  same import tokenizercreate.cpp already declares.  Re-declared here
//  rather than included from there because that file's own declaration is
//  local to it; the mangling is identical, so the linker binds one symbol.
char* __stdcall Storm_507(const char* text, const char* logfilename,
                          int logline);

void AUMsgBuffer::SetText(const char* text, int type)
{
    if (m_text != 0)
        SMemFree(m_text, ".\\SysMessage.cpp", 0x42, 0);

    //  A conditional expression, not an if/else with a store in each arm.
    //  Both spellings duplicate the tail, but only this one puts the null
    //  case's zero through a register first (`xor eax, eax` then
    //  `mov [esi+8], eax`, which is what the shipped code does) - assigning
    //  the literal inside a branch stores the immediate straight to memory.
    m_text = text != 0 ? Storm_507(text, ".\\SysMessage.cpp", 0x43) : 0;
    m_type = type;
}

//  The RTTI descriptor name the shipped code hands Storm for a node.
//  Reproduced as a literal because this build has RTTI off; the instruction
//  score cannot see which string it is either way (canonlib maps every
//  unresolved symbol to SYM), but the allocation tag is what identified the
//  class in the first place, so it is spelled out rather than invented.
static const char s_nodeTypeName[] = ".?AVAUMsgBuffer@@";

//----------------------------------------------------------------------------
//  0x6F4CBE70 - AUMsgList::FreeAll.  See sysmessage.h.
//
//  The tail link is re-read from the field at the top of every iteration
//  rather than stepped through a cursor, because each iteration frees the
//  node it just visited - there is nothing left to step from.  That is also
//  why the loop condition is a signed `> 0` on the raw link and not a null
//  test: a drained list leaves the terminator's own complemented address
//  there, which is negative (storm.h).
//
//  The shipped loop tests at the top and jumps back unconditionally; this
//  build duplicates the test at the bottom instead, which is one extra
//  instruction.  A `for (;;)` with an explicit `break` was measured and
//  MSVC rotates that the same way, so the plain `while` is what is here.
//----------------------------------------------------------------------------
void AUMsgList::FreeAll()
{
    while (m_terminator.m_prevlink > 0)
    {
        AUMsgBuffer* node = (AUMsgBuffer*)m_terminator.m_prevlink;

        if (node->m_text != 0)
            SMemFree(node->m_text, ".\\SysMessage.cpp", 0x35, 0);

        node->m_link.Unlink();
        SMemFree(node, s_nodeTypeName, -2, 0);
    }
}
