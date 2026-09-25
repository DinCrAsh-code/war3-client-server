//============================================================================
//  0x6F001F70 - CAgent::ReleaseSelf().  See agent.h.
//
//  vtable slot 0 of CWidget, CSelectable *and* CItem alike
//  (user_knowledge.json's three vtable dumps all name this same address at
//  index 0), i.e. the shared base implementation none of the three
//  overrides - which is exactly the slot agent.h already reserved for
//  `CAgent::ReleaseSelf` on the strength of `CAgentPtr::AssignChecked`'s
//  `call [ecx][0]` alone.  Until this dump arrived nothing reached the
//  body, so agent.cpp carried a `{}` placeholder for it (see the block
//  comment there, which lists the whole family); that placeholder is gone
//  now and the real body lives here.
//
//  Own translation unit, same reasoning as agent_ctor.cpp/agent_dtor.cpp:
//  0x6F001F70 sits in the low 0x6F001xxx neighbourhood with
//  CAgent::CAgent() (0x6F001730) and CAgent::~CAgent() (0x6F0017F0), not
//  in agent.cpp's own 0x6F02xxxx/0x6F42xxxx cluster, and it makes a real
//  out-of-line call to TSHashTable<AGILE_TYPE_DATA,...>::Ptr - which only
//  survives as a call because that template's instantiation lives in
//  tshashtype.cpp rather than here.
//
//  What it does: ask the object for its own agile type id through
//  vtable[0x1C] (slot 7 - a genuine virtual call, `call [[this]+1Ch]`, not
//  a direct call to this class's own override), look that id up in the
//  game-wide agile type registry (g_pGameData->m_types, game.h), and hand
//  `this` to the allocator that type record parks at +0x70, through *its*
//  vtable slot 1.  So a CAgent does not delete itself: it returns itself
//  to the pool its registered type owns, which is why nothing in this
//  codebase ever reaches a CAgent destructor through a live `delete`
//  (agent.h's own note on ~CAgent()).
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"

//  Storm.dll ordinal 465 - same declaration shape as netconnref.cpp's.
void __stdcall SErrSetLastError(unsigned int code);

void CAgent::ReleaseSelf()
{
    //  A named local, not a temporary folded into the two uses: the shipped
    //  code stores the id into a stack slot and then takes its address
    //  twice (once as Hash()'s `this`, once as Ptr()'s `key` reference),
    //  which is what an AGILE_TYPE_ID lvalue looks like and what a
    //  `Ptr(AGILE_TYPE_ID(GetAgileTypeId()).Hash(), ...)` temporary does
    //  not - that spelling needs two separate temporaries because the two
    //  argument evaluations are sequenced apart.
    AGILE_TYPE_ID id(GetAgileTypeId());

    //  The singleton is read into its own local *before* Hash() runs and
    //  held across it in a callee-saved register (the shipped
    //  `push edi` / `mov edi, dword_6FAB7368` / ... / `lea ecx,[edi+0Ch]`).
    //  Writing it inline as `g_pGameData->m_types.Ptr(...)` instead makes
    //  MSVC reload the global after the call - it cannot prove Hash()
    //  leaves it alone - and the `push edi` disappears with it.  No null
    //  check on either: the caller already had to be alive for this object
    //  to exist at all.
    //  The *singleton pointer*, not a reference to its m_types member: a
    //  reference makes MSVC fold the +0x0C in at the load (`add edi,12`)
    //  and then `mov ecx,edi` at the call, where the shipped code keeps
    //  the base in edi and adjusts at the point of use (`lea ecx,[edi+0Ch]`).
    CGameData* game = g_pGameData;

    //  The hash in its own local, so it is evaluated *before* the argument
    //  pushes rather than between them: the shipped code calls Hash() and
    //  only then pushes `&id` and the result, which is the order a named
    //  local gives and a nested `Ptr(id.Hash(), id)` does not (that one
    //  pushes the right-hand `&id` first, then calls Hash).
    unsigned int hash = id.Hash();

    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    if (type == 0)
    {
        //  Storm's "report and carry on" shape, not an early return: the
        //  shipped code falls straight through into the +0x70 load with
        //  eax still zero and faults if the type really is unregistered.
        //  Written as the assignment MSVC compiles to the `xor eax,eax`
        //  that follows the call, rather than as a `return`, because a
        //  `return` here splits the epilogue in two and loses the shared
        //  exit (docs/msvc-vc8-idioms.md, "Shared exits need goto").
        SErrSetLastError(0x57);
        type = 0;
    }

    //  A genuine virtual call through a real, named member (agiletype.h's
    //  own m_allocator, at +0x70) - see that field's comment for why it is
    //  spelled that way rather than as a raw-offset cast.
    type->m_allocator->ReleaseObject(this);
}
