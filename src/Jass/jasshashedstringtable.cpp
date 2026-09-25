//============================================================================
//  See jasshashedstringtable.h.  0x6F450150/0x6F453ED0 stay redirects for
//  the same four-vs-two (TSHashTable) and no-modelled-vtable
//  (TAllocatorHashTable, allochashtable.h's own note) reasons every other
//  hash-table constructor in this closure does; 0x6F44C340 is
//  0x6F450150's own TSExplicitList link-offset migration helper, reachable
//  only from that redirect (same situation as jasslinklistrelink.cpp's
//  three functions).  0x6F454390 is real: it only ever calls the redirect
//  above, so its own instruction stream is unaffected by anything the
//  redirect hides.
//============================================================================
#include "jasshashedstringtable.h"
#include "storm.h"

#define JASS_HASHEDSTRING_THUNK(addr) \
    {                                  \
        __asm { mov eax, addr }       \
        __asm { jmp eax }              \
    }

//  0x6F450150
__declspec(naked) SHashedStringHashTable* SHashedStringHashTable::Construct()
    JASS_HASHEDSTRING_THUNK(06F450150h)

//  0x6F453ED0
__declspec(naked) SHashedStringAllocTable* SHashedStringAllocTable::Construct()
    JASS_HASHEDSTRING_THUNK(06F453ED0h)

//  0x6F44C340 - `retn 0`, the same TSExplicitList relink helper
//  jasslinklistrelink.cpp's three others are, just this table's own
//  instantiation.
__declspec(naked) void __fastcall RelinkHashedStringBucketList(void*)
    JASS_HASHEDSTRING_THUNK(06F44C340h)

//----------------------------------------------------------------------------
//  0x6F454390 - `retn 0x10`.  Store the four constructor arguments, and
//  when no external table buffer was supplied (field8 == 0), allocate 0x38
//  bytes (".\Jass.cpp" line 0x156) and default-construct one in place;
//  on allocation failure the field is left null instead.
//----------------------------------------------------------------------------
SHashedStringTablePrimer::SHashedStringTablePrimer(void* field0, void* field4,
                                                     SHashedStringAllocTable* externalTable,
                                                     void* fieldC)
{
    m_field0 = field0;
    m_field4 = field4;
    m_table  = externalTable;
    m_fieldC = fieldC;

    if (externalTable == 0)
    {
        void* mem = SMemAlloc(0x38, ".\\Jass.cpp", 0x156, 0);
        if (mem != 0)
            m_table = ((SHashedStringAllocTable*)mem)->Construct();
        else
            m_table = 0;
    }
}
