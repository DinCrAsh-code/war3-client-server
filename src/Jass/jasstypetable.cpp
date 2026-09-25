//============================================================================
//  TAllocatorHashTable<JASSTYPENODE,HASHKEY_STR,64> - the JASS type-name
//  registry ("handle" and its siblings; 0x6F454D04 stores the "handle"
//  literal straight into a node this table owns).  PtrInterned and Rehash
//  were this batch's own worklist entries; Initialize (0x6F451EF0),
//  CheckGrow (0x6F4537F0) and NewNode (0x6F454250) are jassvm-batch-1's
//  addresses, reconstructed here anyway (same generic template body every
//  other instantiation in this file already uses) because a member this
//  batch's own DeclareChildType (jasstypedecl.cpp) calls has to have a
//  definition somewhere for the .mix to link - see tools/link_check.py.
//  Batch-1 landing its own copy of these three under the same instantiation
//  is a duplicate-definition merge conflict to resolve by keeping one, not
//  a sign either copy is wrong.
//============================================================================
#include "jasssymtables.h"
#include "allochash.inl"

template JASSTYPENODE* JassTypeTable::PtrInterned(const char*);
template void          JassTypeTable::Initialize();
template int           JassTypeTable::CheckGrow(unsigned int);
template void          JassTypeTable::Rehash(unsigned int);
template JASSTYPENODE* JassTypeTable::NewNode(unsigned int, void*, void*);
