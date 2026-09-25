//============================================================================
//  0x6F381D50 - CSelectionManager::RebuildCircleGeometry.
//
//  Four instructions: a flag at +0x20 picks one of two builders and the
//  chosen one is tail-jumped into, which is why neither arm can be spelled
//  as a plain call - both shipped arms are `jmp`, and only a tail call
//  reproduces them.  The two differ in one thing, the segment count: the
//  scaled builder takes it from +0xE4 (the constructor leaves 4 there) times
//  four, the default one uses a fixed eight.
//
//  Both builders are still redirects; see gameui_thunks.cpp.
//============================================================================
#include "selectionmanager.h"

void CSelectionManager::RebuildCircleGeometry()
{
    if (m_scaledGeometry != 0)
        BuildScaledCircleGeometry();
    else
        BuildDefaultCircleGeometry();
}
