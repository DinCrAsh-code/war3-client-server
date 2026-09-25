//============================================================================
//  0x6F0D5ED0 - `retn 0Ch`: a CAbility-derived class's own method, called
//  with `this` as the receiver throughout (kept in esi, never adjusted):
//  run the base CAbility::Method_6F052AF0 (0x6F052AF0, ability_slots.cpp,
//  called directly rather than through its own vtable slot - the shipped
//  `call sub_6F052AF0` with no indirection, i.e. an explicit
//  `CAbility::Method_6F052AF0(...)` in source, not a virtual dispatch),
//  resolve a chained handle reference through a sub-object at +0x80, and
//  arm a timer-shaped sub-object at +0x6C with event id 0xD01C1 - the same
//  event id CItem::RemoveFromWorld (item_remove.cpp) arms its own m_timer1
//  with, and the same SAgentTimerArm::Arm (0x6F4778F0) shape.
//
//  ability.h's own field-layout note says CAbility itself ends at +0x6C
//  (m_timer, an embedded CAgentTimer, +0x58..+0x6C), so +0x6C here is the
//  first byte of whatever this derived class adds - a second timer-like
//  sub-object, not CAbility's own m_timer.  Nothing in this call tree
//  names which concrete `CAbilityXxx` class this is, so it stays reached
//  through raw offsets past CAbility rather than through an invented
//  derived-class declaration.
//============================================================================
#include "ability.h"
#include "itemhandleresolve.h"

//  0x6F0419C0 - already reconstructed (handlereref_resolve.cpp);
//  re-declared here the way unitordermake.cpp and agentregistrysingleton
//  .cpp already do, so the mangled name is the same symbol.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  0x6F4778F0 - already reconstructed (item_remove.cpp); re-declared, not
//  re-defined, the same way item_armdroptimer.cpp does.
struct SAgentTimerArm
{
    void Arm(const CFloat* delay, int eventId, void* subject, int a, int b);
};

struct SAbilityWithTrailingTimerAndResolver
{
    char m_baseCAbility[0x6C];          // CAbility's own layout (ability.h)
    char m_timerLike06C[0x80 - 0x6C];   // +0x6C - a second timer sub-object
    char m_resolver080[0x0C];           // +0x80 - the chained-ref resolver

    //  `delay` arrives *by value* (the shipped `lea edx,[esp+32]` takes the
    //  address of this parameter's own stack copy right before the Arm
    //  call, rather than loading an already-held pointer) - a CFloat*
    //  parameter would just reload the pointer instead.
    void PrepareAndArm(void* target, const void* ref, CFloat delay);
};

void SAbilityWithTrailingTimerAndResolver::PrepareAndArm(
    void* target, const void* ref, CFloat delay)
{
    ((CAbility*)this)->CAbility::Method_6F052AF0(target);

    ((SOptionalHandleRefResolver*)&m_resolver080)->ResolveChained(ref);

    ((SAgentTimerArm*)&m_timerLike06C)->Arm(&delay, 0xD01C1, this, 0, 0);
}
