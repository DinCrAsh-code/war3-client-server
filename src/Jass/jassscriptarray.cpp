//============================================================================
//  TSGrowableArray<JassScript*> - JassThreadLocal's own +0x40 member
//  (jassthreadlocalctor.cpp), a plain pointer array with the same trivial
//  copy/destroy ptrarray.cpp's own note describes for TSGrowableArray<void*>.
//
//  The element type is not a guess: 0x6F448820 hands Storm the RTTI
//  descriptor name `.PAVJassScript@@` as its allocation tag - `class
//  JassScript*`, the compiled-script object CLAUDE.md's own session note
//  names (Jass2/Compile.h).
//
//  0x6F4487C0 ComputeChunk (cap 0x40 = 256/sizeof(JassScript*)), 0x6F448820
//  SetAlloc.  As with jassinstancearray.cpp, JassThreadLocal's constructor
//  is the only caller and its own SetCount(1) call inlines everything but
//  these two out-of-line members.
//============================================================================
//  0x6F44B790 - jscript-bin1's own addition: `Grow<JassScript*>` (append one
//  raw slot, no placement-new - see tsarray.inl's own New()/Grow() split).
//  Same table, same instantiation, no new caller established in this
//  session's own closure beyond what jassthreadlocalctor.cpp already names.
class JassScript;

#define TSARRAY_TYPENAME ".PAVJassScript@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<JassScript*>::ComputeChunk(unsigned int);
template void TSGrowableArray<JassScript*>::SetAlloc(unsigned int);
template JassScript** TSGrowableArray<JassScript*>::Grow();
