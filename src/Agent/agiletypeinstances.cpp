//============================================================================
//  0x6F46D0B0 - EnumerateTypeInstances: the agile type registry's own
//  per-type instance walk, and the depth-first recursion over child types
//  that follows it.
//
//  A type that nobody has ever asked for an instance object of has no
//  +0x6C at all, and then there is nothing here to do - not even the child
//  walk, which is why that test is the function's first branch and its
//  early return skips everything.
//
//  Each node of the instance list is refused twice over, and both tests are
//  in the shipped stream: once on the node itself (`m_retired`) and once on
//  the object the node's agent resolves to through the handle table, which
//  has to be an agile registration ('+agl') and has to be live as well.
//  The '+agl' check is the checked-cast conditional expression
//  widgetpathref.cpp records - `setnz` / `sub 1` / `and`, not a second
//  `if` - written with the zero hoisted above the null test, the shape
//  floatpropmodifierbind.cpp already has.
//
//  **The visitor's answer is accumulated, not branched on.**  A zero from
//  it means "stop", and the shipped code folds that into a running flag
//  with `neg`/`sbb`/`add 1`/`or` and tests the flag at the bottom of the
//  loop - which is what an `int` accumulated with `|=` and a loop
//  condition of `node != 0 && stop == 0` gives.  It is also why the flag
//  is tested a second time after the loop, where it can never be set: the
//  compiler cannot know which of the two exits was taken.
//
//  The child list is a TSExplicitList walked from the tail backwards
//  through Prev(), with the raw link masked to zero when it is the
//  terminator - the same "test the sign directly" idiom storm.h documents
//  for every one of these walks.  Each child's type record is looked up
//  once and cached on the child, through the same AGILE_TYPE_ID / Hash /
//  Ptr chain every other reader of the registry uses.
//
//  Own translation unit: EnumerateUnitsOfType calls it for real, and it
//  calls itself.
//============================================================================
#include "agiletypeinstances.h"
#include "agent.h"

//  '+agl' MSB first - see widgetpathref.cpp.
static const unsigned int kAgentTagAgile = 0x2B61676C;

void __fastcall EnumerateTypeInstances(AGILE_TYPE_DATA* type,
                                       UnitQueryVisitFn fn, void* ctx,
                                       CGameData* game, int recurse)
{
    SAgileTypeInstances* host = (SAgileTypeInstances*)type->m_field6C;
    if (host == 0)
        return;

    int stop = 0;

    SAgileAgent* node = host->m_head;
    while (node != 0)
    {
        if (node->m_retired == 0)
        {
            CAgent* agent = node->m_agent;

            CHandleObject* resolved = LookupHandle(agent->m_handle,
                                                   (int)agent->m_typeTag);
            SAgileAgent* live = 0;
            if (resolved != 0)
                live = (resolved->m_kindTag == kAgentTagAgile)
                           ? (SAgileAgent*)resolved : 0;

            if (live != 0 && live->m_retired == 0)
                stop |= (fn(agent, ctx) == 0);
        }

        //  The stop test at the *bottom* and the node test at the top, not
        //  one `&&` condition: written as `while (node != 0 && stop == 0)`
        //  MSVC rotates the pair the other way round - the stop test moves
        //  to the head of the body, and with it the `|=` above collapses to
        //  a plain assignment because the flag is then known zero there
        //  (measured, 56 of 95 against 76).
        if (stop != 0)
            break;

        node = node->m_nextOfType;
    }

    if (stop != 0 || recurse == 0)
        return;

    TSExplicitList<SAgileChildType>& children =
        ((SAgileTypeChildren*)type)->m_children;

    //  Walked as raw link values rather than through Tail()/Prev(), for the
    //  reason TSExplicitList<T>::UnlinkAll already records: the shipped code
    //  tests the sign of the field directly (`jle`) and never goes through
    //  Link()'s own null test, which a Prev() call would put in the loop.
    int child = children.TailLink();
    if (child <= 0)
        child = 0;

    while (child > 0)
    {
        //  The successor is formed before the body and held across the
        //  recursion, which is what the shipped callee-saved register does.
        int prev = ((TSLink<SAgileChildType>*)
                        (children.m_linkoffset + child))->m_prevlink;
        int next = prev > 0 ? prev : 0;

        SAgileChildType* record = (SAgileChildType*)child;
        if (record->m_cached == 0)
        {
            AGILE_TYPE_ID id(record->m_typeId);
            unsigned int hash = id.Hash();
            record->m_cached = game->m_types.Ptr(hash, id);
        }

        EnumerateTypeInstances(record->m_cached, fn, ctx, game, recurse);

        child = next;
    }
}
