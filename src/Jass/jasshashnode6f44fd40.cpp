//============================================================================
//  0x6F44FD40 - another `TSHashTable<T,K>::PtrHashOnly` instantiation
//  (tshash.inl), the same "walk the bucket, compare only m_hashval, never
//  touch the key argument" shape 0x6F42CCF0's own handle-to-agent table
//  copy documents - a different T/K pair the linker gave its own body and
//  address.  Reached from sub_6F454EC0/sub_6F454F40 (out of this
//  session's own closure, themselves reached only from
//  jassunknowntypelookup.cpp's two wrappers), so the real node type is not
//  identified.  Unlike Containers/cunit_agent8_hashinstantiation.cpp's own
//  unidentified node, this one's own match test reads `[eax]` directly
//  (not `[eax+4]`) - i.e. `m_hashval` sits at offset 0 with no vtable
//  pointer ahead of it - so the plain, non-polymorphic
//  `TSHashObject<T,K>` base (Storm/storm.h) is used unmodified rather than
//  adding a destructor to push the header four bytes in.
//============================================================================
#include "storm.h"
#include "tshash.inl"

class SUnidentifiedHashNode6F44FD40
    : public TSHashObject<SUnidentifiedHashNode6F44FD40, unsigned int>
{
};

template SUnidentifiedHashNode6F44FD40*
TSHashTable<SUnidentifiedHashNode6F44FD40, unsigned int>::PtrHashOnly(
    unsigned int, const unsigned int&);
