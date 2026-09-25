//============================================================================
//  0x6F46A5E0 - TSGrowableArray<T>::~TSGrowableArray() for a second 4-byte
//  scalar element type, at a different address (hence a different original
//  translation unit) than intarray.cpp's TSGrowableArray<unsigned int>.
//  `int*` is a placeholder for "some 4-byte, trivially-destructible element"
//  - the destructor's own instructions do not depend on which one, only on
//  T being trivial, and IDA's own label for the RTTI tag string ("aE_1") is
//  too short a fragment to demangle with confidence.
//============================================================================
#include "tsarray.inl"

template TSGrowableArray<int*>::~TSGrowableArray();
