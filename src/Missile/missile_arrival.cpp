//============================================================================
//  0x6F2C42D0 - CMissileThunderBolt impact pipeline, frame [0]: the
//  arrival/impact handler, reached through the missile's own vtable slot
//  0xB4 from CBulletBase::DispatchMessage (missile_dispatch.cpp)
//  for message ids 852374/852380.  This is the frame the operator's
//  breakpoint sat on, and the one that actually applies the hit.
//  docs/targets/cmissilethunderbolt_impact_pipeline.md.
//
//  ONE CORRECTION TO THE GROUND-TRUTH NOTES.  Those notes say
//  "`sub_6F2C1860` (vtable `+0xBC`) is called **unconditionally at
//  0x6F2C440E**, i.e. it runs even when no damage was dealt", and mark it
//  as worth re-checking.  Re-checked here against the disassembly, it is
//  the other way round: 0x6F2C440E is the *common* tail every early-out
//  falls into, and the one path that reaches it is the one that did NOT
//  deal damage.  The damage path returns at 0x6F2C43E9 - its own separate
//  epilogue, after the local ref's inline release - and never reaches
//  0x6F2C440E at all.  So the slot-0xBC step is "what to do when this
//  arrival did not land a hit", not an unconditional finalise.  Everything
//  else in the notes' description of this frame matches instruction for
//  instruction.
//
//  What the frame actually does, in order:
//    - three gates on the missile's own flags word at +0x20: bit
//      0x20000000 ("already dealt", the once-only guard), bit 0x10000000,
//      and bit 1 ("active").  Any of them failing goes straight to the
//      tail;
//    - resolve the missile's +0xA0 handle-ref slot and require it to be a
//      unit ('+w3u', GetUnitTypeFourCC - its type comes back through the
//      resolved object's own vtable slot 0x1C);
//    - take a counted reference to it for the duration (SCheckedUnitSlot);
//    - resolve the +0x2C handle-ref slot as well - that is the `data`
//      object - and build the impact context;
//    - three more gates, on `data` being set and on two queries against
//      the referenced unit;
//    - only then set the "dealt" bit and call DealDamage
//      (missile_dealdamage.cpp).  The bit is set *before* the call, which
//      is what makes it once-only even if DealDamage re-enters.
//============================================================================
#include "missile.h"
#include "missile_thunderbolt.h"
#include "itemhandleresolve.h"

extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4 - encoded 1.0

//  0x6F2BBA80 / 0x6F021A00 / 0x6F26C1C0 / 0x6F2BB570 - all reconstructed
//  elsewhere (misc_handle_resolvers.cpp, handleref_field54_guarded.cpp,
//  misc_rawcode_getters.cpp, misc_trivial_getters.cpp).  Re-declared here
//  rather than pulled in through a header because none of those files has
//  one; the spellings match theirs exactly, since MSVC mangles the calling
//  convention and the enclosing class into the symbol.
struct SHandleRefAt0xA0Owner { void* __thiscall GetField0x54IfSet(); };
void*        __fastcall QueryHandleField0x54IfSet(SOptionalHandleRef* ref);
unsigned int            GetUnitTypeFourCC();

//  vtable+0x1C on the resolved +0xA0 object: its agile type tag.
typedef unsigned int (__thiscall *AgentTypeTagFn)(void* self);

//  SRefCountedAgent - the reference-counted object the +0xA0 slot resolves
//  to - has moved to missile_thunderbolt.h, so that its two query methods
//  can be defined in refcountedagent_queries.cpp against the same class
//  declaration this call site uses.  MSVC puts the class name in the
//  mangled name, so a second copy that drifts is an LNK2019.

//  Both are real bodies now, in refcountedagent_queries.cpp.

//----------------------------------------------------------------------------
//  The scoped counted reference this frame holds across the whole gated
//  block is **SCheckedUnitSlot** (agenttypedslots.h), not a type of its own.
//
//  It used to be spelled here as a local `SAgentRef` with a naked redirect
//  to 0x6F038450 for its constructor - which is the address
//  SCheckedUnitSlot::SCheckedUnitSlot already occupies, reconstructed in
//  checkedunitslot.cpp.  Two C++ names for one shipped function is the
//  exact defect this repo's own rule warns about: the redirect wins at the
//  call site, so this frame ran the original while the reconstruction sat
//  unused, and `mix_config.json` offered the address under a name whose
//  body was `jmp` to itself.
//
//  The destructor is the same five-instruction release either way -
//  SCheckedUnitSlot::Release is that sequence, inlined at both exits, which
//  is why the shipped code carries it twice.
//----------------------------------------------------------------------------
#include "agenttypedslots.h"

