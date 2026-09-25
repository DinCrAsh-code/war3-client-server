//============================================================================
//  0x6F471F90 - a `CAgent`-side method (`this` is the delegate itself, not a
//  `CAgentBaseAbs`): clear this agent's own `m_handle`/`m_typeTag` pair
//  (Agent/agent.h, +0x0C/+0x10) to "unbound", then, if `source` is non-null
//  and carries the exact `kAgentTagPathable` kind tag at its own +0x0C (the
//  same `CHandleObject::m_kindTag` shape Game/game.h already documents),
//  copy source's own +0x14/+0x18 pair into this agent's handle/type fields
//  and notify through this agent's own vtable slot 9 (+0x24,
//  `CAgent::Method_0x24` in agent.h - called through a raw cast here rather
//  than agent.h's own declared `int(int)` signature, since the real call
//  site pushes a pointer, not an int, and this file must not edit agent.h's
//  layout from underneath the parallel session that owns it).
//
//  `source` here is always the enclosing `CAgentBaseAbs` itself (see
//  Agent/cagentbaseabs_module_batch2.cpp's own `ForwardLoadToDelegate`) -
//  i.e. "bind this delegate's handle pair from the CAgentBaseAbs that owns
//  it".  A tiny raw struct, not a free `__fastcall`: it needs a real stack
//  argument alongside `this` (`retn 4`), which only a genuine thiscall
//  member reproduces (Pathfinding/crlagent.h's own note on why a free
//  function cannot be `__thiscall` in this compiler).
//
//  Own translation unit: `ForwardLoadToDelegate`'s own body is short enough
//  for this compiler to inline this at /Ob2, which the shipped dump's own
//  real `call sub_6F471F90` shows did not happen for the real binary.
//============================================================================

//  '+agl' MSB first - see Widget/widgetpathref.cpp's own copy of this
//  constant (kAgentTagPathable) for the full note; declared again locally
//  per this codebase's own convention for a constant every reader keeps its
//  own copy of rather than sharing one header.
static const unsigned int kAgentTagPathable = 0x2B61676C;

struct CAgentDelegateRaw
{
    void* BindSourceHandle(void* source);
};

void* CAgentDelegateRaw::BindSourceHandle(void* source)
{
    char* self = (char*)this;
    *(unsigned int*)(self + 0x0C) = (unsigned int)-1;
    *(unsigned int*)(self + 0x10) = (unsigned int)-1;

    if (source != 0 &&
        *(unsigned int*)((char*)source + 0x0C) == kAgentTagPathable)
    {
        *(unsigned int*)(self + 0x0C) = *(unsigned int*)((char*)source + 0x14);
        *(unsigned int*)(self + 0x10) = *(unsigned int*)((char*)source + 0x18);

        typedef void (__thiscall *Method0x24Fn)(void*, void*);
        ((Method0x24Fn)(*(void***)self)[9])(self, source);
    }

    return this;
}
