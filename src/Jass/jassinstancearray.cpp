//============================================================================
//  TSGrowableArray<Instance*> - two of JassThreadLocal's own members
//  (jassthreadlocalctor.cpp: the +0x04 thread stack and the +0x88 array),
//  both plain pointers with no per-element construction to speak of, so
//  every loop the generic TSGrowableArray<T> template writes optimises down
//  to the same plain SMemReAlloc/SMemAlloc dword-copy dance ptrarray.cpp's
//  own note describes for TSGrowableArray<void*>.
//
//  The element type is not a guess: 0x6F44ADB0 hands Storm the RTTI
//  descriptor name `.PAUInstance@@` as its allocation tag - `struct
//  Instance*`, the Jass2/Instance.h VM-instance record CLAUDE.md's own
//  session note names.  Spelled `SJassThread*` here rather than `Instance*`:
//  jassthreadstate.h already names this exact object (it is the element
//  JassThreadSleep/JassThreadSyncStart read out of this same array) and
//  changing its C++ name now would move every mangled symbol its two
//  existing, already-scored functions reference for no change in the
//  instruction stream this file is judged on - the RTTI tag string above is
//  what the shipped code actually hands Storm, and that string does not
//  depend on what this repo calls the type in source.
//
//  0x6F449590 ComputeChunk (cap 0x40 = 256/sizeof(SJassThread*)), 0x6F44ADB0
//  SetAlloc.  Nothing in this call tree reaches this instantiation's
//  SetCount() or destructor out of line - JassThreadLocal's own constructor
//  is the sole caller, and its two SetCount(...) uses both inline (their
//  bodies call only ComputeChunk/SetAlloc, which is why those two are the
//  only members instantiated here).
//============================================================================
struct SJassThread;

#define TSARRAY_TYPENAME ".PAUInstance@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<SJassThread*>::ComputeChunk(unsigned int);
template void TSGrowableArray<SJassThread*>::SetAlloc(unsigned int);