#include "impactcontext.h"

//----------------------------------------------------------------------------
//  The 0x24-byte impact context this frame builds on its own stack and
//  hands to the last gate.  Its size is the gap the shipped frame leaves
//  between this local and the exception-handling slots above it; nothing
//  here reads a field of it, so nothing about its contents is asserted.
//----------------------------------------------------------------------------
//  A real body now (impactcontext_ctor.cpp), on the layout
//  impactcontext.h records.  Included rather than re-declared: the two
//  spellings have to agree exactly or the constructor call here resolves
//  to a second symbol.

//  0x6F2BCAC0 - retn 0Ch, on the missile itself: fill the context from the
//  missile, the resolved unit and a CFloat parameter.  Not reconstructed.
struct SMissileContextBuilder
{
    void __thiscall BuildImpactContext(SImpactContext* context, void* unit,
                                       const CFloat* param);
};

__declspec(naked) void __thiscall
SMissileContextBuilder::BuildImpactContext(SImpactContext*, void*, const CFloat*)
{
    __asm { mov eax, 0x6F2BCAC0 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F2C42D0 - CMissile's own vtable slot 45.  Renamed this session from
//  the mis-attributed `CMissileThunderBoltRefs::OnArrival` - that struct is
//  an organisational grouping of CMissile's non-virtual thunderbolt-ref
//  helpers, and this address is not one of them: it is reached through
//  `this`'s own vtable, so it belongs on CMissile itself as a real
//  override (of CBulletBase::InvokeSlot0xBC, the slot-45 alias every level
//  from CBulletBase through CBullet left untouched).  See missile.h.
//----------------------------------------------------------------------------
void CMissile::OnArrival()
{
    //  `goto` rather than nested `if`s or an `&&` chain, and the reason is
    //  load-bearing: written as `!(f & 0x20000000) && !(f & 0x10000000)`
    //  this compiler folds the two masks into one `test eax, 0x30000000`,
    //  where the shipped code tests each bit separately and branches to the
    //  same tail.  Three statements each with their own jump is what
    //  reproduces that, and it is also what keeps the resolve/type-check
    //  pair in the shipped register order.
    unsigned int flags = *(unsigned int*)((char*)this + 0x20);
    if (flags & 0x20000000)     //  already dealt - the once-only guard
        goto tail;
    if (flags & 0x10000000)
        goto tail;
    if (!(flags & 2))           //  not active
        goto tail;

    {
        void* unit = ((SHandleRefAt0xA0Owner*)this)->GetField0x54IfSet();
        if (unit == 0)
            goto tail;

        unsigned int typeTag = ((AgentTypeTagFn)(*(void***)unit)[0x1C / 4])(unit);
        if (typeTag != GetUnitTypeFourCC())
            goto tail;

        SCheckedUnitSlot ref((CAgent*)unit);

        void* data = QueryHandleField0x54IfSet(
                         (SOptionalHandleRef*)((char*)this + 0x2C));

        SImpactContext context;
        ((SMissileContextBuilder*)this)->BuildImpactContext(
            &context, unit, &g_slopeThreshold1);

        if (data != 0 &&
            ((SRefCountedAgent*)ref.m_value)->QueryHitAllowed(data, 0, 4) != 0 &&
            ((SRefCountedAgent*)ref.m_value)->QueryContextAllows(&context) != 0)
        {
            //  Set before the call, not after: this is what makes the hit
            //  once-only even if DealDamage re-enters the missile.
            *(unsigned int*)((char*)this + 0x20) |= 0x20000000;
            ((CMissileThunderBoltRefs*)this)->DealDamage(ref.m_value, data);
            return;
        }
    }

tail:
    //  vtable slot 0xBC/47 - a genuine virtual redispatch on `this`, now
    //  that CMissile declares a real Method_0xBC override (missile.h,
    //  missile_core.cpp).  Fixes a real dangling reference this session
    //  found via link_check.py: the placeholder `SVTableSlot0xBCOwner`
    //  punning struct this used to route through had no definition at
    //  all - a genuine LNK2019 that predates CMissile's own class
    //  existing (nothing to dispatch to before this session).
    this->Method_0xBC();
}
