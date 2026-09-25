//============================================================================
//  0x6F00A740 - TSGrowableArray<T>::SetAlloc for a 4-byte, trivially
//  copyable element (no placement-new/destructor calls in the shipped
//  stream at all, which is what a POD T folds Containers/tsarray.inl's
//  general SetAlloc down to).
//
//  Reached from SSprAnimList::Construct's "sorted TSGrowableArray of
//  sub-records" (Widget/sprresource.h), which is not itself reconstructed -
//  its other five callees (sub_6F50C370, sub_6F4EC9F0, sub_6F4E93B0,
//  sub_6F4EB100's qsort comparator) are below this dump's depth cut, so the
//  array this resizes is not established as anything more specific than
//  "one dword per element" here.  `unsigned int` stands in for that element
//  without claiming a business meaning this call tree cannot confirm.
//============================================================================
#include "storm.h"

#define TSARRAY_TYPENAME \
    ".?AVunsigned_int@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

template void TSGrowableArray<unsigned int>::SetAlloc(unsigned int);
