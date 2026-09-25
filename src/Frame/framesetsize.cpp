//============================================================================
//  0x6F605D90 / 0x6F605DB0 - CLayoutFrame::SetWidth and ::SetHeight.
//
//  Four instructions each: store the float, overwrite the function's own
//  incoming argument slot with the 0 the tail call wants there, and jump into
//  QueueForLayout.  Spelling them as two one-line functions is what puts that
//  tail jump in the stream; folding them into one with an index would cost the
//  jump and add a branch.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  0x6F605D90 / 0x6F605DB0 - the two size setters.  Each is four
//  instructions: store the float, overwrite its own incoming argument slot
//  with the 0 the tail call wants there, and jump.  Spelling them as two
//  one-line functions is what puts that tail jump in the stream; folding
//  them into one with an index would cost the jump and add a branch.
void CLayoutFrame::SetWidth(float width)
{
    m_width = width;
    QueueForLayout(0);
}

void CLayoutFrame::SetHeight(float height)
{
    m_height = height;
    QueueForLayout(0);
}

