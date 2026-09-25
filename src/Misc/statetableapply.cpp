//============================================================================
//  0x6F763F00 - SStateTable::ApplyByKind.
//
//  Four arms, and every one of them is a *tail* call - which is why the
//  shipped body rewrites its own second argument slot in place rather than
//  pushing a new frame.  -3 is the default, -1 and -2 are the presets 7 and 3
//  (but only when +0x00 is set; otherwise the kind collapses to 0 and falls
//  into the indexed arm), and anything else indexes the row array directly.
//
//  The dump's block for this heading runs the indexed arm (0x6F741720, a
//  shipped function of its own with no heading) in with the body, so the
//  heading's score covers the twenty-two instructions above it and the eight
//  after them are statetableindexed.cpp's.
//============================================================================
#include "statetable.h"

void SStateTable::ApplyByKind(void* value, int kind)
{
    //  The -3 arm is written last on purpose: the shipped block order tests
    //  it first and places its body after every other exit, which is what
    //  MSVC does with the arm the source leaves till the end.
    if (kind != -3)
    {
        if (kind == -2)
        {
            if (m_00)
            {
                ApplyPreset(value, 3);
                return;
            }
            kind = 0;
        }
        else if (kind == -1)
        {
            if (m_00)
            {
                ApplyPreset(value, 7);
                return;
            }
            kind = 0;
        }

        ApplyIndexed(value, kind);
        return;
    }

    ApplyDefault(value);
}
