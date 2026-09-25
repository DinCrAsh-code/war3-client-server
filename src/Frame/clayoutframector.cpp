//============================================================================
//  0x6F6065C0 - CLayoutFrame::CLayoutFrame.
//
//  The vtable pointer and the point count are in the member-initialiser list
//  and everything else is in the body, because that is the only split that
//  puts the two of them *ahead* of the two member constructors: MSVC runs a
//  member-initialiser list in declaration order and the two intrusive links
//  are declared after both (docs/msvc-vc8-idioms.md, "A member-initialiser
//  list runs in declaration order, so split the constructor").
//============================================================================
#include "frame.h"
#include "framethunks.h"

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

//  The vtable pointer is not stored by hand: CLayoutFrame declares one
//  `virtual` (frame.h), so MSVC stamps ??_7CLayoutFrame@@6B@ ahead of the
//  member-initialiser list, which is where the shipped code has it.
CLayoutFrame::CLayoutFrame()
    : m_pointCount(9)
{
    m_rect[0] = 0.0f;
    m_rect[1] = 0.0f;
    m_rect[2] = 0.0f;
    m_rect[3] = 0.0f;
    m_54 = 0;
    m_width   = 0.0f;
    //  Between the width and the height, which is where the shipped code
    //  puts it - not a slip.
    m_64 = 0;
    m_height  = 0.0f;
    m_scale   = 1.0f;

    //  m_pointCount is re-read out of memory rather than folded to 36: the
    //  compiler cannot prove the stores above did not land on it.
    memset(m_points, 0, m_pointCount * sizeof(void*));
    m_2C = 0;
}
