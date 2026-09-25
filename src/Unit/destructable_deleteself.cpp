//============================================================================
//  0x6F2674D0 - CDestructable's own vtable slot 1, DeleteSelf.
//
//  Own translation unit, same reason destructable_dtor.cpp's own header
//  comment gives: this makes a real `call` into ~CDestructable, and
//  same-TU MSVC would inline it away.
//============================================================================
#include "destructable.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CDestructable* CDestructable::DeleteSelf(int flags)
{
    this->CDestructable::~CDestructable();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
