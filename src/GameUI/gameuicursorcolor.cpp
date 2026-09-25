//============================================================================
//  0x6F393040 - CWorldFrameWar3::ApplyCursorColor.
//
//  Push the colour the current cursor mode should be drawn in into the
//  cursor's own render object.  Mode 8 - the "targeting a player" one - uses
//  that player's palette colour; every other mode has a fixed RGB triple in
//  a three-byte-per-entry table, folded into an opaque ARGB value by the
//  shipped `or 0FFFFFF00h` / `shl 8` chain (written in that order, which
//  docs/msvc-vc8-idioms.md records is what puts the two instructions this
//  way round).
//
//  The table is spelled as three separate byte arrays, one per component,
//  because IDA spells the three consecutive bytes as three separate symbols
//  and the canonicaliser compares operand *symbols*: one array with `+1` and
//  `+2` subscripts would cost two of the three loads their match.
//
//  Own translation unit: four real calls out of it.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "frame.h"
#include "playercolor.h"        // g_unk6FAB65F4

struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
};

//  0x6F60EDE0 - Misc/trivialpredicates.cpp: the render object's own head.
int __fastcall GetIndirectHead_6F60EDE0(const void* self);

//: byte_6F941354 / 55 / 56 - the fixed cursor colours, three bytes apart.
extern const unsigned char g_cursorColorRed[];
extern const unsigned char g_cursorColorGreen[];
extern const unsigned char g_cursorColorBlue[];

enum { kCursorModePlayerTarget = 8, kCursorColorChannel = 5 };

void CWorldFrameWar3::ApplyCursorColor()
{
    void* render = ((CScreenFrame*)GetGameUI(1, 0))->EnsureRenderObject(0);

    if (render == 0)
        return;

    void* sprite = (void*)GetIndirectHead_6F60EDE0(render);

    if (sprite == 0)
        return;

    unsigned int color;

    if (m_cursorMode == kCursorModePlayerTarget)
    {
        IndexedArrayHolder_6F3A1650* world =
            (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

        color = *GetPlayerColorValue(world->m_actingSlot);
    }
    else
    {
        int mode = m_cursorMode;

        color = ((((g_cursorColorRed[mode * 3] | 0xFFFFFF00u) << 8)
                  | g_cursorColorGreen[mode * 3]) << 8)
                | g_cursorColorBlue[mode * 3];
    }

    SpriteSetChannelColor(sprite, kCursorColorChannel, &color);
}
