//============================================================================
//  0x6F42B3C0 - another TSHashTable<T,K>::PtrHashOnly instantiation
//  (tshash.inl), the same "walk the bucket, compare only m_hashval, never
//  touch the key argument" shape 0x6F42CCF0's own handle-to-agent table
//  copy already documents - a different T/K pair the linker (no /OPT:ICF,
//  agentdefaults.cpp) gave its own body and address. Reached from CUnit's
//  own vtable BFS closure rather than that table's call tree, so the real
//  node type is unidentified; only the "has a vtable, so the node header
//  sits four bytes in" fact PtrHashOnly's own [eax+4] compare requires is
//  modelled, matching AGILE_TYPE_DATA's own reason for the same shape
//  (agiletype.h).
//============================================================================
#include "storm.h"
#include "tshash.inl"

class SUnidentifiedHashNode6F42B3C0
    : public TSHashObject<SUnidentifiedHashNode6F42B3C0, unsigned int>
{
public:
    virtual ~SUnidentifiedHashNode6F42B3C0();
};

template SUnidentifiedHashNode6F42B3C0*
TSHashTable<SUnidentifiedHashNode6F42B3C0, unsigned int>::PtrHashOnly(
    unsigned int, const unsigned int&);
