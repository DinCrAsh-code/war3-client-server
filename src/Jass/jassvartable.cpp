//============================================================================
//  TAllocatorHashTable<JassNativeSlot,HASHKEY_STR,64> - the variable/parameter
//  declaration table (jassdeclare.cpp's DeclareVariable calls through it).
//  Only four of its members fell in this batch's worklist - Ptr, Initialize,
//  CheckGrow, NewNode; Rehash (0x6F452520) is another batch's, and is
//  declared, not defined, by TAllocatorHashTable's own class body.
//============================================================================
#include "jasssymtables.h"
#include "allochash.inl"

template JassNativeSlot* JassVarTable::Ptr(const char*);
template void         JassVarTable::Initialize();
template int          JassVarTable::CheckGrow(unsigned int);
template JassNativeSlot* JassVarTable::NewNode(unsigned int, void*, void*);
