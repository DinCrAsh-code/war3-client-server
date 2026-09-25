//============================================================================
//  CInfoPanel and CInfoPanelGroup - the bottom-left panel that describes
//  whatever is selected, and the group of them.
//
//  Both names are the shipped ones.  CInfoPanel's constructor stamps
//  ??_7CInfoPanel@@6B@ (0x6F3388F0); CInfoPanelGroup's stamps
//  ??_7CInfoPanelGroup@@6B@ at +0 and its `_0` twin at +0x124, and tags its
//  own allocation "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/
//  CInfoPanelGroup.cpp".
//
//  The +0x124 twin is what fixes the second base.  CSimpleFrame is exactly
//  0x124 bytes, CInfoPanel adds no member of its own, and the object
//  constructed at +0x124 is a CObserver (`lea edi, [esi+124h]` /
//  `call sub_6F629A70`) - so the group is a CInfoPanel *and* a CObserver, in
//  that order, and its own first field lands at +0x130, which is exactly
//  where the grid goes.
//============================================================================
#ifndef INFOPANEL_H
#define INFOPANEL_H

#include "frame.h"
#include "observer.h"

//----------------------------------------------------------------------------
//  0x6F3388B0 - a CSimpleFrame that fills its parent, and nothing else.
//----------------------------------------------------------------------------
class CInfoPanel : public CSimpleFrame
{
public:
    CInfoPanel(void* parent);       // 0x6F3388B0, `retn 4`
};

class CInfoPanelGroup : public CInfoPanel, public CObserver
{
public:
    CInfoPanelGroup(void* parent);  // 0x6F36D7B0, `retn 4`

    //  0x6F369770 - build the group's cells into the grid below.  `retn 0`;
    //  still a redirect (GameUI/gameui_thunks.cpp).
    void PopulateGrid();

    //  +0x130 - the 2 x 6 grid the panel lays its cells out in, 0x15C bytes,
    //  which is CSimpleGrid's own size to the byte.
    CSimpleGrid* m_pGrid;           // +0x130
};

#endif
