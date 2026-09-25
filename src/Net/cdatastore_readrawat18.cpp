//============================================================================
//  0x6F4C3420 - a one-instruction alias of CDataStore::ReadRaw, i.e. a bare
//  `jmp` with `this` and both arguments already where the callee wants them.
//  IDA gives it its own name, so it is a genuine second entry point rather
//  than a chunk - the linker's ICF-style alias for a second inline expansion,
//  the same shape handleobject.cpp's 0x6F4C4660 already carries.
//
//  It is a member.  Declared as a free `__fastcall ReadRawAlias(CDataStore*,
//  void*, unsigned int)` - which is how it stood until the save game's own
//  slot-record read was written against it - it takes `dest` in edx where
//  the shipped alias takes it on the stack, cleans 4 bytes where the shipped
//  one cleans 8 (an abi_audit.py finding: "reads edx as an incoming argument
//  and the shipped function does not"), and MSVC declines the tail call
//  across the free-to-member boundary, so the one shipped instruction came
//  out as five.  As a member the conventions agree and the tail call is the
//  `jmp` the image has.
//============================================================================
#include "cdatastore.h"

CDataStore* CDataStore::ReadRawAlias(void* dest, unsigned int size)
{
    return ReadRaw(dest, size);
}
