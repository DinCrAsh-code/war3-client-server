//============================================================================
//  0x6F457250 - TSExplicitList<JassSymbolNode>::UnlinkAll.  A fifth
//  instantiation of Containers/tslist.inl's own template (the fourth being
//  Misc/citemclosure_dbfieldhash.cpp's SDatabaseFieldRecord one) - its own
//  translation unit because it is a real call at every one of this batch's
//  own JassScopeTable call sites (Initialize/CheckGrow/Rehash/NewNode,
//  jasssymbolhash.cpp), the same way Containers/tslist.cpp's own
//  AGILE_TYPE_RELATIONSHIP instantiation is.
//============================================================================
#include "jassscopetable.h"
#include "tslist.inl"

template void TSExplicitList<JassSymbolNode>::UnlinkAll();
