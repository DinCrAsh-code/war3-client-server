//============================================================================
//  TSGrowableArray<CPreselectUnit*> - the preselection list's own growable
//  array of unit pointers.
//
//  0x6F3785F0 SetAlloc.  Byte for byte the same body as
//  TSGrowableArray<void*>::SetAlloc (0x6F3A1AA0, ptrarray.cpp): a pointer
//  element has nothing to construct, copy or destroy beyond its four bytes,
//  so every loop the generic template writes around those operations
//  optimises away and what is left is the SMemReAlloc / SMemAlloc /
//  dword-copy / SMemFree dance.  Two identical bodies at two addresses is
//  ordinary for this image, which was linked without /OPT:ICF
//  (agentdefaults.cpp).
//
//  It is a *separate* instantiation and not a second reference to
//  ptrarray.cpp's: the shipped code hands Storm the type descriptor name
//  ".?AV?$TSGrowableArray@PAVCPreselectUnit@@..." here and a different one
//  there, which is what says the original had two element types rather than
//  one array used twice.  Instantiating over a distinct pointer type is
//  also what keeps the two out of each other's way at link time - the same
//  symbol defined in two translation units is an LNK2005 that only
//  link_check.py would catch.
//============================================================================
#include "tsarray.inl"

//  Never defined: only ever named through a pointer, exactly as the shipped
//  code only ever moves four bytes at a time.
class CPreselectUnit;

template void TSGrowableArray<CPreselectUnit*>::SetAlloc(unsigned int);
