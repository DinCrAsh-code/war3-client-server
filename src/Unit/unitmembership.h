//============================================================================
//  0x6F421E20 / 0x6F4219B0 - "does this membership set hold that unit".
//
//  The set is reached as a player record's +0x34 (unit_playertable.cpp,
//  unit_playermasksearch.cpp) and it forwards to a list that lives at its
//  own +0x1E8 - the shipped `add ecx, 1E8h` / `jmp`, which is a tail call
//  on a sub-object and not an inheritance adjustor thunk (there is no
//  second base).
//============================================================================
#ifndef UNITMEMBERSHIP_H
#define UNITMEMBERSHIP_H

//  One entry of the intrusive list.  The walk ends on a *signed* `<= 0`,
//  not on null - the same terminator convention TSExplicitList uses
//  (storm.h), so the last link is a complemented address and not zero.
struct SUnitMembershipNode
{
    char                 m_reserved00[4];
    SUnitMembershipNode* m_pNext;      // +0x04
    void*                m_pUnit;      // +0x08
};

struct SUnitMembershipList
{
    //  0x6F4219B0 - `retn 4`.
    int __thiscall Contains(void* unit);

    char                 m_reserved00[0x0C];
    SUnitMembershipNode* m_pHead;      // +0x0C
};

struct SUnitMembershipSet
{
    //  0x6F421E20 - `retn 4`, and its own body is two instructions.
    int __thiscall Contains(void* unit);

    char                m_reserved00[0x1E8];
    SUnitMembershipList m_list;        // +0x1E8
};

#endif
