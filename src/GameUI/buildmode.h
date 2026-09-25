//============================================================================
//  CBuildMode and the frame it owns.
//
//  Both class names are the shipped ones, out of the two vtables their
//  constructors stamp - ??_7CBuildMode@@6B@ at 0x6F39C825 and the
//  ??_7CBuildFrame@@6B@ / ??_7CBuildFrame@@6B@_0 pair at 0x6F39D45E - and
//  the shipped allocation tag ".\\CBuildMode.cpp" names the module they
//  live in.  The `_0` twin at +0x0B4 is the two-vtable shape of a CFrame
//  subclass (CLayer at +0, CLayoutFrame at +0xB4), which settles the base
//  the same way CTimeCover's does.
//
//  CBuildMode's own 0x19C-byte allocation for the frame is what fixes
//  CBuildFrame's size: the last thing its constructor builds is the slot at
//  +0x198, and 0x198 + 4 is 0x19C exactly.
//============================================================================
#ifndef BUILDMODE_H
#define BUILDMODE_H

#include "frame.h"
#include "observer.h"
#include "agenttypedslots.h"

//----------------------------------------------------------------------------
//  The type-checked 'abil' slot at CBuildFrame +0x198 - a nineteenth member
//  of Agent/agenttypedslots.h's family, whose Assign is 0x6F0DEB90 and whose
//  required type comes from the tag getter at 0x6F0211D0 (`mov eax,
//  6162696Ch` - 'abil').  Only its constructor is reached here, and it is
//  still a redirect, so it is spelled as a `Construct` member rather than as
//  a C++ constructor: MSVC will not take __declspec(naked) on one, and
//  calling it from the body is also what puts it after the field stores,
//  which is the shipped order (GameUI/selectmodector.cpp says the same).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot
{
    //  0x6F0E7410 - `retn 4`.  Still a redirect.
    void Construct(CAgent* candidate);

    CAgent* m_value;        // +0x00
};

class CBuildFrame : public CFrame
{
public:
    //  0x6F39D420 - GameUI/buildframector.cpp.  `retn 4`.
    CBuildFrame(CFrame* parent);

    char               m_reserved168[0x0C];  // +0x168
    //  +0x174 / +0x178 - two ids left at -1.
    int                m_field174;           // +0x174
    int                m_field178;           // +0x178
    //  +0x17C .. +0x184 - three floats zeroed as one `fldz` run.  Three
    //  named fields and not an array, because they go in the constructor's
    //  member-initialiser list (see buildframector.cpp) and an array member
    //  cannot.
    float              m_float17C;           // +0x17C
    float              m_float180;           // +0x180
    float              m_float184;           // +0x184
    int                m_field188;           // +0x188
    int                m_field18C;           // +0x18C
    int                m_field190;           // +0x190
    //  +0x194 - the unit being placed, held in a '+w3u' checked slot
    //  (Agent/agenttypedslots.h).
    SCheckedUnitSlot   m_unit;               // +0x194
    //  +0x198 - the ability that started the placement, in an 'abil' one.
    SCheckedAbilitySlot m_ability;           // +0x198
};

class CBuildMode : public CObserver
{
public:
    //  0x6F39C7E0 - GameUI/buildmodector.cpp.  `retn 4`.
    CBuildMode(CFrame* parent);

    CBuildFrame* m_pFrame;      // +0x0C
    int          m_field10;     // +0x10
};

#endif
