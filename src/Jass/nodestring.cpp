//============================================================================
//  0x6F463DB0 - NodeString::NodeString.  See jassnodes.h.
//
//  Unescapes `text` into a stack buffer (the shipped frame reserves 0x400
//  bytes for it - `sub esp, 408h` - which is also why this constructor is
//  SEH-framed: a 0x400-byte local plus the placement-constructed RCString
//  earn the same `__except_handler4` frame docs/msvc-vc8-idioms.md already
//  says this build's fixed /GS- /EHs-c- cannot reproduce, so this is
//  DIFFERS by construction regardless of how faithfully the escape switch
//  itself is reproduced.  A recognised escape (`\\`, `\'`, `\"`, `\r`, `\n`,
//  `\b`, `\f`, `\t`) becomes its one-byte meaning and advances the reader by
//  two; anything else copies the two source bytes through unchanged - the
//  shipped jump table's default case, read off `jpt_6F463E76`.
//============================================================================
#include "jassnodes.h"

NodeString::NodeString(JassCompileContext* ctx, const char* text)
    : Node(ctx, 0x22)
{
    char buf[0x400];
    char* out = buf;
    const char* p = text;

    for (char c = *p; c != 0; c = *p)
    {
        if (c != '\\')
        {
            *out++ = c;
            p += 1;
            continue;
        }

        char esc = p[1];
        switch (esc)
        {
        case '\\': *out++ = '\\'; break;
        case '\'': *out++ = '\''; break;
        case '"':  *out++ = '"';  break;
        case '\r': *out++ = '\r'; break;
        case '\n': *out++ = '\n'; break;
        case '\b': *out++ = '\b'; break;
        case '\f': *out++ = '\f'; break;
        case '\t': *out++ = '\t'; break;
        default:
            *out++ = '\\';
            *out++ = esc;
            break;
        }
        p += 2;
    }
    *out = 0;

    m_text.Construct(buf);

    m_typeKind = 6;
    m_typeExtra = 0;
}
