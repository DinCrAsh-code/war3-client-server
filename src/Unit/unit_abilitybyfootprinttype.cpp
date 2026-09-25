//============================================================================
//  0x6F06F2D0/0x6F06F9D0 - CUnit batch-19, slot 107's own depth<=2 closure.
//  Two ability-toggle bodies, both keyed off the unit's own
//  `m_footprintType` FourCC (widget.h, +0x30) rather than a fixed literal
//  the way unit_flagbit8.cpp's SetFlagBit8 is: this pair looks the
//  ability-type record up by whatever FourCC a different accessor
//  (0x6F0226A0 / 0x6F024910, both plain `mov eax, <imm32> / retn` bare
//  constants in Misc/misc_trivial_getters.cpp) returns, not by
//  `m_footprintType` directly - `m_footprintType` only feeds the second
//  function's own type-table probe (UnitTypeIntField1A8_6F29C240,
//  unittypequeries.cpp), a separate lookup from the ability-type-record
//  one both bodies also do through LookupAbilityTypeRecord.
//
//  Both reuse the exact `(manager, this, request)` AddAbilityFn call
//  shape unit_flagbit8.cpp's SetFlagBit8 already established (manager's
//  own vtable slot +0x80, unrelated to CUnit's *own* +0x80 slot -
//  ClearPendingAndNotify - which is a different object's vtable
//  entirely), and the same NotifyAbilityListChanged(manager) tail call.
//  The difference from SetFlagBit8: here the "request" object handed to
//  AddAbility is not a freshly-constructed SAbilityAddRequest but a
//  pointer straight into the looked-up ability-type record itself
//  (`record + 0x18`) - reusing storage the record already owns rather
//  than building a temporary.
//
//  Both bodies' "get the ability manager" step calls a *different*
//  address (0x6F06E3C0 / 0x6F06E970) than SetFlagBit8's own
//  GetAbilityManager (0x6F07ABC0) - zero stack arguments either way (the
//  `push esi`/`push edi` immediately before each call is this function's
//  own callee-saved register spill, not an argument - it is popped back
//  off *after* the call overwrites it with the return value, the normal
//  MSVC delay-the-push-to-first-use shape). Neither address has a dump
//  anywhere in this tree (`tools/dumpfn.py` reports nothing), so neither
//  can be reconstructed or even confidently named - both are NEED_DUMP
//  naked thunks in unit_abilitybyfootprinttypethunks.cpp, `retn 0`
//  because this is the only caller pushing anything at either address in
//  this whole repo and it pushes nothing.
//============================================================================
#include "unit.h"

struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);   // 0x6F0787D0
    void  RemoveAbility(void* ability);                            // 0x6F079CC0
    void  NotifyAbilityListChanged(void* manager);                 // 0x6F079990
};

//  Misc/misc_trivial_getters.cpp.
extern unsigned int GetFourCC_6F0226A0();
extern unsigned int GetFourCC_6F024910();

//  Item/abilitytyperecordtable.cpp - __fastcall, typeId in ecx.
extern void* __fastcall LookupAbilityTypeRecord(unsigned int typeId, int);

//  Pathfinding/rowtable.cpp - __fastcall, column in ecx.
extern int __fastcall RowTableLookup(int column);

//  unittypequeries.cpp - __fastcall, one arg in ecx.
extern int __fastcall UnitTypeIntField1A8_6F29C240(unsigned int fourCC);

//  unit_abilitybyfootprinttypethunks.cpp - neither has a dump anywhere in
//  this tree; both return a manager-shaped pointer taking no arguments.
extern void* GetAbilityManagerVariantA_6F06E3C0();
extern void* GetAbilityManagerVariantB_6F06E970();

typedef void (__thiscall *AddAbilityFn)(void* self, void* unit, void* request);

//  0x6F06F2D0 - unconditional add: if the ability-type record for
//  GetFourCC_6F0226A0() exists at all, hand it straight to AddAbility.
void CUnit::AddAbilityFromRecordUnconditional_6F06F2D0()
{
    void* record = LookupAbilityTypeRecord(GetFourCC_6F0226A0(), 0);
    if (record == 0)
        return;

    void* manager = GetAbilityManagerVariantA_6F06E3C0();
    void* request = (char*)record + 0x18;

    AddAbilityFn addAbility = (AddAbilityFn)(*(void***)manager)[0x80 / 4];
    addAbility(manager, this, request);

    ((SUnitAbilityHost*)this)->NotifyAbilityListChanged(manager);
}

//  0x6F06F9D0 - a footprint-type-gated toggle: any type field other than
//  2 means "drop the ability if the unit already carries it"; type field
//  2 means "add it if the unit doesn't, and only if the ability's own row
//  passes RowTableLookup" (confirmed off the dump's own `jz`/fallthrough
//  shape - the type-2 branch is the one that falls into the lookup/add
//  path, not the removal one).
void CUnit::ToggleAbilityByFootprintType_6F06F9D0()
{
    unsigned int fourcc = GetFourCC_6F024910();
    void* ability = ((SUnitAbilityHost*)this)->FindAbility(fourcc, 0, 0, 1, 1);
    int typeField = UnitTypeIntField1A8_6F29C240(m_footprintType);

    if (typeField != 2)
    {
        if (ability == 0)
            return;
        ((SUnitAbilityHost*)this)->RemoveAbility(ability);
        return;
    }

    if (ability != 0)
        return;

    void* record = LookupAbilityTypeRecord(fourcc, 0);
    if (record == 0)
        return;

    if (!RowTableLookup(*(int*)((char*)record + 0x1F8)))
        return;

    void* manager = GetAbilityManagerVariantB_6F06E970();
    void* request = (char*)record + 0x18;

    AddAbilityFn addAbility = (AddAbilityFn)(*(void***)manager)[0x80 / 4];
    addAbility(manager, this, request);

    ((SUnitAbilityHost*)this)->NotifyAbilityListChanged(manager);
}
