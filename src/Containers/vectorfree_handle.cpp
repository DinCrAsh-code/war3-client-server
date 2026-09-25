//============================================================================
//  0x6F4DC5B0 - TSGrowableArray<T>::~TSGrowableArray() for a third 4-byte
//  scalar element type (IDA's label for the tag string, "aG_4", is too
//  short a fragment to demangle with confidence).  `unsigned int` is reused
//  as the stand-in element type; the destructor's instructions do not
//  depend on which trivially-destructible 4-byte T it is.
//============================================================================
#include "tsarray.inl"

template TSGrowableArray<unsigned int>::~TSGrowableArray();
