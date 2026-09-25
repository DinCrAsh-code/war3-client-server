//============================================================================
//  CGameDescStore's own vtable slot 4, 0x6F654F40 - one of the "shared
//  address" slots vtable_classes.json's own CGameDescStore notes flag:
//  its base-dtor call is 0x6F654270, already reconstructed as
//  `CDataStoreScratch::~CDataStoreScratch()` (cdatastorescratch.cpp), and
//  its own body is byte-for-byte the same "scalar deleting destructor"
//  shape as every other DeleteSelf in this family (netprovider_deleteself
//  .cpp and friends) - consistent with /OPT:ICF folding CDataStoreScratch's
//  own DeleteSelf into this slot of an unrelated class's vtable rather than
//  this genuinely being CGameDescStore's own code. Modelled as
//  CDataStoreScratch's own DeleteSelf (a real thiscall member, matching the
//  `this`=ecx / flags=stack / retn 4 shape the shipped code uses), but not
//  wired into CDataStoreScratch's own `virtual` table declared in
//  cdatastorescratch.h - this session did not establish which real slot
//  index it occupies there, and guessing would risk a wrong vtable order
//  for a class already compiled and scored elsewhere.
//============================================================================
#include "cdatastorescratch.h"

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

class CDataStoreScratchDeleter
{
public:
    //  0x6F654F40.
    void* DeleteSelf(unsigned int flags);
};

void* CDataStoreScratchDeleter::DeleteSelf(unsigned int flags)
{
    ((CDataStoreScratch*)this)->~CDataStoreScratch();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}
