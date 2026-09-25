//============================================================================
//  0x6F45FF00 - JassCompileContext::Resolve.  Resolve an identifier: the
//  local scope first, falling back to the global one only on a miss.  The
//  mirror of JassCompileContext::Declare (jassdeclaresymbol.cpp), and the
//  natural reader of JassScopeTable::Ptr (jasssymbolhash.cpp).
//============================================================================
#include "jassnodes.h"

JassSymbolNode* JassCompileContext::Resolve(const char* key)
{
    JassSymbolNode* found = m_localScope.Ptr(key);
    if (found)
        return found;

    return m_globalScope.Ptr(key);
}
