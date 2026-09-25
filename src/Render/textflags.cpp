//============================================================================
//  0x6F4CC380 - turn the frame layer's own style word into the layout
//  engine's flag word.  Nine bits move, none of them to the same place.
//
//  A run of `if (x & bit) result |= other;` statements and not a table: the
//  shipped stream tests each bit against the incoming word in place and ORs
//  a literal in, in source order, with no lookup anywhere.
//============================================================================
#include "text.h"

int __fastcall RemapTextFlags(int style)
{
    int flags = 0;

    if (style & 0x100)
        flags = 1;
    if (style & 0x200)
        flags |= 4;
    if (style & 0x400)
        flags |= 8;
    if (style & 0x800)
        flags |= 0x10;
    if (style & 0x40)
        flags |= 2;
    if (style & 0x1000)
        flags |= 0x200;
    if (style & 0x2000)
        flags |= 0x400;
    if (style & 0x4000)
        flags |= 0x800;
    if (style & 0x8000)
        flags |= 0x1000;

    return flags;
}
