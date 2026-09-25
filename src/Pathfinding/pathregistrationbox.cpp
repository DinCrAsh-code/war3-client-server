//============================================================================
//  0x6F49E0A0 - overwrite a registration's cell rectangle outright.
//
//  Not CGridRegistration::UpdateBox (0x6F49FF90): that one re-registers the
//  difference between the old rectangle and the new one in the grid's cell
//  lists.  This one only replaces the four numbers, and is what a caller uses
//  when it is about to (or has just) done the grid bookkeeping itself.
//============================================================================
#include "spatialgrid.h"

void CGridRegistration::SetBox(const SCellBox* box)
{
    m_box = *box;
}
