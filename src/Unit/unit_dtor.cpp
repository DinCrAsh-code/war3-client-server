//============================================================================
//  0x6F28B670 - CUnit's vtable slot 1 (+0x04): the scalar deleting
//  destructor.  See unit.h.
//
//  Same shape as CItem's (agentwar3_dtor.cpp) and for the same reason:
//  the shipped code makes a *real* `call sub_6F28B340` into ~CUnit rather
//  than having it inlined, so ~CUnit has to live in another translation
//  unit (unit_dtorbody.cpp) or this file's own copy of it disappears into
//  the call site.
//
//  It carries the `this != 0` test CItem's does and CWidget's/
//  CSelectable's do not - the same difference agentwar3_dtor.cpp records,
//  and for the same reason: with the destructor out of line the compiler
//  no longer knows a store through `this` has already happened by the time
//  it reaches the free, so it cannot drop the null check.
//
//  This is also the translation unit that emits ??_7CUnit@@6B@.  MSVC puts
//  a class's vtable in whichever TU defines its first declared non-inline
//  virtual, and for CUnit that is DeleteSelf - which is what makes
//  tools/check_vtables.py able to see all 118 slots at once.
//============================================================================
#include "unit.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CUnit* CUnit::DeleteSelf(int flags)
{
    this->CUnit::~CUnit();
    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
