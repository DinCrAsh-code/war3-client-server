//============================================================================
//  Naked redirects for the callees CUnit::SetFlagBit8 (unit_flagbit8.cpp,
//  slot 42, 0x6F07B4A0) declares and calls but does not reconstruct.  None
//  is a reconstruction and none is ever hookable - same rule as
//  unit_herothunks.cpp.
//
//  Every argument count is the shipped call site's own push count, read
//  off each callee's own `retn` in its asm/ dump (tools/dumpfn.py).
//============================================================================

struct SUnitAbilityHost
{
    void  RemoveAbility(void* ability);
    void  NotifyAbilityListChanged(void* manager);
};

struct SAbilityAddRequest
{
    void Construct(unsigned int rawcode);
    void Destruct();
};

void* GetAbilityManager();

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F079CC0 (`retn 4`) - ecx = unit, one stack arg (the ability pointer
//  FindAbility returned).
ADDR_THUNK(void SUnitAbilityHost::RemoveAbility(void* ability), 0x6F079CC0)

//  0x6F079990 (`retn 4`) - ecx = unit, one stack arg (the ability manager
//  pointer AddAbility was just called on).
ADDR_THUNK(void SUnitAbilityHost::NotifyAbilityListChanged(void* manager), 0x6F079990)

//  0x6F25F5C0 (`retn 4`) - ecx = &request object, one stack arg (the same
//  rawcode already stored into the object's own first field).
ADDR_THUNK(void SAbilityAddRequest::Construct(unsigned int rawcode), 0x6F25F5C0)

//  0x6F251C50 (`retn` bare, 0 stack args) - ecx = &request object; releases
//  +0x8 through the generic handle-release path if it is non-null.
ADDR_THUNK(void SAbilityAddRequest::Destruct(), 0x6F251C50)

//  0x6F07ABC0 (`retn` bare, 0 stack args) - no `this`, no arguments: the
//  ability manager singleton getter.
ADDR_THUNK(void* GetAbilityManager(), 0x6F07ABC0)

#undef ADDR_THUNK
