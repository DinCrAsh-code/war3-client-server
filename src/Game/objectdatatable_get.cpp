//============================================================================
//  0x6F261CC0 - the upgrade data table singleton, created on first use.
//
//  The cache slot is the same thread-local game-data context every other
//  data lookup in this binary goes through (Game/gamecontext.h): slot 0x0D,
//  then +0x10, then +0x24.  When it is empty the table is built by the data
//  loader with kind 9 and told to load "Units\\UpgradeData" through its own
//  vtable slot 2, with the path first turned into a string handle.
//
//  `lea ecx, [eax+9]` in the shipped stream is `mov ecx, 9`: eax was just
//  tested and found zero, so the compiler forms the constant off it.  There
//  is no source spelling for that (docs/msvc-vc8-idioms.md, "Constants
//  formed off a register the compiler knows is zero") and it is one
//  instruction.
//
//  Own translation unit: both of upgradeaffects.cpp's functions call it for
//  real, and at nine instructions /Ob2 would inline it on sight.
//============================================================================
#include "objectdatatable.h"
#include "gamecontext.h"

//  0x6F25F0B0 - the data loader's factory, and 0x6F4CB790 - a path string
//  to whatever handle the loader wants.  Neither has a body in this call
//  tree; both are naked redirects in objectdatatable_thunks.cpp, and both
//  take everything in registers (the shipped call sites push nothing).
CObjectDataTable* __fastcall MakeObjectDataTable(int kind, void* arg);
int __fastcall ResolveDataPath(const char* path, void* arg);

//  The cache slot the table is parked in.
struct SUpgradeDataCacheSlot
{
    char               m_reserved00[0x24];
    CObjectDataTable*  m_pUpgradeData;   // +0x24
};

struct SUpgradeDataSlotRoot
{
    char                    m_reserved00[0x10];
    SUpgradeDataCacheSlot*  m_pCache;    // +0x10
};

//  The loader interface, modelled only as far as the one slot this reaches.
struct SObjectDataLoader
{
    void** m_vtable;
};
typedef void (__thiscall *LoadSheetFn)(void* self, int pathHandle);

CObjectDataTable* GetUpgradeDataTable()
{
    SUpgradeDataSlotRoot* root =
        (SUpgradeDataSlotRoot*)GetThreadLocalSlot(kThreadLocalGameData);

    CObjectDataTable* table = root->m_pCache->m_pUpgradeData;
    if (table == 0)
    {
        table = MakeObjectDataTable(9, 0);
        //  The vtable is read *before* the path is resolved: the shipped
        //  code loads `[esi]` into edi and only then calls the resolver.
        void** vtable = *(void***)table;
        int path = ResolveDataPath("Units\\UpgradeData", 0);
        ((LoadSheetFn)vtable[2])(table, path);
    }
    return table;
}
