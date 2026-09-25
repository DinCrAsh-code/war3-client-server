//============================================================================
//  0x6F47FD00 - the "agent component" CAgentTickRelay::FireEvent relays
//  into (agent+0x30's own vtable slot 0x20): a FourCC allow-list gate in
//  front of a further relay through this object's own +0x54 sub-object,
//  vtable slot 0x18.  Frame [6] of the CMissileThunderBolt impact pipeline
//  (docs/targets/cmissilethunderbolt_impact_pipeline.md) - the class this
//  method belongs to is not otherwise identified in this closure, so it is
//  modelled as a bare offset target on an anonymous struct rather than
//  named.
//============================================================================
#include "agenttickrelay.h"

typedef void* (__thiscall *AgentField54FireFn)(void* self, unsigned int eventCode, SAgentTickEventCtx* ctx);

struct SAgentEventRouter
{
    void* __thiscall DispatchByFourCC(SAgentTickEventCtx* ctx);
};

void* __thiscall SAgentEventRouter::DispatchByFourCC(SAgentTickEventCtx* ctx)
{
    unsigned int code = ctx->eventCode;

    switch (code)
    {
    case 0x63702661:
    case 0x6070656F:
    case 0x6066636C:
    case 0x606C6B6C:
    case 0x60706965:
    case 0x6072746C:
    case 0x6370266F:
    case 0x63702670:
    case 0x63702665:
    case 0x6370266C:
    case 0x63702677:
    case 0x63702678:
    case 0x706A7E65:
        break;
    default:
        return 0;
    }

    void* sub = *(void**)((char*)this + 0x54);
    if (!sub)
        return 0;

    void* vtable = *(void**)sub;
    AgentField54FireFn fn = *(AgentField54FireFn*)((char*)vtable + 0x18);
    return fn(sub, code, ctx);
}
