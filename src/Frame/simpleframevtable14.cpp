//============================================================================
//  0x6F6089A0 - slot 14 (+0x38).  Pull the two floats a message-shaped
//  struct keeps at +0x24/+0x28 (the same offsets slots 20/21 read a mouse
//  message's x/y from - Frame/simplebuttonvtable20.cpp) into a local point
//  and forward to slot 10 (PointInRect).
//============================================================================
#include "frame.h"
#include "framethunks.h"

int CSimpleFrame::HitTestMessage(const void* msg) const
{
    float point[2];
    point[0] = *(const float*)((const char*)msg + 0x24);
    point[1] = *(const float*)((const char*)msg + 0x28);

    return PointInRect(point);
}
