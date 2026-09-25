//============================================================================
//  JassStringToCString (0x6F3BAA40) - "a JASS string handle to the C string
//  behind it", jassnatives.h's own summary.  A JASS rc-string handle IS the
//  string object's own address (SHandleHolder/GetHandleOrZero,
//  Agent/handleobject.cpp - the same address-as-handle shape that class
//  already reads for a different owner type), so resolving it is
//  GetHandleOrZero followed by a "is this really a `TRIGSTR_NNN` reference
//  into the map's own string table" check.
//============================================================================
#include "jassnatives.h"
#include "handleobject.h"

//----------------------------------------------------------------------------
//  0x6F39F610 - lazily creates (or, with `forceInit` clear, merely fetches)
//  the world object's own rc-string value-name table at +0x3EC, under the
//  same `__except_handler4` SEH frame docs/msvc-vc8-idioms.md records as
//  unreproducible at this toolchain's fixed /GS- /EHs-c- (ResolveWidgetHandle,
//  jassnatives.h, is the same shape for a different table).  `retn 4`,
//  `this` (the world object) in ecx.
//----------------------------------------------------------------------------
struct SWorldRcStringTableOwner
{
    void* GetOrCreateValueNameTable(int forceInit);
};

__declspec(naked) void* SWorldRcStringTableOwner::GetOrCreateValueNameTable(int)
{
    __asm { mov eax, 06F39F610h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F6F82E0 - the value-name table's own lookup: find the entry whose key
//  equals `index` (0x6F6F8150 - a plain linear scan of a 0x144-byte-stride
//  TSGrowableArray, no dump reached in this batch) and return its own
//  string field at +0x10, or the shared `ValueName` fallback constant if
//  nothing matched.  `retn 4`, `this` (the table `SWorldRcStringTableOwner`
//  hands back) in ecx.
//----------------------------------------------------------------------------
struct SValueNameTable
{
    const char* LookupByIndex(int index);
};

__declspec(naked) const char* SValueNameTable::LookupByIndex(int)
{
    __asm { mov eax, 06F6F82E0h }
    __asm { jmp eax }
}

//  Storm_506/Storm_508 - already declared elsewhere (Render/cstatus.h,
//  Containers/allochashtable.h); redeclared locally as every other user of
//  them does.
unsigned int __stdcall Storm_506(const char* s);
int          __stdcall Storm_508(const char* a, const char* b,
                                 unsigned int max);

//  Storm ordinal 575 - not used anywhere else in this repo yet.  Called here
//  with a pointer just past a matched "TRIGSTR_" prefix and its result used
//  as a plain index, which is exactly `atoi`'s own contract.
int __stdcall Storm_575(const char* s);

//----------------------------------------------------------------------------
//  0x6F6F0A00 - does `s` start with the literal "TRIGSTR_", and if so what
//  numeral follows it?  `retn 0`, `this` (the candidate string) in ecx.
//  Storm_506 is strlen, Storm_508 is a bounded strncmp (both already
//  declared elsewhere in this repo - Render/cstatus.h,
//  Containers/allochashtable.h).
//----------------------------------------------------------------------------
int __fastcall ParseTrigstrIndex(const char* s)
{
    unsigned int prefixLen = Storm_506("TRIGSTR_");

    if (Storm_508(s, "TRIGSTR_", prefixLen) != 0)
        return -1;

    return Storm_575(s + prefixLen);
}

//----------------------------------------------------------------------------
//  0x6F3BAA40 - `__stdcall`, `retn 4`.  A null world object (nothing has
//  started a game yet) is a flat null result before anything else runs.
//----------------------------------------------------------------------------
const char* __stdcall JassStringToCString(int hString)
{
    if (!g_unk6FAB65F4)
        return 0;

    SWorldRcStringTableOwner* world = (SWorldRcStringTableOwner*)g_unk6FAB65F4;
    void* table = world->GetOrCreateValueNameTable(1);

    const char* str = (const char*)GetHandleOrZero((const SHandleHolder*)hString);

    if (!str || !*str)
        return 0;

    int index = ParseTrigstrIndex(str);

    if (index != -1)
        return ((SValueNameTable*)table)->LookupByIndex(index);

    return str;
}
