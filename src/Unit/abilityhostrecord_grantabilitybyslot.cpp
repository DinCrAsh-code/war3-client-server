//============================================================================
//  0x6F2095D0 - GrantAbilityBySlotSearch.  `this` (ebp) is a CUnit; `retn 8`,
//  two stack arguments.  Resolves the unit's own path-owner object
//  (+0x30, or CUnit::GetHandleObject()-style slow resolve via 0x6F472890
//  when unset - the same fallback shape ResolveAndNotifyPosition already
//  uses, on a receiver this call tree never otherwise names), then:
//
//  - if the unit's own +0x90 counter is zero AND arg_4 is zero, forwards
//    (arg_0, ownerPathRef->+0x30) to the out-of-scope 0x6F2077A0 with mode 0;
//  - else if arg_0 is null, same forward with mode 2;
//  - else scans the unit's own five-slot rawcode array at +0xF4 for
//    arg_0, treating 0x41556661/0x41556675 as wildcards that always match
//    the first slot; a full miss forwards with mode 1;
//  - on a hit at slot `i`, asks the out-of-scope 0x6F208A00(0, i) whether
//    the grant is allowed; if not, same forward with mode 1;
//  - if allowed, looks the ability up via SUnitAbilityHost::FindAbility
//    (0x6F0787D0, already reconstructed) and, if found, tells it through
//    its own vtable slot 0x2E4/4 = 185 and bumps a counter through the
//    out-of-scope 0x6F296F70; if not found, tells the *slot's own rawcode*
//    through the out-of-scope 0x6F2076F0 before the same 0x6F296F70 call;
//  - finally, if arg_4 is zero and the unit's own +0x90 counter (already
//    decremented) reaches zero, notifies through the out-of-scope
//    0x6F3329F0, then always through SAttachmentCounterNotify::OnCountChanged
//    (0x6F021270, already reconstructed) and SUnknownForwarder_0x6F26FE50
//    ::Method(0) (0x6F26FE50, already reconstructed).
//
//  Every out-of-scope callee here is a raw, address-derived guess past
//  what its own call site pins down - none of them is otherwise
//  identified anywhere in this codebase.
//============================================================================
#include "unit.h"

//  0x6F0787D0 - SUnitAbilityHost::FindAbility, already reconstructed
//  (unit_findability.cpp); re-declared here (method signature only, so the
//  mangled name binds to that one real symbol) the way every other caller
//  that doesn't already have the full struct in scope does.
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

//  0x6F2077A0 (`retn 0Ch`) - the shared "nothing granted" exit, reached
//  with three different `mode` values.
struct SUnknownGrantExit_6F2077A0
{
    void Report(int mode, void* ownerRawcode, void* arg0);
};
__declspec(naked) void SUnknownGrantExit_6F2077A0::Report(int, void*, void*)
{
    __asm { mov eax, 06F2077A0h }
    __asm { jmp eax }
}

//  0x6F208A00 (`retn 8`) - `this` is the unit; asks whether slot `index`
//  may be granted.
struct CUnitProbe_6F208A00
{
    int __thiscall CanGrantSlot(int flag, int index);
};
__declspec(naked) int CUnitProbe_6F208A00::CanGrantSlot(int, int)
{
    __asm { mov eax, 06F208A00h }
    __asm { jmp eax }
}

//  0x6F296F70 (`retn 8`) - `this` is the resolved ability's own path-owner
//  object; two stack arguments.
struct SUnknownAbilityCounter_6F296F70
{
    void __thiscall Bump(unsigned int matchedRawcode, int count);
};
__declspec(naked) void SUnknownAbilityCounter_6F296F70::Bump(unsigned int, int)
{
    __asm { mov eax, 06F296F70h }
    __asm { jmp eax }
}

//  0x6F2076F0 (`retn 4`) - `this` is the unit; one stack argument (the
//  matched slot's own rawcode).
struct CUnitNotify_6F2076F0
{
    void __thiscall NotifyRawcode(unsigned int rawcode);
};
__declspec(naked) void CUnitNotify_6F2076F0::NotifyRawcode(unsigned int)
{
    __asm { mov eax, 06F2076F0h }
    __asm { jmp eax }
}

