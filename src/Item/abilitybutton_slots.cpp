//============================================================================
//  Four of CAbilityButton's own small vtable overrides: slot 3
//  (Method_0x0C), slots 32/33 (Method_6F052B10/Method_6F052AF0, overriding
//  CAbility's own), slot 135 (DefaultIntFieldQueryAO, overriding CAbility's
//  own), and slot 193 (Method_0x304, overriding CPower's own default).
//  See abilitybutton.h for each address/offset.
//============================================================================
#include "abilitybutton.h"

//----------------------------------------------------------------------------
//  0x6F024210 - slot 3.  `this instance's own icon-like answer` first
//  (Method_0x308), then the fallback pair (Method_0x318, twice - once
//  against the default-icon sentinel, once against the record's own +0x8
//  field) - written with the shipped code's own two separate calls to
//  Method_0x318 rather than one cached local, matching its own two
//  separate `call edx` sites off the one preloaded slot pointer.
//----------------------------------------------------------------------------
int CAbilityButton::Method_0x0C(const struct SAgentMessage* msg)
{
    unsigned int id = msg->m_id;

    if (id == Method_0x308())
    {
        Method_0x330((void*)msg);
        return 1;
    }

    //  The two remaining failure paths (the default-icon sentinel here and
    //  the second Method_0x318 mismatch below) share the shipped code's own
    //  single `return 0` epilogue - written with a `goto` so MSVC merges
    //  them into one block instead of sinking a duplicate epilogue at each
    //  early exit (docs/msvc-vc8-idioms.md, "Shared exits need goto").
    if (Method_0x318() == g_defaultAbilityIconId)
        goto notFound;
    if (id == Method_0x318())
    {
        Method_0x334((void*)msg);
        return 1;
    }

notFound:
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F054840 - slot 33, overriding CAbility::Method_6F052AF0.  Reset this
//  instance's own four button fields to their construction defaults, then
//  tail-call the base version unchanged - the same "reset, then let the
//  base recompute the type record" shape slot 32 below shares.
//----------------------------------------------------------------------------
void CAbilityButton::Method_6F052AF0(void* target)
{
    m_defaultIconId = g_defaultAbilityIconId;

    m_field70 = 0;
    m_field74 = 0;
    m_field78 = 0;
    m_field7C = 0;

    CAbility::Method_6F052AF0(target);
}

//----------------------------------------------------------------------------
//  0x6F054860 - slot 32, overriding CAbility::Method_6F052B10.  Same reset,
//  then tail-call the base version with both incoming arguments unchanged.
//----------------------------------------------------------------------------
void CAbilityButton::Method_6F052B10(void* arg0, void* recordField)
{
    m_defaultIconId = g_defaultAbilityIconId;

    unsigned int zeroA = 0;
    unsigned int zeroB = 0;
    m_field70 = zeroA;
    m_field74 = zeroB;
    m_field78 = zeroA;
    m_field7C = zeroB;

    CAbility::Method_6F052B10(arg0, recordField);
}

//----------------------------------------------------------------------------
//  0x6F0228F0 - slot 135, overriding CAbility::DefaultIntFieldQueryAO.
//  `code == Method_0x308()` as a 0/1 answer.
//----------------------------------------------------------------------------
int CAbilityButton::DefaultIntFieldQueryAO(int code)
{
    return (unsigned int)code == Method_0x308();
}

//----------------------------------------------------------------------------
//  0x6F031490 - slot 193, overriding CPower::Method_0x304.  Try this
//  instance's own icon-like answer (Method_0x308) first; on a miss, its
//  hotkey-like answer (Method_0x318); on a second miss, two more
//  candidates (Method_0x320/Method_0x328).  Any hit answers 0 (matched -
//  nothing further to fill in); no hit falls through to CPower's own
//  sentinel default, 0xDD, exactly the value CPower's own Method_0x304
//  answers unconditionally.
//----------------------------------------------------------------------------
int CAbilityButton::Method_0x304(int arg)
{
    //  Two distinct `return 0` epilogues in the shipped code, not one
    //  shared by all four - each reused by exactly one other exit
    //  (docs/msvc-vc8-idioms.md, "Shared exits need goto"): the first
    //  branch's default-icon miss shares one with the Method_0x320 match;
    //  the second branch's own match falls through to the same block the
    //  Method_0x328 match reaches. A third, single `return 0xDD` epilogue
    //  is shared by both flag checks and the final fallthrough.
    unsigned int r1 = Method_0x308();
    if ((unsigned int)arg == r1)
    {
        unsigned int r2 = Method_0x318();
        if (r2 == g_defaultAbilityIconId)
            goto returnZeroA;
        if (m_flags & 0x80)
            goto returnDD;
        goto returnZeroA;
    }
    goto secondBranch;

    //  Placed here, not at the function's tail, so the Method_0x320 match
    //  below reaches it with a backward jump - the shipped code's own
    //  `loc_6F0314C0` sits right after the first branch, not at the end.
returnZeroA:
    return 0;

secondBranch:
    {
        unsigned int r2 = Method_0x318();
        if ((unsigned int)arg == r2)
        {
            if (!(m_flags & 0x80))
                goto returnDD;
            goto returnZeroB;
        }
    }

    {
        unsigned int r3 = Method_0x320();
        if ((unsigned int)arg == r3)
            goto returnZeroA;
    }

    {
        unsigned int r4 = Method_0x328();
        if ((unsigned int)arg == r4)
            goto returnZeroB;
    }

returnDD:
    return 0xDD;

returnZeroB:
    return 0;
}
