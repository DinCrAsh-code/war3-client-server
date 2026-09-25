//============================================================================
//  0x6F611D40 - CTextFrame::SetString.
//
//  Three ways in and one way out.  A null string just drops everything; a
//  string that fits in the buffer already there is copied over it in place
//  (FreeText(0), which keeps that buffer and drops only the masked copy); a
//  longer one - or the first one - gets a fresh Storm duplicate and a new
//  capacity.  The `ja` on the length test is unsigned, which is what says
//  m_textCapacity is.
//
//  Then the three dirty flags, then the display pointer: masked captions get
//  their own run of '*' and everything else aliases m_pText, which is why
//  FreeText has two arms.  The relayout goes through the CLayoutFrame base
//  at +0xB4 and is the one call site in this library that passes 1 rather
//  than 0 for `checkQueued`.
//
//  The `push 1` / `call FreeText` on the null path has no `mov ecx, esi` in
//  front of it because nothing has touched ecx since the prologue's
//  `mov esi, ecx` - MSVC drops a receiver load it can see is redundant
//  (docs/msvc-vc8-idioms.md, "A missing `mov ecx, <recv>` is not evidence of
//  a free function").
//============================================================================
#include "frame.h"
#include "framethunks.h"

static const char kTextFrameFile[] = ".\\CTextFrame.cpp";

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

//----------------------------------------------------------------------------
//  0x6F611090 - a fresh run of '*' as long as `text`, which is what a masked
//  caption draws instead of its own string.  Allocate strlen+1, fill the
//  first strlen bytes and terminate; the `xor eax,eax` / `test esi,esi` /
//  `jbe` guard round the fill is there because a zero-length string still
//  needs its terminator, at offset 0.
//
//  **`static`, and in SetString's own translation unit, is what puts its
//  argument in `eax`.**  The shipped function reads eax without ever setting
//  it and its one caller leaves the string there - the whole-program
//  convention docs/msvc-vc8-idioms.md records under "Arguments in eax and
//  esi".  Ordinarily that is out of reach, but a `static` function whose
//  every call site MSVC can see gets to invent a convention, and this
//  compiler invents exactly the shipped one (`; _text$ = eax` in the
//  listing, `ret 0`).  At 27 instructions /Ob2 declines to inline it, so the
//  shipped `call` survives; measured against the alternative, a non-static
//  `__fastcall` in a file of its own costs this function two register names
//  and CTextFrame::SetString its last two instructions.
//
//  The price is that it is not externally visible and therefore not
//  hookable - which is the right trade for a leaf whose only caller is
//  hooked with it.
//----------------------------------------------------------------------------
static char* MakeMaskedText(const char* text)
{
    if (text == 0)
        return 0;

    unsigned int length = Storm_506(text);
    char* masked = (char*)SMemAlloc(length + 1, kTextFrameFile, 26, 0);

    unsigned int end = 0;
    if (length > 0)
    {
        memset(masked, '*', length);
        end = length;
    }

    masked[end] = 0;
    return masked;
}

void CTextFrame::SetString(const char* text)
{
    if (text != 0)
    {
        unsigned int length = Storm_506(text);

        if (m_pText != 0 && length <= m_textCapacity)
        {
            FreeText(0);
            Storm_501(m_pText, text, m_textCapacity + 1);
        }
        else
        {
            FreeText(1);
            m_pText = Storm_507(text, kTextFrameFile, 545);
            m_textCapacity = length;
        }
    }
    else
    {
        FreeText(1);
    }

    m_22C = 1;
    m_230 = 1;
    m_224 = 1;

    if (HasStyleBits(0x100) && m_pText != 0)
        m_pDisplayText = MakeMaskedText(m_pText);
    else
        m_pDisplayText = m_pText;

    ((CLayoutFrame*)this)->QueueForLayout(1);

    m_23C = 1;
}
