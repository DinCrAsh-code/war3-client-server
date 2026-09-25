//============================================================================
//  Naked redirects for the object-data-table callees this batch declares
//  and calls but does not reconstruct.  Each body jumps to the shipped
//  function at its real address, so the call runs original code; none of
//  them is a reconstruction and none is ever hookable (a JMP patch on one
//  of these would send the game straight back into the address it just
//  patched).
//
//  **Every argument count below came off the shipped call site's own push
//  count**, not off the callee's body - none of these has a dump, so
//  tools/thunk_abi_audit.py reports them as "no dump body for the target",
//  which is unknown rather than clean.  Nine of them were settled by hand
//  instead (`ida_query func_at <addr>` for the range, then `ida_query bytes
//  <end-3> 3` for the tail: `C3` is retn 0, `C2 lo hi` is retn <imm16>) and
//  carry the byte the image really ends with; the rest are still call-site
//  inference.  One of the nine was wrong - see 0x6F4C8990.
//
//      0x6F25F0B0  ecx = kind, edx = 0, nothing pushed        -> retn 0   (image: C3)
//      0x6F4CB790  ecx = path, edx = 0, nothing pushed        -> retn 0
//      0x6F4C9D20  ecx = list, edx = index, nothing pushed    -> retn 0
//      0x6F4C8990  ecx = object, edx = buffer, one push       -> retn 4
//                  (IDA: 0x6F4C8990..0x6F4C89C8 ends `C2 04 00`.  Declared
//                  as a __thiscall member `GetName(char*, int)` this pushed
//                  eight bytes into a callee that pops four - the frame
//                  shift class thunk_abi_audit.py exists for, and one it
//                  could not see because this address has no dump.)
//      0x6F25B720  nothing at all                             -> retn 0   (image: C3)
//      0x6F25B1A0  ecx = store, two pushes                    -> retn 8   (image: C2 08 00)
//      0x6F2B0000  ecx = key, nothing pushed                  -> retn 0
//      0x6F25C910  ecx = table, nothing pushed                -> retn 0   (image: C3)
//      0x6F25CCB0  ecx = table, two pushes                    -> retn 8   (image: C2 08 00)
//      0x6F25C9C0  four pushes, caller cleans none            -> retn 10h (image: C2 10 00)
//      0x6F25CBA0  four pushes, caller cleans none            -> retn 10h (image: C2 10 00)
//      0x6F4CA120  ecx = index, edx = key, nothing pushed     -> retn 0
//      0x6F2519F0  ecx = store, two pushes                    -> retn 8   (image: C2 08 00)
//      0x6F25B1D0  ecx = store, one push                      -> retn 4
//
//  0x6F005DC0 (SRowHost::MakeRow) is no longer here - it has a real body
//  now, in objectdatatable_rows.cpp (CAbility depth<=5 closure batch 1).
//============================================================================
#include "objectdatatable.h"

struct SBaseDataStore
{
    int KeyAt(int cursor, int flags);
    int Contains(const char* key, int flags);
    int Canonicalise(unsigned int packedKey);
};

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

ADDR_THUNK(CObjectDataTable* __fastcall MakeObjectDataTable(int, void*), 0x6F25F0B0)
ADDR_THUNK(int __fastcall ResolveDataPath(const char*, void*), 0x6F4CB790)
ADDR_THUNK(void* __fastcall IndexModifiedObject(void*, int), 0x6F4C9D20)
ADDR_THUNK(void __fastcall NamedDataObjectGetName(void*, char*, int), 0x6F4C8990)
ADDR_THUNK(SBaseDataStore* __cdecl GetBaseDataStore(), 0x6F25B720)
ADDR_THUNK(int SBaseDataStore::KeyAt(int, int), 0x6F25B1A0)
ADDR_THUNK(int SBaseDataStore::Contains(const char*, int), 0x6F2519F0)
ADDR_THUNK(int SBaseDataStore::Canonicalise(unsigned int), 0x6F25B1D0)
ADDR_THUNK(int __fastcall IsKnownDataKey(int), 0x6F2B0000)
ADDR_THUNK(int CObjectDataTable::RowCount(), 0x6F25C910)
ADDR_THUNK(int CObjectDataTable::QueryColumn88(void*, int), 0x6F25CCB0)
ADDR_THUNK(int __stdcall DataCellQueryInt(void*, int, int, int), 0x6F25C9C0)
ADDR_THUNK(const char* __stdcall DataCellQueryString(void*, int, int, const char*), 0x6F25CBA0)
ADDR_THUNK(void* __fastcall LookupModifiedObject(void*, const char*), 0x6F4CA120)

#undef ADDR_THUNK
