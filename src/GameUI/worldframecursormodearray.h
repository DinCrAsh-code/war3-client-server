//============================================================================
//  `enum CursorMode` - the shipped element type of the growable array
//  CWorldFrameWar3 embeds at +0x1B0 (gameui.h), named by its own Storm RTTI
//  descriptor tag `.?AW4CursorMode@@` (gameuicursorstate.cpp's
//  kCursorModeTypeName, worldframecursormodearray.cpp's own instantiation
//  tag).  No enumerators are named: nothing in this call tree reads a
//  cursor-mode value as anything but an opaque 4-byte token round-tripped
//  through SetCursorState's own `state` parameter.
//============================================================================
#ifndef WORLDFRAMECURSORMODEARRAY_H
#define WORLDFRAMECURSORMODEARRAY_H

#include "storm.h"

enum CursorMode {};

//----------------------------------------------------------------------------
//  CWorldFrameWar3's own +0x1B0 array, reached without depending on
//  gameui.h's (swapped) m_modeCount/m_modeCapacity names - see
//  worldframecursormodearray.cpp.
//----------------------------------------------------------------------------
inline TSGrowableArray<CursorMode>* GetCursorModeArray(void* worldFrame)
{
    return (TSGrowableArray<CursorMode>*)((char*)worldFrame + 0x1B0);
}

#endif
