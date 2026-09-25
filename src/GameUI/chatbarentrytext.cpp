//============================================================================
//  0x6F341410 - SSelectionHost::SetEntryText (the chat bar's editable line).
//
//  Three instructions of body: fetch the line at +0x1E0 and hand it the
//  string.  The trailing `1` is CTextFrame::SetText's own second argument
//  (0x6F615B50, `retn 8`) and not something this function decides.
//
//  Own translation unit: SetEntryActive calls it for real.
//============================================================================
#include "gameui.h"

void SSelectionHost::SetEntryText(const char* text)
{
    m_pEntryBox->SetText(text, 1);
}
