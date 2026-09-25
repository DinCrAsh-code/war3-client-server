//============================================================================
//  0x6F0418A0 - is this agent's own handle still live?
//
//  The same three-step test CWidget::AdjustWidgetPathRefs (widgetpathref.cpp)
//  opens with, reduced to a predicate: resolve the agent's own handle/type
//  pair at +0x0C/+0x10 through the global handle table, check the object
//  behind it still carries the '+agl' tag, and check it is not already busy.
//  The tag test and the null test share one branch in the shipped code -
//  `setnz`/`sub 1`/`and` on the pointer, which is a checked cast written as
//  a conditional expression and not two `if`s (docs/msvc-vc8-idioms.md,
//  "`setne` / `sub 1` / `and` on a pointer is a checked cast").
//
//  Batch A reached it as a redirect from the pause sweep
//  (gameuihighlightsweep.cpp) and from the screen-projection pass
//  (gameuiagentscreenpos.cpp); both call it before dereferencing an agent
//  they only hold a handle to.
//============================================================================
#include "game.h"

namespace
{
    //  The object LookupHandle hands back for a CAgent handle.  Only the two
    //  fields this function tests are named - the same two widgetpathref.cpp
    //  names on the same object.
    struct SAgentHandleObject
    {
        char         m_reserved00[0x0C];
        unsigned int m_typeTag;      // +0x0C, must equal kAgileHandleTag
        char         m_reserved10[0x10];
        unsigned int m_busy;         // +0x20, must be zero
    };

    //  The agent's own +0x0C handle and +0x10 type tag, in the handle-pair
    //  order LookupHandle is called with everywhere in this binary.
    struct SAgentHandleFields
    {
        char         m_reserved00[0x0C];
        unsigned int m_handle;       // +0x0C
        int          m_handleType;   // +0x10
    };

    //  '+agl' MSB first - see widgetpathref.cpp's kAgentTagPathable and
    //  agentregistry.h's kAgileHandleTag, which are this same constant.
    const unsigned int kAgileHandleTag = 0x2B61676C;
}

int __fastcall IsAgentHandleLive(const void* agent)
{
    const SAgentHandleFields* self = (const SAgentHandleFields*)agent;

    SAgentHandleObject* object = (SAgentHandleObject*)
        LookupHandle(self->m_handle, self->m_handleType);

    //  All three refusals share one `xor eax,eax` / `retn` and the success
    //  is the fall-through, which is what a guarded success path gives and
    //  a chain of early `return 0`s does not.
    if (object != 0)
    {
        SAgentHandleObject* live =
            (object->m_typeTag == kAgileHandleTag) ? object : 0;

        if (live != 0 && live->m_busy == 0)
            return 1;
    }

    return 0;
}
