//============================================================================
//  0x6F3FD470 - TSHashTable<CPathingFootprintReg,HASHKEY_STRI>'s own vtable
//  slot 2, the scalar deleting destructor: call the real Destruct()
//  (pathingfootprintregtable_ctors.cpp) then, if bit 0 of `flags` is set
//  and `this` is non-null, free through Storm's own allocator - same shape
//  bulletbase_ctordtor.cpp's DeleteSelf and Missile/missile_core.cpp's own
//  slot-47 override use elsewhere in this repo. A free function taking the
//  table as an explicit argument, not a member of this repo's own
//  `TSHashTable<T,K>` template, for the same reason
//  pathingfootprintregtable_ctors.cpp's own header note gives: the real
//  shipped table is a four-slot vtable this repo's simplified two-slot
//  template does not model, so nothing here may stamp it.
//============================================================================
#include "storm.h"

struct SPathingFootprintRegTable
{
    void Destruct();   // 0x6F3FCBC0, pathingfootprintregtable_ctors.cpp
};

void* __fastcall TSHashTable_CPathingFootprintReg_ScalarDeletingDestructor(
    void* table, unsigned int flags)
{
    ((SPathingFootprintRegTable*)table)->Destruct();

    if ((flags & 1) && table != 0)
        SMemFree(table, "delete", -1, 0);

    return table;
}
