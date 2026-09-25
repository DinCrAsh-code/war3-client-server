//============================================================================
//  0x6F4A4DF0 / 0x6F4A4E40 - two near-identical twins that report a
//  subject's `+0x14`/`+0x18` pair (or a `{-1,-1}` sentinel when the subject
//  pointer itself is null) to a fixed global host (`g_pTimeSync`,
//  Game/game.h, `dword_6FAB73D8`) via `sub_6F4815A0`, with a constant code
//  (1 for the first, 2 for the second) that is the only real difference
//  between the two bodies - same shape as `CAgentRelation::PostToEndpointA/
//  PostToEndpointB` (agentrelation.h), which differ only in a context field
//  the same way. Both bodies are transcribed separately, not factored
//  through a shared helper, because the shipped binary carries two full,
//  separately-compiled copies rather than one out-of-line function called
//  twice.
//
//  `this` (`ecx`) is read with an explicit null check before either field is
//  touched, so the subject is not necessarily this agent itself - nothing in
//  this call tree pins down what it actually is (called from `NIpse::
//  CRlAgent::Slot4`/`::Slot17`, Pathfinding/crlagent.h - reached through,
//  but not owned by, CAgentBaseAbs's own vtable slots 4/17, per the dump's
//  own call graph). `subjectId` is a genuine
//  *stack* argument (`retn 4`), not a second register: a free function
//  cannot be spelled `__thiscall` in this compiler (dataallocator.h's own
//  note on the same restriction, also cited by Misc/nullsub.cpp's
//  `SVtableNoOpArg`), so this is written as an ordinary non-virtual method
//  of a plain struct instead, called through a pointer that may be null -
//  legal here because neither body dereferences `this` before the explicit
//  check.
//
//  `sub_6F4815A0` is out of this session's batch, owned by a parallel
//  session on the same dump - forward-declared here only so this
//  translation unit compiles under `verify.py`, which never links.
//============================================================================
#include "game.h"

struct SGlobalAgentNotifyHost
{
    //  (code, context, subjectId) - real declared order established from
    //  the call site's own push order (the last-pushed stack value is
    //  always the first parameter): `code` is a constant (1 or 2 here),
    //  `context` is the address of the local 3-dword block built below,
    //  `subjectId` is this function's own stack argument, passed through
    //  unexamined.
    void Notify_6F4815A0(unsigned int code, const void* context,
                        unsigned int subjectId);
};

struct SAgentBaseAbsNotifyCtx
{
    unsigned int m_a;
    unsigned int m_b;
    unsigned int m_reserved08;
};

struct SAgentBaseAbsNotifySubject
{
    //  0x6F4A4DF0
    void NotifyField1(unsigned int subjectId);
    //  0x6F4A4E40
    void NotifyField2(unsigned int subjectId);

    char         m_reserved00[0x14];
    unsigned int m_field14;   // +0x14
    unsigned int m_field18;   // +0x18
};

void SAgentBaseAbsNotifySubject::NotifyField1(unsigned int subjectId)
{
    SAgentBaseAbsNotifyCtx ctx;
    unsigned int a;
    if (this != 0)
    {
        a = m_field14;
        ctx.m_b = m_field18;
    }
    else
    {
        a = (unsigned int)-1;
        ctx.m_b = (unsigned int)-1;
    }
    ctx.m_a = a;
    ctx.m_reserved08 = 0;

    ((SGlobalAgentNotifyHost*)g_pTimeSync)->Notify_6F4815A0(1, &ctx, subjectId);
}

void SAgentBaseAbsNotifySubject::NotifyField2(unsigned int subjectId)
{
    SAgentBaseAbsNotifyCtx ctx;
    unsigned int a;
    if (this != 0)
    {
        a = m_field14;
        ctx.m_b = m_field18;
    }
    else
    {
        a = (unsigned int)-1;
        ctx.m_b = (unsigned int)-1;
    }
    ctx.m_a = a;
    ctx.m_reserved08 = 0;

    ((SGlobalAgentNotifyHost*)g_pTimeSync)->Notify_6F4815A0(2, &ctx, subjectId);
}
