//============================================================================
//  DELIBERATE BEHAVIOUR CHANGES, for live debugging only.
//
//  ==========================================================================
//  THIS REPO IS A MATCHING DECOMPILATION.  Every switch in this file makes
//  the reconstruction stop matching the shipped binary on purpose, so a
//  target's BEHAVIOUR verdicts are lies while one is on.  Turn them all off
//  before doing any matching work.
//
//  They are self-announcing: each one changes a function that is currently
//  EXACT, so `python3 tools/verify.py` prints REGRESSION for it on every run
//  while the switch is on.  A REGRESSION line you did not expect is the
//  alarm.
//  ==========================================================================
//
//  Why a header and not a /D on the command line: the verifier gate builds on
//  the operator's machine from the pushed commit, with that machine's own
//  flags, so a command-line define never reaches it.  The switch has to be in
//  the source to travel.
//============================================================================
#ifndef MDP_DEMO_H
#define MDP_DEMO_H

//----------------------------------------------------------------------------
//  Make every CWidget-shaped object that is NOT a unit report player 0 as its
//  owner, and let it through the visibility gate.
//
//  CWidget::GetOwningPlayerIndex (vtable[0xEC], 0x6F266A40) is two
//  instructions - `or eax,-1` / `retn` - so a widget, a selectable, a
//  destructable, an item and a CCaptainAI all answer "no owner".  CUnit is
//  the one class of the six that overrides the slot (confirmed: its vtable is
//  absent from the five that carry this address), so a real unit is
//  unaffected by this switch and keeps its own owner.
//
//  Turning this on makes those five answer player 0 instead.  Three sites
//  move together:
//
//    * GetOwningPlayerIndex           returns 0 rather than -1
//    * IsVisibilityMaskLive           always says the mask is live
//    * CreateSpriteFromDescriptor     sets player 0's bit in both of
//                                     CWidget's 16-bit player masks at
//                                     creation time
//
//  What that is *not*: a guarantee the UI will let you drag an item around.
//  Ownership and visibility are two of the gates the selection and command
//  path consults; this switch moves the two this repo actually owns.  If
//  items become selectable but not commandable, the next gate to look at is
//  CItem's own vtable[0x80] family, which is reconstructed
//  (CItem::Method_0x80, item.cpp) but not part of this switch.
//----------------------------------------------------------------------------
//#define MDP_DEMO_PLAYER0_OWNS_WIDGETS 1

#endif
