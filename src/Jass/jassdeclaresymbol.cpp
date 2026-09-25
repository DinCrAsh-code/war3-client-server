//============================================================================
//  0x6F464420 - declare a new symbol by name in whichever of the compile
//  scope's two tables is active right now (jassscopetable.h's own note on
//  +0x00/+0x04/+0x2C).  Unlike TAllocatorHashTable::SetKey (allochash.inl),
//  which duplicates the key string through Storm, this stores the caller's
//  own pointer directly - the shipped body never calls Storm_507, so
//  whatever owns `key`'s storage has to outlive the symbol table entry.
//
//  The `Storm_590(key)` call appears twice in the shipped body, once per
//  branch, rather than hoisted above the branch and shared - so it is
//  written the same way here rather than as a single call feeding a
//  ternary-selected table, which would compile to one call where the
//  target has two.
//============================================================================
#include "jassnodes.h"

JassSymbolNode* JassCompileContext::Declare(const char* key)
{
    unsigned int hash;
    JassScopeTable* table;

    if (m_insideFunctionBody)
    {
        hash = Storm_590(key);
        table = &m_localScope;
    }
    else
    {
        hash = Storm_590(key);
        table = &m_globalScope;
    }

    JassSymbolNode* node = table->NewNode(hash, 0, 0);
    node->m_hashval = hash;
    node->m_key = key;
    return node;
}
