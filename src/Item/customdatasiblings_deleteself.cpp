//============================================================================
//  All six siblings' own vtable slot 2 (the scalar deleting destructor) -
//  byte-identical in shape to CCustomData::DeleteSelf/
//  CAbilityCustomData::DeleteSelf (Item/abilitycustomdata_deleteself.cpp):
//  none of the six adds any field of its own, so each compiler-generated
//  destructor reduces to nothing but the same `call sub_6F256180`
//  (CCustomData::~CCustomData) every other sibling's own dump calls too.
//  checksum-batch-B (2026-09-12).
//
//  Own translation unit, same reason as abilitycustomdata_deleteself.cpp:
//  ~CCustomData() (abilitycustomdata_dtor.cpp) has to stay a real `call`,
//  not get inlined into any of these.
//============================================================================
#include "customdatasiblings.h"

//  Storm.dll ordinal 403 - same declaration as abilitycustomdata_deleteself.cpp.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kDelete[] = "delete";

//  0x6F25B3F0.
CUnitCustomData* CUnitCustomData::DeleteSelf(unsigned int flags)
{
    this->CCustomData::~CCustomData();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F25B4B0.
CItemCustomData* CItemCustomData::DeleteSelf(unsigned int flags)
{
    this->CCustomData::~CCustomData();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F25B7B0.
CDestructableCustomData* CDestructableCustomData::DeleteSelf(unsigned int flags)
{
    this->CCustomData::~CCustomData();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F25B870.
CDoodadCustomData* CDoodadCustomData::DeleteSelf(unsigned int flags)
{
    this->CCustomData::~CCustomData();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F25B630.
CBuffCustomData* CBuffCustomData::DeleteSelf(unsigned int flags)
{
    this->CCustomData::~CCustomData();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F25B6F0.
CUpgradeCustomData* CUpgradeCustomData::DeleteSelf(unsigned int flags)
{
    this->CCustomData::~CCustomData();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
