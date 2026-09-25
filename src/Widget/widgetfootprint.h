//============================================================================
//  The pathing-footprint side of CWidget - three vtable slots (0x148, 0x150
//  and 0x154) that put the widget's footprint onto the pathing grid, take
//  it off again, and re-apply it after a move.
//
//  All three share the same opening: get the widget's footprint shape for
//  its type, ask the handle sub-object where the widget is and which way it
//  faces, turn the facing into one of the four quadrants, snap the position
//  to the footprint's own cell grid, and then hand shape + position +
//  direction to the grid.  That shared shape is why the three trees have 53,
//  46 and 31 functions between them but only a couple of dozen distinct new
//  ones.
//============================================================================
#ifndef WIDGETFOOTPRINT_H
#define WIDGETFOOTPRINT_H

#include "footprinttype.h"
#include "CFloat.h"

//  The two-CFloat position block every footprint call takes.  Contiguous by
//  construction: the shipped code passes one address for both the x/y pair
//  ApplyDirection snaps and the block FootprintVisitCells reads.  Both
//  members are left uninitialised - the callers assign them immediately, and
//  CFloat's zeroing default constructor would emit two dead stores at every
//  site (CFloat.h).
struct SFootprintPos
{
    SFootprintPos() : m_x(kCFloatNoInit), m_y(kCFloatNoInit) {}
    CFloat m_x;
    CFloat m_y;
};

//  0x6F32D0D0 - footprintshapealt.cpp.  The "alternate" footprint shape, the
//  twin of GetFootprintShape (0x6F32D0B0) reading a different object-editor
//  field.
CFootprintShape* __fastcall GetFootprintShapeAlt(unsigned int id);

//  0x6F3E0D30 (VisitCells) and 0x6F3DB4F0 (CellBox) are declared as members
//  of CFootprintShape in footprinttype.h.  They were free `__fastcall`
//  functions taking the shape as the first argument, which put the *second*
//  argument in edx and so pushed one argument fewer than either callee
//  cleans - see the note at their declaration.

//  0x6F468C70 - hand that box to the pathing grid.  ecx only.
void __fastcall PathGridInvalidateBox(void* box);

//  dword_6FA73A90 - set once the pathing grid is up; every one of the three
//  slots skips its bounding-box half while it is clear.
extern int g_pathGridReady;

#endif