//  0x6F3329F0 (`retn 0`) - `this` is the resolved ability's own path-owner
//  object; no stack arguments.
struct SUnknownAbilityNotify_6F3329F0
{
    void __thiscall Notify();
};
__declspec(naked) void SUnknownAbilityNotify_6F3329F0::Notify()
{
    __asm { mov eax, 06F3329F0h }
    __asm { jmp eax }
}

//  0x6F021270 - SAttachmentCounterNotify::OnCountChanged, already
//  reconstructed (Widget/attachmentcounters.cpp).
struct SAttachmentCounterNotify
{
    void __thiscall OnCountChanged();
};

//  0x6F26FE50 - SUnknownForwarder_0x6F26FE50::Method, already
//  reconstructed (Misc/cunit_agent5_wrapper_6f26fe50.cpp).
struct SUnknownForwarder_0x6F26FE50
{
    void Method(void* candidate);
};

//  The ability's own vtable slot 0x2E4/4 = 185 - a raw dispatch, the same
//  shape CLAUDE.md's own examples use, since this receiver is not a class
//  this call tree otherwise names.
typedef void (__thiscall* AbilitySlot185Fn)(void*);

//  0x6F472890 - SAttachmentOwnerQuery::QueryOwner, already reconstructed
//  (Widget/attachmentownerquery.cpp); re-declared here so the mangled name
//  binds to that one real symbol.
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();
};

//  `this` (a CUnit) in ecx, `retn 8` (two stack arguments) - a member, not
//  a free function, so it is declared on a minimal local receiver rather
//  than on the shared CUnit itself (unit.h).
struct CUnitAbilityGrantSearch
{
    void GrantAbilityBySlot(void* arg0, int forceAll);
};

void CUnitAbilityGrantSearch::GrantAbilityBySlot(void* arg0, int forceAll)
{
    void* unit = this;
    void* ownerRaw = *(void**)((char*)unit + 0x30);
    void* owner = ownerRaw;
    if (ownerRaw == 0)
    {
        owner = ((SAttachmentOwnerQuery*)unit)->QueryOwner();
    }

    void* ownerRawcode = *(void**)((char*)owner + 0x30);

    if (*(int*)((char*)unit + 0x90) == 0 && forceAll == 0)
    {
        ((SUnknownGrantExit_6F2077A0*)unit)->Report(0, ownerRawcode, arg0);
        return;
    }

    if (arg0 == 0)
    {
        ((SUnknownGrantExit_6F2077A0*)unit)->Report(2, ownerRawcode, arg0);
        return;
    }

    unsigned int rawcode = (unsigned int)(size_t)arg0;
    unsigned int* slots = (unsigned int*)((char*)unit + 0xF4);
    int i;
    for (i = 0; i < 5; i++)
    {
        if (i == 0 && (rawcode == 0x41556661u || rawcode == 0x41556675u))
            break;
        if (slots[i] == rawcode)
            break;
    }
    if (i >= 5)
    {
        ((SUnknownGrantExit_6F2077A0*)unit)->Report(1, ownerRawcode, arg0);
        return;
    }

    if (!((CUnitProbe_6F208A00*)unit)->CanGrantSlot(0, i))
        return;

    unsigned int slotRawcode = slots[i];
    void* found = ((SUnitAbilityHost*)unit)->FindAbility(slotRawcode, 0, 1, 1, 0);
    if (found != 0)
    {
        AbilitySlot185Fn slot185 = *(AbilitySlot185Fn*)(*(char**)found + 0x2E4);
        slot185(found);

        int count = *(int*)((char*)found + 0x50) + 1;
        ((SUnknownAbilityCounter_6F296F70*)owner)->Bump(slotRawcode, count);
    }
    else
    {
        ((CUnitNotify_6F2076F0*)unit)->NotifyRawcode(slotRawcode);
        ((SUnknownAbilityCounter_6F296F70*)owner)->Bump(slotRawcode, 1);
    }

    if (forceAll == 0)
    {
        int* counter90 = (int*)((char*)unit + 0x90);
        *counter90 -= 1;
        if (*counter90 == 0)
            ((SUnknownAbilityNotify_6F3329F0*)owner)->Notify();
    }

    ((SAttachmentCounterNotify*)unit)->OnCountChanged();
    ((SUnknownForwarder_0x6F26FE50*)owner)->Method(0);
}
