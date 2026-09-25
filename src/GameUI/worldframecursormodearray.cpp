//============================================================================
//  CWorldFrameWar3's cursor-mode stack: four out-of-line members of
//  storm.h's own TSGrowableArray<T>, freshly instantiated over `enum
//  CursorMode` - 0x6F38CDE0 ComputeChunk, 0x6F38CE40 SetAlloc, 0x6F393100
//  Grow, 0x6F393150 SetCount.
//
//  The instantiation is real, not written out by hand: every one of the
//  four bodies below is Containers/tsarray.inl's own generic definition,
//  reached for a POD `T` whose per-element construct/destroy loops
//  disappear the same way Containers/intarray.cpp's TSGrowableArray<unsigned
//  int> and Item/itemhandletable.h's TSGrowableArray<int> already do -
//  `lea edi, ds:0[ebp*4]` in 0x6F38CE40 and `lea eax, [eax+ecx*4]` in
//  0x6F393100 both confirm sizeof(T) == 4.
//
//  `enum CursorMode` and not `int`: the four generic members MSVC emits for
//  a given T get exactly one mangled name apiece regardless of the Storm
//  tag string baked into the body, so a second instantiation over `int` (or
//  `unsigned int`) here would collide at link time with intarray.cpp's or
//  itemhandletable.h's - two different shipped functions sharing one C++
//  symbol.  `enum CursorMode` is the real element type besides: the tag
//  this file hands Storm, `.?AW4CursorMode@@`, is the same literal
//  gameuicursorstate.cpp's own kCursorModeTypeName already carries for
//  0x6F395DC0's release of the very same array, read there as the RTTI
//  descriptor name of `enum CursorMode`.
//
//  The array itself lives at CWorldFrameWar3 +0x1B0..+0x1C0
//  (worldframecursormodearray.h), one dword short of what gameui.h already
//  named there: TSGrowableArray<T>'s own layout is m_alloc/m_count/m_data/
//  m_chunk in that order, so +0x1B0 is the *capacity* and +0x1B4 the
//  *count* - the opposite of what `m_modeCount`/`m_modeCapacity` there
//  say.  ReleaseCursorModes (gameuicursorstate.cpp) sets both to zero and so
//  could not have told the difference; not renamed here, since
//  CWorldFrameWar3's own layout is a sibling session's to keep. This file
//  reaches the array through the reinterpret_cast in
//  worldframecursormodearray.h instead of through those two names, so the
//  swap costs nothing here.
//
//  Own translation unit: no direct callers within this call tree's own
//  functions live here - gameuicursorstate.cpp's PushCursorMode/
//  PopCursorMode are the callers, and per CLAUDE.md's "one translation unit
//  per original module" they stay with the rest of that file's cursor-mode
//  code rather than moving here for four calls.
//============================================================================
#define TSARRAY_TYPENAME ".?AW4CursorMode@@"
#include "tsarray.inl"
#include "worldframecursormodearray.h"

template unsigned int TSGrowableArray<CursorMode>::ComputeChunk(unsigned int);
template void         TSGrowableArray<CursorMode>::SetAlloc(unsigned int);
template CursorMode*  TSGrowableArray<CursorMode>::Grow();
template void         TSGrowableArray<CursorMode>::SetCount(unsigned int);
