//============================================================================
//  0x6F46DCE0 - given a raw agile type id (passed in the this-slot, not a
//  real object - the one caller, Agent/cagentbaseabs_binddelegate.cpp, hands
//  it CAgentBaseAbs's own +0x10 field), resolve the AGILE_TYPE_DATA record
//  for that id and invoke its pooling allocator's own parameterless hook.
//  Same `AGILE_TYPE_ID(id).Hash()` / `g_pGameData->m_types.Ptr(hash, id)`
//  shape Agent/agent_releaseself.cpp's `CAgent::ReleaseSelf` already
//  establishes for a *virtual* type id; this one is handed a raw value
//  directly, and the record's allocator is asked for slot 0
//  (`CAgileTypeAllocator::Slot00`, agiletype.h) rather than slot 1
//  (`ReleaseObject`, which takes an argument this call site's own `retn 0`
//  has no room to have pushed) - a genuine tail call, `jmp eax` straight
//  into the allocator's own vtable slot 0, whatever it leaves in `eax`
//  passed straight back to the caller (which uses it as the new delegate to
//  install).
//
//  A free function rather than a member: this compiler cannot spell a free
//  function `__thiscall` (Pathfinding/crlagent.h's own note on the same
//  restriction), so it is declared `__fastcall` instead - one register
//  argument compiles identically to a this-only thiscall body either way.
//
//  Own translation unit, not folded into Agent/cagentbaseabs_binddelegate.cpp:
//  its one caller there is a two-line function this compiler's /Ob2 would
//  gladly inline given the chance, which the shipped dump's own real
//  `call sub_6F46DCE0` shows did not happen - CLAUDE.md's own house rule
//  ("put a callee in its own TU rather than reaching for
//  __declspec(noinline)").
//
//  `g_pGameData` is read into its own local *before* `Hash()` runs (matching
//  agent_releaseself.cpp's own comment on why: MSVC cannot prove `Hash()`
//  leaves the global alone, so writing this inline instead makes it reload
//  the global after the call, which the shipped `push esi` / `mov esi,
//  g_pGameData` ahead of the `Hash()` call rules out).
//============================================================================
#include "agiletype.h"
#include "game.h"

//  agiletype.h's own `CAgileTypeAllocator::Slot00()` is declared `void()` -
//  correct for its one other, real caller, but this call site's own tail
//  `jmp eax` hands whatever the callee leaves in `eax` straight back as this
//  function's own return value, which a `void` call cannot do.  Reached
//  through a locally-declared raw interface with a real `void*` return
//  instead of agiletype.h's own declaration, so a genuine C++ virtual call
//  (rather than a manual vtable-slot pointer chase) is what this compiler
//  sees in tail position - the same shape that lets
//  Agent/cagentbaseabs_module_batch2.cpp's own `ForwardSaveToDelegate`
//  compile its own single forwarding virtual call straight down to a `jmp`.
struct CAgileTypeAllocatorSlot0Raw
{
    virtual void* Slot0();
};

void* __fastcall AgentBase_InvokeTypePoolSlot0(unsigned int typeIdRaw)
{
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeIdRaw);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    return ((CAgileTypeAllocatorSlot0Raw*)type->m_allocator)->Slot0();
}
