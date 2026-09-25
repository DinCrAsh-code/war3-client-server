//============================================================================
//  0x6F36D7B0 - CInfoPanelGroup::CInfoPanelGroup.
//
//  The CInfoPanel base over the parent, the CObserver base at +0x124, both
//  vtables, and then one 2 x 6 grid: 0x15C bytes tagged with the shipped
//  source path (".../UI/CInfoPanelGroup.cpp" line 0x24 - which is what names
//  this class), sized 0.18593749 wide by 0.1015625 high, anchored point 7 to
//  point 7 of the panel itself at (-0.0015625, 0), and finally filled in.
//
//  The three floats are the shipped constants read straight out of .rdata at
//  0x6F939F18 / 0x6F939F1C / 0x6F939F20; the instruction score cannot see
//  any of them (both sides canonicalise a `fld <symbol>` to `fld SYM`), so
//  they are values this file has to get right on its own.
//
//  `this` is handed to SetPointTo as the anchor frame with no adjustment and
//  no null test, which is what says the CLayoutFrame subobject is at offset
//  zero - the same reading CInfoPanel's own SetAllPoints forces.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  a constructor with two bases and an owned allocation to unwind.
//
//  Own translation unit: four real calls out of it, one of them into its own
//  base's module.
//============================================================================
#include "infopanel.h"
#include "storm.h"

static const char kInfoPanelGroupFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CInfoPanelGroup.cpp";

CInfoPanelGroup::CInfoPanelGroup(void* parent)
    : CInfoPanel(parent)
{
    void* memory = SMemAlloc(0x15C, kInfoPanelGroupFile, 0x24, 0);

    m_pGrid = memory ? new (memory) CSimpleGrid(this) : 0;

    m_pGrid->SetWidth(0.18593749f);
    m_pGrid->SetHeight(0.1015625f);
    m_pGrid->SetPointTo(7, this, 7, -0.0015625f, 0.0f, 1);
    m_pGrid->SetSize(2, 6);

    PopulateGrid();
}
