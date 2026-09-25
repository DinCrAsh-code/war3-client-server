//============================================================================
//  0x6F002760 - CMiscCustom::DeleteSelf(), vtable slot 2, the scalar
//  deleting destructor.  Same shape as
//  Item/abilitycustomdata_deleteself.cpp's own DeleteSelf overrides.
//
//  Own translation unit, same reasoning as miscustom_dtor.cpp's own file
//  header: this file must see only ~CMiscCustom()'s declaration, never its
//  definition, or the destructor's own body (and from there
//  TeardownSources()'s own body) inlines straight into this function
//  instead of the real `call sub_6F0024E0` the dump shows.
//============================================================================
#include "CMiscCustom.h"

//  Storm.dll ordinal 403 - see Item/abilitycustomdata_deleteself.cpp's own
//  declaration comment for why this is not extern "C".
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kDelete[] = "delete";

CMiscCustom* CMiscCustom::DeleteSelf(unsigned int flags)
{
    this->CMiscCustom::~CMiscCustom();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
