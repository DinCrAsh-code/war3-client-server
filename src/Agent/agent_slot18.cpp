//============================================================================
//  0x6F472AE0 - CAgent's vtable slot 6 (+0x18).  See agent.h.
//
//  A FourCC dispatcher, shared unchanged by CWidget, CSelectable and CItem.
//  It only runs at all once four guards pass: the game data singleton
//  exists and has its +0x08 word set, this agent's own handle still
//  resolves, the object it resolves to carries the '+agl' kind tag, and
//  that object's +0x20 is clear.  Then it asks the second argument for its
//  payload (a plain +0x0C getter) and picks a handler on the selector.
//
//  Own translation unit: nine handlers, all real calls, all in this same
//  0x6F472xxx module - and every one of them is a tail call in the shipped
//  code, which only survives if they are in other files.
//============================================================================
#include "agent.h"
#include "game.h"

//  0x6F4A5EC0 - the payload getter.  agent_payload.cpp.
struct SSlot18Arg
{
    void* GetPayload();
};

//  '+agl' read low byte first, one letter along from agiletype.h's own
//  kAgileRootTypeId ('+aga') - the same tag CAgent::Method_0x5C
//  (agent_slot5c.cpp) checks.
//  '+agl' MSB first - the agile type id every handle-registered object
//  carries, one byte off CAgent's own '+aga'.  Same constant as
//  kAgileHandleTag (agentregistry.h), which carries the full note.
static const int kAgileLTag = 0x2B61676C;

void CAgent::Method_0x18(unsigned int selector, void* arg)
{
    CGameData* game = g_pGameData;
    if (game == 0)
        return;
    if (*(const int*)(game->m_reserved00 + 8) == 0)
        return;

    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    if (obj == 0)
        return;

    //  The same branchless `xor/cmp/setnz/sub/and` tag check
    //  CAgent::Method_0x5C uses - see docs/msvc-vc8-idioms.md.
    CHandleObject* checked = (obj->m_kindTag == kAgileLTag) ? obj : 0;
    if (checked == 0)
        return;
    if (*(const int*)((const char*)checked + 0x20) != 0)
        return;

    void* payload = ((SSlot18Arg*)arg)->GetPayload();

    //  A real `switch`: the shipped code is a bisecting compare tree over
    //  nine sparse constants with two of them sharing a handler twice over,
    //  which is exactly what MSVC lowers a sparse switch to.  Every arm
    //  tail-calls its handler (`pop edi` / `pop esi` / `retn 8` straight
    //  after the `call`), so none of the handlers may live in this file.
    switch (selector)
    {
    case 0x606C6B6C:  Handler_lkl(payload, arg); break;
    case 0x6066636C:  Handler_lcf(payload); break;
    case 0x6070656F:  Handler_oep(payload, arg); break;
    case 0x6072746C:  Handler_ltr(payload); break;
    case 0x60706965:  Handler_eip(payload, arg); break;
    case 0x63702677:  NotifyMessage67(); break;
    case 0x6370266F:
    case 0x63702670:  NotifyMessage66(); break;
    case 0x63702678:  NotifyMessage68(); break;
    case 0x706A7E65:
    case 0x63702661:  NotifyMessage65(); break;
    }
}
