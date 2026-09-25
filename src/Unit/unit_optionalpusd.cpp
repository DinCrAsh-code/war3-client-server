//============================================================================
//  0x6F41CE90 - reached from CUnit's slot107 closure (per-template
//  initialiser, 0x6F2A0E30).  Behind a feature gate
//  (SOptionalHolder08::ForwardIfPresent(0x80218), Misc/cunit_agent8_batch1.cpp):
//  look up (or make) the '+pusd' agent (SCheckedSlot_6F40BD90,
//  checkedslot_batch2526.h/checkedslot_batch25.cpp), resolve two of its own
//  embedded SOptionalHandleRef sub-objects against `this` and the caller's
//  own argument (Adjuster_6F416E70/Adjuster_6F416D30,
//  Misc/misc_adjuster_thunks.cpp - +0x20 and +0x2C into
//  SOptionalHandleRefResolver::ResolveChained), run the agent's own vtable
//  slot 24 (+0x60) against the caller's argument, then hand the unit and
//  the agent to a not-yet-reconstructed finisher (0x6F416BA0, "below this
//  slice's own BFS cut") tagged with the same 0x80218 feature id.
//
//  Same unreproducible __except_handler4-shaped frame every converting
//  constructor in this family opens (docs/msvc-vc8-idioms.md).
//============================================================================
#include "unit.h"
#include "checkedslot_batch2526.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"

//  0x6F62A580 - already reconstructed (Misc/cunit_agent8_batch1.cpp) under
//  this exact class/member name; redeclared here (not redefined) so this
//  TU's own call resolves to the same mangled symbol.
struct SOptionalHolder08
{
    int ForwardIfPresent(int value);
};

//  0x6F416E70/0x6F416D30 - naked adjuster thunks into
//  SOptionalHandleRefResolver::ResolveChained (Misc/misc_adjuster_thunks.cpp),
//  declared `void()` there since a naked body ignores its own C++ signature;
//  the real ABI is thiscall(self, const void* ref), reached the same way
//  vtable_dispatch_audit.py's own typedef-and-cast technique reaches a raw
//  vtable slot.
extern void Adjuster_6F416E70();
extern void Adjuster_6F416D30();
typedef void* (__thiscall *ResolveChainedFn)(void* self, const void* ref);

//  0x6F40BD90 - checkedslot_batch25_getters.cpp.
unsigned int GetFixedRawcode_6F40BD90();

struct SMadeAgent_Pusd
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//  Agent's own vtable slot 24 (+0x60) - the one call sites elsewhere in
//  this family (widget_slot68.cpp) already reach through this offset,
//  here with a single scalar argument rather than a by-value CWorldVec3.
typedef void (__thiscall *AgentSlot0x60SingleArgFn)(void* self, int arg);

//  0x6F416BA0 - not reconstructed (below this slice's own BFS cut, like
//  its sibling adjusters' own targets): ecx = unit, two stack args (the
//  agent, then the same 0x80218 feature id the gate above checked).
struct SUnitFeatureFinisher
{
    void FinishFeature(unsigned int featureId, void* agent);
};

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

ADDR_THUNK(void SUnitFeatureFinisher::FinishFeature(unsigned int featureId, void* agent),
           0x6F416BA0)

#undef ADDR_THUNK

void CUnit::EnsureOptionalPusdAgent(int arg0)
{
    if (!((SOptionalHolder08*)this)->ForwardIfPresent(0x80218))
        return;

    SCheckedSlot_6F40BD90 temp(0);
    unsigned int rawcode = GetFixedRawcode_6F40BD90();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, rawcode, type->m_allocator);

    CAgent* found = ((SMadeAgent_Pusd*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);

    CAgent* agent = temp.m_value;
    ((ResolveChainedFn)Adjuster_6F416E70)(agent, this);
    ((ResolveChainedFn)Adjuster_6F416D30)(agent, (const void*)arg0);

    ((AgentSlot0x60SingleArgFn)(*(void***)agent)[0x60 / 4])(agent, arg0);

    ((SUnitFeatureFinisher*)this)->FinishFeature(0x80218, agent);
}
