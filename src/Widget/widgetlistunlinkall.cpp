//============================================================================
//  0x6F35CD50 / 0x6F35CE50 - two more TSExplicitList<T,0>::UnlinkAll
//  instantiations, byte-identical to each other and to CHeroBar's own
//  placeholder instantiation (GameUI/herobar.cpp): a list whose link offset
//  is the compile-time constant 0, i.e. embedded at the very front of
//  whatever owns it, reached through CWidget::ClearPlayerMaskBits' closure.
//
//  Neither this dump nor ClearPlayerMaskBits' own call tree ever puts a
//  node into either list or names the owning class, so - exactly as
//  herobar.cpp's own comment says for the same shape - the element type
//  here is a placeholder: Containers/tslist.inl's generic walk never
//  dereferences T itself, only TSLink<T>'s own two fields, so any type of
//  the right size produces the identical instruction stream regardless of
//  what the real owner actually is.
//============================================================================
#include "storm.h"
#include "tslist.inl"

//  Two distinct incomplete element types (not `void` itself, which would
//  collide with CHeroBar's own `TSExplicitList<void, 0>` instantiation and
//  fail as a duplicate symbol) - same convention as herobar.cpp's own
//  comment: nothing here dereferences T, so an otherwise-unused declared
//  type is enough to give each address its own instantiation.
struct SUnlinkedListNode0;
struct SUnlinkedListNode1;

//  0x6F35CD50
template void TSExplicitList<SUnlinkedListNode0, 0>::UnlinkAll();
//  0x6F35CE50
template void TSExplicitList<SUnlinkedListNode1, 0>::UnlinkAll();
