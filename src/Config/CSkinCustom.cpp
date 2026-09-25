//============================================================================
//  CSkinCustom's own vtable slots.  Own translation unit per CLAUDE.md's
//  one-TU-per-original-module rule: 0x6F30ECC0/0x6F309820 sit in a
//  completely different address neighbourhood from CMiscCustom.cpp's own
//  0x6F002xxx family, so they shipped in a different module even though
//  the two classes are related by inheritance.
//============================================================================
#include "CSkinCustom.h"

//  Storm.dll ordinal 403 - see CMiscCustom.cpp's own declaration comment.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F30ECC0 - vtable slot 2.  Calls the base's own ~CMiscCustom()
//  directly (0x6F0024E0, the identical address CMiscCustom::DeleteSelf
//  calls) since this class declares no destructor of its own.
//----------------------------------------------------------------------------
CSkinCustom* CSkinCustom::DeleteSelf(unsigned int flags)
{
    this->CMiscCustom::~CMiscCustom();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F309820 - vtable slot 3.  See CSkinCustom.h's own note.
//----------------------------------------------------------------------------
unsigned int CSkinCustom::Method3() const
{
    return 1;
}
