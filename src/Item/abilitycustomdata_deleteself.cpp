//============================================================================
//  CCustomData::DeleteSelf (0x6F25B130) and CAbilityCustomData::DeleteSelf
//  (0x6F25B570) - each class's vtable slot 2, the scalar deleting
//  destructor.  Byte-identical shapes: CAbilityCustomData adds no fields
//  of its own, so its own destructor reduces to nothing but a call into
//  the base, and both dumps make the exact same `call sub_6F256180`.
//
//  Own translation unit so ~CCustomData() (abilitycustomdata_dtor.cpp)
//  stays a real call rather than inlining into either of these - see that
//  file's own header note.
//============================================================================
#include "abilitycustomdata.h"

//  Storm.dll ordinal 403.  Same declaration as src/storm.h's, repeated
//  rather than pulling in that header's containers - see
//  docs/msvc-vc8-idioms.md, "Declaring imports so the diff can see them".
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kDelete[] = "delete";

CCustomData* CCustomData::DeleteSelf(unsigned int flags)
{
    this->CCustomData::~CCustomData();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

CAbilityCustomData* CAbilityCustomData::DeleteSelf(unsigned int flags)
{
    this->CCustomData::~CCustomData();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
