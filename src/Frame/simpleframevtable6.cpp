//============================================================================
//  0x6F604FB0 / 0x6F604FA0 - slots 6/7 (+0x18/+0x1C): m_scale * m_width and
//  m_scale * m_height.  No arguments, no callees.
//============================================================================
#include "frame.h"
#include "framethunks.h"

float CSimpleFrame::GetScaledWidth() const
{
    return m_scale * m_width;
}

float CSimpleFrame::GetScaledHeight() const
{
    return m_scale * m_height;
}
