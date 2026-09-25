//============================================================================
//  0x6F352F30 - TSGrowableArray<NTempest::C3Vector>::~TSGrowableArray().
//  Same reasoning as vectorfree_imvector.cpp: IDA's label
//  `aAvc3vectorNtem` is a truncation of ".?AVC3Vector@NTempest@@", naming
//  the element type `NTempest::C3Vector` - most plausibly a 3-component
//  vector, though nothing in this call tree reads through one, so its
//  members stay an opaque reserved block.
//============================================================================
#include "tsarray.inl"

namespace NTempest
{
struct C3Vector
{
    char m_reservedUnknown[12];
};
}

template TSGrowableArray<NTempest::C3Vector>::~TSGrowableArray();
