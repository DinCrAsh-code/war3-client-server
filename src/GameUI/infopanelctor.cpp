//============================================================================
//  0x6F3388B0 - CInfoPanel::CInfoPanel.
//
//  Three statements: the CSimpleFrame base over the same parent, this
//  class's own vtable, and "fill the parent" with a re-layout.  Not part of
//  the JASS_PanCameraToTimedWithZ dump itself - it is CInfoPanelGroup's base
//  and the dump stops one level short of it - but it is in the
//  JASS_SetItemPosition tree, and reconstructing it is what lets the group's
//  own constructor be written as a constructor at all.
//
//  The parent goes into SetAllPoints with *no* +0xB4 adjustment and no null
//  test: a CSimpleFrame is a CLayoutFrame at offset zero, so the conversion
//  costs nothing, which is what the shipped `push edi` says.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  a constructor with a base to unwind.
//============================================================================
#include "infopanel.h"

CInfoPanel::CInfoPanel(void* parent)
    : CSimpleFrame(parent)
{
    SetAllPoints((CLayoutFrame*)parent, 1);
}
