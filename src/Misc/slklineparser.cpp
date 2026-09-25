//============================================================================
//  A two-member fragment of the ".slk"-record-reading state object
//  Misc/citemclosure_database_thunks.cpp's own header already names (the
//  World Editor's generic named-database engine reads `.slk`-shaped game-
//  data sheets through this family).  Both members share the same two
//  fields: +0x04, the cursor into the file's text buffer, and +0x08, the
//  start of the line the cursor is currently walking - AdvanceToNextLine
//  below sets +0x08 from +0x04 at entry, and DispatchLineTag below reads
//  its own record tag through +0x08.  Only those two offsets (plus +0x00,
//  an error-code slot DispatchLineTag writes on a malformed line) are
//  established from this call tree; nothing else about the object's shape
//  is - see CLAUDE.md, "struct offsets are exact, and only touched members
//  get names".
//============================================================================

struct SSlkLineParser
{
    //  0x6F4C85E0 - advance past the current line: record its start at
    //  +0x08, then scan forward from +0x04 until a NUL, CR or LF.  A byte
    //  found this way is NUL-terminated in place (consuming it) unless the
    //  scan already sat on a NUL; either way the cursor then steps over one
    //  trailing CR and one trailing LF if either follows, so a "\r\n" pair
    //  is swallowed whole and a lone "\r" or "\n" is swallowed once.
    //  `retn 0`, always returns 1 - nothing in this call tree reaches the
    //  case where that return value is anything else, so whether it is
    //  meant to signal "more data" is not established here.
    int AdvanceToNextLine();

    //  0x6F4CA570 - read the current line's own record tag through +0x08.
    //  'E' alone is SLK's own end-of-file record (return 0, no dispatch);
    //  otherwise the second character must be ';' or +0x00 is set to the
    //  SLK-format error code -998 (0xFFFFFC1A) and 0 is returned. 'N' and
    //  every recognised-but-unhandled `<letter>;` tag both fall through to
    //  the same "skip two characters, return 1" tail with no call at all.
    //  Three tags are dispatched for real, all out of this batch's own
    //  scope (Misc/citemclosure_database_thunks.cpp's own reasoning: no
    //  committed vtable/layout for the enclosing database classes this
    //  session touches) - `sub_6F4CA370` for 'B' (bounds), `sub_6F4C9340`
    //  for 'C' (cell) and `sub_6F4C9410` for 'F' (format), each called with
    //  the line pointer advanced two characters past the tag and its ';'.
    //  `retn 0`.
    int DispatchLineTag();

    int   m_lastError;    // +0x00
    char* m_cursor;        // +0x04
    char* m_lineStart;     // +0x08
};

//  Out-of-scope callees - declared only so DispatchLineTag's own call sites
//  resolve; not reconstructed this session (see DispatchLineTag's own
//  comment above).  `retn 4`, no `this`: `mov eax,[esp+arg_0]` is every one
//  of the three dumps' own first instruction, with no `ecx` read anywhere
//  in any of them.
__declspec(naked) int __stdcall ParseSlkBoundsRecord(const char*)
{
    __asm { mov eax, 06F4CA370h }
    __asm { jmp eax }
}
__declspec(naked) int __stdcall ParseSlkCellRecord(const char*)
{
    __asm { mov eax, 06F4C9340h }
    __asm { jmp eax }
}
__declspec(naked) int __stdcall ParseSlkFormatRecord(const char*)
{
    __asm { mov eax, 06F4C9410h }
    __asm { jmp eax }
}

//  0x6F4C85E0.
int SSlkLineParser::AdvanceToNextLine()
{
    m_lineStart = m_cursor;

    if (*m_cursor != 0)
    {
        while (*m_cursor != '\r' && *m_cursor != '\n')
        {
            m_cursor++;
            if (*m_cursor == 0)
                break;
        }
    }

    //  same "already at NUL" test the scan above uses to stop - true either
    //  because the line held no CR/LF before its own terminator, or because
    //  the loop above broke out on a CR/LF.
    if (*m_cursor != 0)
    {
        *m_cursor = 0;
        m_cursor++;
    }

    if (*m_cursor == '\r')
        m_cursor++;
    if (*m_cursor == '\n')
        m_cursor++;

    return 1;
}

//  0x6F4CA570.
int SSlkLineParser::DispatchLineTag()
{
    char tag = m_lineStart[0];

    if (tag == 'E')
        return 0;

    if (tag != 'N')
    {
        if (m_lineStart[1] != ';')
        {
            m_lastError = -998;
            return 0;
        }

        switch (tag)
        {
        case 'B':
            ParseSlkBoundsRecord(m_lineStart + 2);
            return 1;
        case 'C':
            ParseSlkCellRecord(m_lineStart + 2);
            return 1;
        case 'F':
            ParseSlkFormatRecord(m_lineStart + 2);
            return 1;
        }
    }

    return 1;
}
