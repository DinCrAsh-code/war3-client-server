//============================================================================
//  0x6F39D420 - CBuildFrame::CBuildFrame.
//
//  A CFrame over the whole of its parent: the base built with the parent and
//  two zeroes, both vtables stamped, eleven fields cleared, the two checked
//  slots constructed, the frame anchored to fill the parent, the parent's
//  own controller adopted and two flag words or'd with 2.
//
//  Two things worth reading carefully:
//
//    * the anchor argument is the *null-checked* +0xB4 conversion - the
//      shipped code computes `parent + 0xB4` only when `parent` is non-null
//      (`cmp edi, ebx` / `lea ebx, [edi+0B4h]` over an already-zeroed ebx),
//      which is what a CFrame* -> CLayoutFrame* conversion of a possibly
//      null pointer compiles to.  GameUI/timeofdayindicatorctor.cpp records
//      the same pair;
//    * the controller adopt is 0x6F606850, which this repo already carries
//      under the JASS_PauseGame slice's name, CWorldFrameWar3::
//      NotifyControllerChanged (Frame/framecursorcontroller.cpp).  It is
//      really CFrame's own - its +0x140 is CFrame's +0x140, as that file's
//      own note says - and it is reached here through a CBuildFrame.  The
//      cast is what keeps one shipped address carrying exactly one C++
//      name; renaming it would be a new hook key for another slice's
//      function.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  a constructor with a base and two sub-objects to unwind - the `push -1`
//  and scope table, the cookie prologue, the four `mov [esp+..], <state>`
//  stores, the three appended unwind funclets and the `jmp
//  __CxxFrameHandler3` tail - none of which this repo's fixed /GS- /EHs-c-
//  emits.
//
//  Own translation unit: its caller (CBuildMode's constructor) calls it for
//  real.
//============================================================================
#include "buildmode.h"
#include "gameui.h"
#include "storm.h"          // placement new

//  0x6F606890 - Misc/trivialaccessors_06.cpp: the frame's own +0x140, i.e.
//  the controller the parent is already driven by.
int __fastcall GetField0x140_6F606890(const void* self);

//  The eleven fields the shipped constructor sets before the checked slots
//  are *member initialisers*, in declaration order: MSVC emits a member
//  initialiser list in that order, which is what puts the eight stores ahead
//  of the '+w3u' slot's constructor with no placement-new null test in front
//  of it.  The 'abil' slot follows in the body because it is still a redirect
//  and cannot be spelled as a constructor at all.
CBuildFrame::CBuildFrame(CFrame* parent)
    : CFrame((int)parent, 0, 0),
      m_field174(-1),
      m_field178(-1),
      m_float17C(0.0f),
      m_float180(0.0f),
      m_float184(0.0f),
      m_field188(0),
      m_field18C(0),
      m_field190(0),
      m_unit(0)
{
    m_ability.Construct(0);

    SetAllPoints(parent, 1);

    ((CWorldFrameWar3*)this)->NotifyControllerChanged(
        (void*)GetField0x140_6F606890(parent));

    m_flagMask |= 2;
    m_flags |= 2;
}
