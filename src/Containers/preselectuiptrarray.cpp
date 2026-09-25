//============================================================================
//  TSGrowableArray<CPreselectUnit*> - the same instantiation
//  Unit/preselectarray.cpp's SetAlloc (0x6F3785F0) already belongs to.  Two
//  module-global instances of it back the selection-visual object's own
//  factory (Widget/selectable_selectionvisual.cpp's CreateSelectionVisual,
//  0x6F37B7C0): a freelist of released visuals and the array of currently
//  live ones.  (The visual's own class is CPreselectUI by its vftable -
//  0x6F3798F0's ??_7CPreselectUI@@6B@ - so "CPreselectUnit" is one earlier
//  session's own guess at the pointee name for this pointer-only,
//  never-defined class; not this batch's to rename.)
//
//  ComputeChunk (0x6F378590) needs nothing pointee-specific - a pointer
//  array's ComputeChunk/SetAlloc/Grow never touch what T points to, so the
//  compiler can (and does) hand two distinct pointer instantiations
//  byte-identical bodies (docs/msvc-vc8-idioms.md, "A pointer-template
//  instantiation can be folded even in a link with no /OPT:ICF") - the
//  plain generic body, maxchunk = 256/sizeof(T*) = 0x40.
//
//  Grow() (0x6F37B580) and SetCount() (0x6F37B5D0) are real calls here: the
//  freelist is popped by a direct SetCount(count-1) rather than through
//  Grow, and the live array is appended to with Grow.
//============================================================================
#include "tsarray.inl"

class CPreselectUnit;

//  SetAlloc itself is not redeclared here: it has no element-specific
//  specialisation (T is a pointer), so this TU's own calls into it through
//  Grow/SetCount implicitly instantiate the same generic body
//  Unit/preselectarray.cpp already instantiates explicitly - two ordinary,
//  linker-folded copies of one template, the way any template function used
//  from two translation units is.

template unsigned int TSGrowableArray<CPreselectUnit*>::ComputeChunk(unsigned int);
template CPreselectUnit** TSGrowableArray<CPreselectUnit*>::Grow();
template void TSGrowableArray<CPreselectUnit*>::SetCount(unsigned int);
