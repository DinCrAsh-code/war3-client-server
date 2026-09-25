//============================================================================
//  0x6F621B70 - CModelFrame::CModelFrame.
//
//  A CFrame, both vtables stamped, and three cleared words.  Not part of the
//  JASS_PanCameraToTimedWithZ dump itself - the tree stops one level short of
//  it - but it is CBackdropFrame's base, and reconstructing it is what lets
//  that class, and the chat bar above it, be written as constructors at all.
//
//  All three arguments go straight through to CFrame's own constructor.
//============================================================================
#include "modelframe.h"

CModelFrame::CModelFrame(int a, int b, int c)
    : CFrame(a, b, c)
{
    m_field168 = 0;
    m_field16C = 0;
    m_field170 = 0;
}
