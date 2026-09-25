//============================================================================
//  JassScopeTable's five loop-carrying members - the same five
//  Render/prefetchhash.cpp's own CPrefetchTable instantiation gets, and for
//  the same reason: none of them can inline into another (MSVC declines a
//  body with a loop at /O2) and the shipped code has a real call at every
//  edge between them.
//
//      0x6F45FE70  Ptr(const char*)
//      0x6F457F80  Initialize()
//      0x6F458320  CheckGrow(unsigned int)
//      0x6F457FE0  Rehash(unsigned int)
//      0x6F458440  NewNode(unsigned int, void*, void*)
//
//  See jassscopetable.h for how HASHKEY_STR (Storm_508, case-sensitive) and
//  the node's own field offsets were pinned from this batch's own closure.
//============================================================================
#include "jassscopetable.h"
#include "allochash.inl"

//  JassScopeTable is a derived class over JassScopeTableBase (see
//  jassscopetable.h's own note on why), not a typedef the way every other
//  TAllocatorHashTable instantiation in this repo is - so these five
//  loop-carrying members belong to the base template and have to be
//  explicit-instantiated through its own name.  Calls through
//  JassScopeTable (e.g. `table.Ptr(...)`) still resolve to these exact
//  symbols: Ptr/Initialize/CheckGrow/Rehash/NewNode are inherited,
//  non-virtual members, so the mangled base-class symbol is what a
//  derived-object call site actually emits a reference to.
template JassSymbolNode* JassScopeTableBase::Ptr(const char*);
template void            JassScopeTableBase::Initialize();
template int              JassScopeTableBase::CheckGrow(unsigned int);
template void            JassScopeTableBase::Rehash(unsigned int);
template JassSymbolNode* JassScopeTableBase::NewNode(unsigned int, void*, void*);
