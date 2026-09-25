//============================================================================
//  0x6F07B4A0 - CUnit's vtable slot 42 (+0xA8), overriding CWidget's own
//  SetFlagBit8 (0x6F077E20, Agent/agentwar3_flagbits.cpp).  The base body
//  is a plain set/clear of bit 8 on CAgentWar3::m_flags; CUnit's override
//  doesn't touch m_flags at all - it toggles a hidden ability instead.
//  sub_6F10FA90's entire body is `mov eax, 4176756Ch / retn`, and
//  0x4176756C is the multichar literal 'Avul'.  Nothing in any dump here
//  resolves that rawcode to a readable ability name, so it stays a
//  literal rather than a guess - the same rule
//  Misc/misc_rawcode_getters.cpp already states for the FourCCs it can't
//  place.
//
//  Shape: look the ability up (SUnitAbilityHost::FindAbility,
//  Unit/unit_herothunks.cpp - `this` in ecx, five stack args, `retn 14h`).
//  If "the unit already has it" already matches the `on` argument, there
//  is nothing to do.  Otherwise: drop it if the unit has it and `on` is
//  false, or build a request object and hand it to the ability manager's
//  own AddAbility slot (index 0x20 off the manager 0x6F07ABC0 returns) if
//  the unit doesn't have it and `on` is true, then tell the manager the
//  unit's ability list changed.
//
//  0x6F25F5C0/0x6F251C50 (the request object's ctor/dtor) and
//  0x6F079CC0/0x6F07ABC0/0x6F079990 are declared as thunks in
//  unit_flagbit8thunks.cpp rather than reconstructed: none is specific to
//  this slot, each is its own substantial function (up to 276 own
//  instructions) with no established name anywhere else yet, and
//  docs/targets/CUnit__vtable.md already flagged this slot's closure as
//  too large to absorb in the same pass as the root - this is that
//  tradeoff made explicit rather than skipped.
//
//  The ceiling: the shipped body opens with a full __except_handler4
//  frame (security cookie, SEH scope-table push - the request object's
//  non-trivial destructor is what earns it) that this repo's fixed
//  /GS- /EHs-c- build cannot emit.  See docs/targets/JASS_Location.md's
//  "The ceiling: one SEH frame, twenty functions" for the identical shape
//  on nineteen other functions.  Past the frame the logic - both branch
//  conditions, both call sequences, the argument order - lines up 1:1;
//  the only other loss is the 0x10-byte stack-slot displacement the frame
//  puts under every `[esp+N]` after it.
//============================================================================
#include "unit.h"

//  0x6F0787D0 (Unit/unit_herothunks.cpp) plus the two more this slot
//  needs, declared in unit_flagbit8thunks.cpp: 0x6F079CC0 (`retn 4`,
//  drop an ability the unit already carries) and 0x6F079990 (`retn 4`,
//  tell the ability manager the unit's ability list changed).
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
    void  RemoveAbility(void* ability);
    void  NotifyAbilityListChanged(void* manager);
};

//  unit_flagbit8thunks.cpp.  `this` in ecx; the ctor's one stack argument
//  is the same rawcode the caller has already stored into the object's
//  own first field - not redundant, since the (GS-protected, not
//  reproducible here) ctor body does its own setup past the three plain
//  fields, including whatever fills +0x8 for the dtor to release.
struct SAbilityAddRequest
{
    unsigned int m_rawcode;   // +0x0
    int          m_field4;    // +0x4 - always 0 at this call site
    void*        m_field8;    // +0x8 - filled in by the ctor

    void Construct(unsigned int rawcode);
    void Destruct();
};

//  unit_flagbit8thunks.cpp (`retn 0`) - the ability manager singleton.
void* GetAbilityManager();

typedef void (__thiscall *AddAbilityFn)(void* self, void* unit, SAbilityAddRequest* request);

//  0x6F10FA90 - `mov eax, 4176756Ch / retn`.
static unsigned int GetFourCC_6F10FA90() { return 'Avul'; }

void CUnit::SetFlagBit8(int on)
{
    void* ability = ((SUnitAbilityHost*)this)
                        ->FindAbility(GetFourCC_6F10FA90(), 0, 0, 1, 1);

    if ((ability == 0) == (on == 0))
        return;

    if (ability != 0)
    {
        ((SUnitAbilityHost*)this)->RemoveAbility(ability);
        return;
    }

    SAbilityAddRequest request;
    request.m_rawcode = GetFourCC_6F10FA90();
    request.m_field4 = 0;
    request.m_field8 = 0;
    request.Construct(GetFourCC_6F10FA90());

    void* manager = GetAbilityManager();
    AddAbilityFn addAbility = (AddAbilityFn)(*(void***)manager)[0x80 / 4];
    addAbility(manager, this, &request);

    ((SUnitAbilityHost*)this)->NotifyAbilityListChanged(manager);

    request.Destruct();
}
