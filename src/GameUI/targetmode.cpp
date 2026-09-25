//============================================================================
//  0x6F2F8280 - CTargetMode::CTargetMode.
//
//  The class name is the shipped one: the constructor stamps
//  `??_7CTargetMode@@6B@` (0x6F935DEC) into the object it is building.
//
//  Built on a CObserver base - the shipped code calls CObserver's own
//  constructor on `this` at offset zero and then overwrites the vtable
//  pointer it just wrote, which is the standard derived-class shape
//  observer.cpp already records for CAgent.  The vtable itself is modelled
//  as a data symbol rather than as eight `virtual`s, for the reason
//  floatboundarylistener.cpp gives: declaring the slots would make this
//  build emit its own short table and then fail to link every body in it.
//
//  The six -1 sentinels at +0x18..+0x2C come out of one register and the
//  two zeroes out of another, which is the same "one register serves as the
//  zero and is then turned into the -1" shape the request blocks in this
//  target already use (positionrecordbuild.cpp).
//
//  What it cannot reproduce is the frame: the checked slot constructed at
//  +0x30 can raise, so the shipped body carries the
//  __except_handler4-shaped, cookie-XORed frame that unwinds the CObserver
//  base, and docs/msvc-vc8-idioms.md records that frame as unreachable at
//  this toolchain's fixed /GS- /EHs-c-.
//
//  Own translation unit: two real calls out of it.
//============================================================================
#include "observer.h"
#include "agenttypedslots.h"

//  ??_7CTargetMode@@6B@ - the shipped vtable, by address.
extern void* const g_vftCTargetMode;

//  dword_6F92ED9C - already in funcmap.DATA under this name.
extern const unsigned int g_defaultAbilityIconId;

class CTargetMode : public CObserver
{
public:
    CTargetMode();

    int               m_field0C;    // +0x0C
    unsigned int      m_iconId;     // +0x10
    int               m_field14;    // +0x14
    int               m_slot18;     // +0x18   six -1 sentinels; nothing in
    int               m_slot1C;     // +0x1C   this tree reads any of them
    int               m_slot20;     // +0x20   back, so none gets a name
    int               m_slot24;     // +0x24   beyond its offset
    int               m_slot28;     // +0x28
    int               m_slot2C;     // +0x2C
    //  +0x30 - a checked '+w3s' CSelectable slot, constructed empty.  This
    //  is what puts the __except_handler4-shaped frame round the body: the
    //  slot has to be released if anything after it raises.  0x6F2C9670 is
    //  that constructor, reconstructed by the JASS_PauseGame target, which
    //  reached it from CGameUI's own side (agenttypedslots.h).
    SCheckedSelectableSlot m_target;   // +0x30
};

CTargetMode::CTargetMode()
    : m_target(0)
{
    m_field0C = 0;
    *(const void**)this = &g_vftCTargetMode;
    m_iconId = g_defaultAbilityIconId;
    m_field14 = 0;
    m_slot18 = -1;
    m_slot1C = -1;
    m_slot20 = -1;
    m_slot24 = -1;
    m_slot28 = -1;
    m_slot2C = -1;
}
