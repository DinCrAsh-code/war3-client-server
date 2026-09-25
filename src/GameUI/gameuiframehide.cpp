//============================================================================
//  0x6F2F1010 - CSimpleFrame::Hide.
//
//  Set the "hidden" flag at +0x90 and tail-jump into vtable slot 0x68.  The
//  pair of slots 0x64/0x68 is what the tip's own construction uses in the
//  other order (`+0x90 = 0` then slot 0x64), which is what names them.
//
//  The `jmp edx` is MSVC's tail call on `return slot(this);`, not something
//  the source spells.
//============================================================================
#include "frame.h"

//  slot 0x68 (+0x1A) - the frame's own hide.
typedef void (__thiscall *FrameSlot0x68Fn)(void* self);

void CSimpleFrame::Hide()
{
    //  The slot is named before the flag is stored, because the shipped code
    //  loads it first; writing the store first and the dispatch as one
    //  expression schedules the load after it.
    FrameSlot0x68Fn hide = (FrameSlot0x68Fn)(*(void***)this)[0x68 / 4];

    m_shown = 1;
    hide(this);
}
