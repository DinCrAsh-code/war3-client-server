//============================================================================
//  0x6F33BD30 - CSimpleConsole::CSimpleConsole.
//
//  The name is the ??_7CSimpleConsole@@6B@ the body stamps.  A CSimpleFrame
//  with four fields of its own: two cleared words and two copies of one
//  float constant, 0.004f (0x3B83126F at 0x6F93A014, read with ida_query).
//
//  The vtable stamp lands *after* all four field stores in the shipped
//  stream even though MSVC writes it first in source order - a store to
//  [esi] and four stores to [esi+N] reorder freely, and the scheduler moves
//  it.  Nothing in the source says so; it comes out that way.
//============================================================================
#include "frame.h"

class CSimpleConsole : public CSimpleFrame
{
public:
    CSimpleConsole(void* parent);

    int   m_field124;   // +0x124
    int   m_field128;   // +0x128
    float m_float12C;   // +0x12C
    float m_float130;   // +0x130
};

CSimpleConsole::CSimpleConsole(void* parent)
    : CSimpleFrame(parent)
{
    m_float12C = 0.004f;
    m_field124 = 0;
    m_float130 = 0.004f;
    m_field128 = 0;
}
