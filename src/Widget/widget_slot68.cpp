//============================================================================
//  0x6F2AF6A0 - CWidget's vtable slot 68 (+0x110), shared unchanged by
//  CSelectable and CItem: hand this widget's world position to a freshly
//  made widget-side agent, clear a set of bits from the widget's own player
//  mask, and tell the game UI about it.
//
//  The agent is built, used and dropped inside this one call: a type-checked
//  reference-counted slot (widgetagentslot.h) holds it for exactly the two
//  calls that need it and releases it on the way out, which is what the
//  refcount decrement and conditional slot-0 release at the tail are.
//
//  The 'hgw+' type id is fetched *twice* - once to find the pool the agent
//  comes from and once to fill the request block in - rather than kept in a
//  register.  That is the shipped code, and it is why GetWidgetAgileTypeId
//  is out of line at all (agilewidgettype.cpp).
//
//  The world position goes to the agent's own vtable slot 24 (+0x60) *by
//  value*: twelve bytes reserved with `sub esp,0Ch` and filled in from the
//  CWorldVec3 the handle sub-object hands back, not a pointer to it.
//
//  Same unreproducible __except_handler4-shaped SEH frame every destructible
//  local in this family carries (docs/msvc-vc8-idioms.md) - the slot's own
//  release has to run if anything between here and the end throws.
//
//  Own translation unit: eight real calls out of it.
//============================================================================
#include "widget.h"
#include "widgetagentslot.h"
#include "widgetagentquery.h"
#include "agiletype.h"
#include "agent.h"
#include "game.h"

typedef CPathRef* (__thiscall *GetHandleObjectFn)(void*);
//  The held agent's own vtable slot 24 (+0x60): (widget, world position by
//  value, mask).
typedef void (__thiscall *AgentSlot0x60Fn)(void* self, CWidget* widget,
                                           CWorldVec3 world,
                                           unsigned int mask);

//  What the maker hands back: only the CAgent* at +0x54 is reached.
struct SMadeAgentHolder
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//  0x6F333520 - item_placegate.cpp.
struct SItemPlaceGate
{
    void OnPlaced();
    void OnMaskChanged();
};

void CWidget::ClearPlayerMaskBits(unsigned int mask, int)
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F0955C0 constructor is (widgetagentslot_ctor.cpp).
    SWidgetAgentSlot held(0);

    //  The pool the widget-side agents come from: look 'hgw+' up in the
    //  game-wide agile type registry and take that type record's own
    //  allocator, exactly as CAgent::ReleaseSelf does for its own type
    //  (agent_releaseself.cpp) - including the singleton held in a
    //  callee-saved register across Hash().
    AGILE_TYPE_ID id(GetWidgetAgileTypeId());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetWidgetAgileTypeId(), pool);
    //  The maker hands back a *holder*, and the agent this slot wants is
    //  the CAgent* at its +0x54 - the shipped `mov eax,[eax+54h]` between
    //  the two calls, not the holder itself.
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    //  The handle sub-object's world position, copied into the by-value
    //  argument block the reserved twelve bytes are.
    CWorldVec3 world =
        ((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this)
            ->SmartPtrToWorldVec3();

    //  Read once into a local and used for both calls: the shipped code
    //  keeps it in ebp across the position query rather than reloading the
    //  slot each time.
    CAgent* agent = held.m_value;
    ((AgentSlot0x60Fn)(*(void***)agent)[0x60 / 4])(agent, this, world, mask);

    m_playerMaskA &= (unsigned short)~mask;

    ((SItemPlaceGate*)agent)->OnMaskChanged();

    //  The slot's own release, inlined: the same decrement-and-release-if-
    //  last three instructions CAgentPtr::Release has (agent.h), which is
    //  what every one of these smart-pointer instantiations has inlined at
    //  its destructor rather than called.
    CAgent* last = held.m_value;
    if (last != 0)
    {
        last->m_refcount += -1;
        if (last->m_refcount == 0)
            ((void (__thiscall*)(void*))(*(void***)last)[0])(last);
    }
}
