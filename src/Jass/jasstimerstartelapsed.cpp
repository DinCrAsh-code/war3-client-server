//============================================================================
//  SJassTimer's own Start and GetElapsed (jassnatives.h), both previously
//  naked `mov eax,<addr>/jmp eax` redirects in jassnatives_thunks.cpp.
//  Unrelated addresses (0x6F3F6890 and 0x6F3ECD30 are ~0x8B60 apart, in
//  different shipped modules) kept in one TU only because neither calls the
//  other and both are members of the one class this batch's own header
//  already carries - the same "combine unrelated same-class members" call
//  jasseventcontext.cpp already makes for GetCurrentScriptEvent/Data.
//
//  Both reach `this+0x24`, an embedded polymorphic sub-object (its own
//  vtable pointer sits at that offset, not a pointer to one - the receiver
//  of a virtual call is the embedded object itself) this batch calls
//  CEventAnnouncer: GetElapsed asks its virtual slot 0x18 for an override
//  value, Start hands it to the deep SEH-wrapped broadcast helper
//  (0x6F477A80) once construction finishes.  Nothing else in this call
//  tree resolves what the sub-object's other slots do.
//============================================================================
#include "jassnatives.h"
#include "agiletype.h"
#include "agent.h"
#include "game.h"
#include "widgetagentquery.h"
#include "eventannouncer.h"   // CEventAnnouncer - shared with Agent/agenttypetag.cpp

namespace
{

#define TIMER_THUNK(addr)        \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  0x6F3B54D0 - a plain refcounted-assign, no type check: release whatever
//  the slot already held, addref the new candidate (if it and its old
//  value differ), store it.  Not one of agenttypedslots.h's checked slots
//  - those all validate an agile type id first; this one just swaps.
//  Thunked (deep, and no dump of its own callee sub_6F471910 in this
//  batch's own scope).
struct SAgentRefSlot
{
    CAgent* m_value;

    SAgentRefSlot* Assign(CAgent* candidate);
};

__declspec(naked) SAgentRefSlot* SAgentRefSlot::Assign(CAgent*)
{
    __asm { mov eax, 0x6F3B54D0 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F3B1480 - the timer's own required agile-type FourCC leaf, called
//  twice (once for the registry lookup, once for the query block) rather
//  than hoisted, the same shape cunit_agent6_queryfields.cpp's whole family
//  already documents for the identical reason (kept the optimiser from
//  folding the two calls into one).
//----------------------------------------------------------------------------
static unsigned int GetJassTimerPoolTypeTag()
{
    return 0x68646C72;
}

//----------------------------------------------------------------------------
//  0x6F3B14D0 - a plain +0x24 field setter on whatever object it is handed:
//  no dispatch.  `retn 4`, `this` in ecx.  Called here against the pool
//  slot's own held object (see Start below), not against the timer itself.
//----------------------------------------------------------------------------
namespace
{
    struct SFieldOwner24
    {
        char m_reserved00[0x24];
        void* m_field24;

        void SetField24(void* value) { m_field24 = value; }
    };
}

//----------------------------------------------------------------------------
//  0x6F3F6890 - `TimerStart`'s own object-side Start.  Lazily builds the
//  timer's agent-query pool (the same "hash the fixed type id, look it up
//  in g_pGameData->m_types, build+submit an SWidgetAgentQuery against its
//  pool" shape cunit_agent6_queryfields.cpp's whole family already
//  documents) the first time it is armed, stores the callback on the pool
//  object's own +0x24 field, arms the `this+0x44` embedded sub-object
//  (virtual slot 0) with the real/periodic flag, then posts the "timer
//  started" event through the `this+0x24` announcer.
//----------------------------------------------------------------------------
void SJassTimer::Start(const int* seconds, int periodic, int callback)
{
    SAgentRefSlot* pool = (SAgentRefSlot*)((char*)this + 0x40);

    if (pool->m_value == 0)
    {
        AGILE_TYPE_ID id(GetJassTimerPoolTypeTag());
        AGILE_TYPE_DATA* type = g_pGameData->m_types.Ptr(id.Hash(), id);
        void* allocator = type->m_allocator;

        SWidgetAgentQuery query;
        InitWidgetAgentQuery(&query, GetJassTimerPoolTypeTag(), allocator);

        struct SMadeAgentHolder { char m_reserved00[0x54]; CAgent* m_pAgent; };
        pool->Assign(((SMadeAgentHolder*)
                      SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    }

    //  0x6F3B14D0 - store the callback on the pool's own held object.
    ((SFieldOwner24*)pool->m_value)->SetField24((void*)(long)callback);

    //  `this+0x44`'s own embedded sub-object, virtual slot 0 - arm the
    //  timer for real, passing the seconds pointer and a fixed 1.
    typedef void (__thiscall *ArmFn)(void*, const int*, int);
    void* armReceiver = (char*)this + 0x44;
    ArmFn arm = (ArmFn)(*(void***)armReceiver)[0];
    arm(armReceiver, seconds, 1);

    //  Post the "timer started" event through the +0x24 announcer.
    ((CEventAnnouncer*)((char*)this + 0x24))->PostEvent(
        seconds, 0x80204, this, periodic, 0);
}

//----------------------------------------------------------------------------
//  0x6F3ECD30 - `TimerGetElapsed`'s own object-side GetElapsed: an override
//  through the +0x24 announcer's virtual slot 0x18 when one is armed and
//  live, else the plain cached value at +0x50.  Always returns `out`
//  itself, not the callee's own result - the call is for its side effect
//  only.
//----------------------------------------------------------------------------
int* SJassTimer::GetElapsed(int* out)
{
    void* override = *(void**)((char*)this + 0x30);

    if (override && !(*((unsigned char*)override + 0x12) & 1))
        ((CEventAnnouncer*)((char*)this + 0x24))->QueryOverride(out);
    else
        *out = *(int*)((char*)this + 0x50);

    return out;
}
