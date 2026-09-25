//============================================================================
//  Probably_W3GS_ActionHandler_MaybeServerVersion - 0x6F682300 (root of this
//  dump). The user's own note guesses this is a client/server sibling of the
//  other W3GS opcode dispatcher (Probably_W3GS_ActionHandler, 0x6F672340,
//  w3gsactionhandler.cpp) that "just handles different opcodes" and "doesn't
//  share callers" with it - both hold up under inspection: this switch's own
//  opcode range (1-80 / 0x01-0x50) and case count (81) are completely
//  different from the other dispatcher's (0x1D-0x4C / 29-76, 48 cases), the
//  two share none of their 37+34 handler addresses, and nothing in either
//  call tree reaches the other.
//
//  CORRECTED AGAIN (a later session, this time from the caller chain, not
//  the packet content): "MaybeServerVersion" in this file's own name is
//  backwards. Both direct install sites for this dispatcher (sub_6F6835C0,
//  sub_6F683800 - a 2-entry function-pointer table at 0x6F9716FC/
//  0x6F971700) push the debug-assert filename ".\NetClient.cpp" three times
//  each - this dispatcher is client-side code, confirmed the same way the
//  sibling dispatcher's own install chain was confirmed to be NetRouter.cpp
//  (docs/targets/Probably_W3GS_ActionHandler.md's "Client vs server,
//  corrected" section). It does NOT dispatch PacketId.GameAction (0x26) or
//  PacketId.Tock (0x27) - both fall to the default case - so despite being
//  client-side, this is still not where a client receives or unpacks a
//  relayed action bundle; see docs/targets/Probably_W3GS_
//  ActionHandler_MaybeServerVersion.md's own top section for the full
//  trace and what remains open.
//
//  CORRECTED (per the user - a prior identification here was wrong): this
//  switch's opcode range happening to overlap the WC3.Protocol `GameActionId`
//  enum's own numbering (0x01-0x75) was read as evidence this dispatches
//  individual in-game action verbs. It is not - this dispatcher switches on
//  the same kind of value the *other* dispatcher does (a top-level W3GS
//  *packet* opcode), the same way `Probably_W3GS_ActionHandler`'s own case
//  38/`0x26` reaches `GameAction_0x26_PacketHandler` (gameaction0x26_handler.cpp)
//  by packet type, not by an individual queued action's verb. Nothing in
//  either dump establishes what real W3GS packet type any of this
//  dispatcher's 81 opcodes correspond to; the `GameActionId` positional
//  match was coincidental. Handlers below are named by address
//  (`W3GSAction2_<addr>`), not by any assumed `GameActionId` semantics -
//  see w3gsaction2_6f676da0.cpp's own header for the specific case (opcode
//  0x13) this correction applies to.
//
//  Unlike the other dispatcher, this one has no self/vtable guard at all -
//  `self = *arg0` is read and forwarded unconditionally - and it dispatches
//  through *two* levels of table indirection: the raw opcode (biased by -1,
//  range-checked against 0x50) is looked up in a 6F682788-based byte table
//  to produce a second, dense index that actually drives the jump table.
//  This used to be modelled here as an explicit, hand-written second lookup
//  with an `extern`, contents-unknown table - wrong on two counts, both
//  fixed once the user pulled the real bytes at 0x6F682788 from a debugger:
//  the IDA "case N" comments this file's switch already used as its case
//  labels turn out to be the *raw opcode value* (N), not the table's own
//  output (confirmed against all 81 real table entries: opcode 8, undefined
//  in the WC3.Protocol `GameActionId` enum the user supplied, still gets a
//  real, non-default table entry, and opcode 0x13/19 = `W3GSAction2_6F676DA0`
//  resolves cleanly to 0x11 = a real handler, not the default sentinel
//  0x25) - and writing a second explicit table lookup on top of a switch
//  whose case labels are already dense (1-81, gaps filled with `return 1`)
//  just made this build's own compiler insert *its own* second compaction
//  table on top of the hand-written one, which is why the previous version
//  scored two extra instructions worse than it needed to. Switching
//  directly on the raw opcode below lets the compiler build the exact same
//  two-level shape (bias, byte-table lookup, jump) itself, the same way it
//  does for any sufficiently sparse switch - see docs/msvc-vc8-idioms.md,
//  "A jump-table switch's case-body layout order is not source-addressable".
//
//  Handler calling convention: `retn 0xC` (3 stack dwords), confirmed by
//  the trivial handlers themselves and the shared body shape several of the
//  larger ones share (`mov eax,[esp+arg_8] / push .../ mov eax,[esp+arg_4] /
//  push eax / mov eax,[esp+arg_0] / push eax / call ...` - the same
//  ascending arg_0/arg_4/arg_8 layout this dispatcher forwards). None of the
//  37 handlers are reconstructed this session except the two whose entire
//  body is a single constant return (w3gsactionhandler2_stubs.cpp).
//
//  All 37 real calls and both jump/default paths verified present with the
//  right target and the right forwarded arguments (`diff` on just the call
//  targets, both sides sorted, is empty) - but the raw instruction score is
//  low (46/380) for a reason with no source-level fix: a switch lowered
//  through a jump table lays its case bodies out in memory in whatever order
//  the compiler's own case-value/hash decisions produce, and this build's
//  layout order for the 37 near-identical case bodies does not match the
//  target's, even though the *set* of calls and each one's own instructions
//  are the same. See the BEHAVIOUR entry in tools/funcmap.py for the full
//  reasoning (same "not source-addressable" category as the other
//  dispatcher's register-allocation difference).
//============================================================================

int __stdcall W3GSAction2_6F676DA0(void*, unsigned int, unsigned int);   // sub_6F676DA0, opcode 0x13
int __stdcall W3GSAction2_6F67AA80(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67AB30(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67AC60(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67AD90(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67AE70(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67AF30(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67AFF0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67B370(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67B640(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67B660(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67B710(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67B830(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67B8C0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67BB60(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67BB80(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67CE30(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67D040(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67D270(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67E8B0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67E8D0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67EC10(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67EE10(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67EF90(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67F0E0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67F930(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67FC60(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F67FD30(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F680600(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F680620(void*, unsigned int, unsigned int);
//  Four-wide, unlike every other handler here - see w3gsaction_thunks.cpp.
int __stdcall W3GSAction2_6F680740(void*, unsigned int, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F680870(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F680900(void*, unsigned int, unsigned int);
//  Four-wide, unlike every other handler here - see w3gsaction_thunks.cpp.
int __stdcall W3GSAction2_6F680F40(void*, unsigned int, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F6817D0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F6737A0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction2_6F6737B0(void*, unsigned int, unsigned int);

//  Parameter order established empirically (matched target diff): the raw
//  opcode's own dword slot is the *first* stdcall parameter and is reused
//  whole as the "Size"-shaped value every handler receives, `Src` is the
//  second, and the self pointer-to-pointer is the third - two trailing
//  dwords are never read. Case labels below are the real, debugger-confirmed
//  raw opcode values (see the header comment) - each is a WC3.Protocol
//  GameActionId.
int __stdcall Probably_W3GS_ActionHandler_MaybeServerVersion(
    unsigned int opcodeDword, unsigned int Src, void** arg0,
    unsigned int /*unused*/, unsigned int trailing)
{
    void* self = *arg0;

    switch ((unsigned char)opcodeDword)
    {
    case 1: return W3GSAction2_6F67AA80(self, Src, opcodeDword);   // PauseGame
    case 2: return W3GSAction2_6F67CE30(self, Src, opcodeDword);   // ResumeGame
    case 3: return W3GSAction2_6F6737A0(self, Src, opcodeDword);   // SetGameSpeed
    case 4: return W3GSAction2_6F67D040(self, Src, opcodeDword);   // IncreaseGameSpeed
    case 5: return W3GSAction2_6F67AB30(self, Src, opcodeDword);   // DecreaseGameSpeed
    case 6: return W3GSAction2_6F6817D0(self, Src, opcodeDword);   // SaveGameStarted
    case 7: return W3GSAction2_6F680600(self, Src, opcodeDword);   // SaveGameFinished
    case 8: return W3GSAction2_6F67AC60(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied, but a real handler here
    case 9: return W3GSAction2_6F67AD90(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied, but a real handler here
    case 10: return W3GSAction2_6F67AE70(self, Src, opcodeDword);  // undefined in the GameActionId enum supplied, but a real handler here
    case 11: return W3GSAction2_6F680620(self, Src, opcodeDword);  // undefined in the GameActionId enum supplied, but a real handler here
    case 12: return W3GSAction2_6F67E8D0(self, Src, opcodeDword);  // undefined in the GameActionId enum supplied, but a real handler here
    case 13: return W3GSAction2_6F67AF30(self, Src, opcodeDword);  // undefined in the GameActionId enum supplied, but a real handler here
    case 14: return W3GSAction2_6F680F40(self, Src, opcodeDword, trailing);  // undefined in the GameActionId enum supplied, but a real handler here
    case 15: return W3GSAction2_6F67B640(self, Src, opcodeDword);  // undefined in the GameActionId enum supplied, but a real handler here
    case 16: return W3GSAction2_6F67AFF0(self, Src, opcodeDword);  // SelfOrder
    case 17: return W3GSAction2_6F67D270(self, Src, opcodeDword);  // PointOrder
    case 18: return 1;                                             // ObjectOrder - no handler (default sentinel in the real table)
    case 19: return W3GSAction2_6F676DA0(self, Src, opcodeDword);        // W3GSAction2_6F676DA0
    case 20: return W3GSAction2_6F680740(self, Src, opcodeDword, trailing);  // FogObjectOrder
    case 21: return 1;                                             // "_unseen0x15" per the enum - no handler here either
    case 22: return W3GSAction2_6F67B370(self, Src, opcodeDword);   // ChangeSelection
    case 23: return W3GSAction2_6F67B660(self, Src, opcodeDword);   // AssignGroupHotkey
    case 24: return W3GSAction2_6F67B710(self, Src, opcodeDword);   // SelectGroupHotkey
    case 25: return W3GSAction2_6F680870(self, Src, opcodeDword);   // SelectSubGroup
    case 26: return W3GSAction2_6F680900(self, Src, opcodeDword);   // PreSubGroupSelection
    case 27: return W3GSAction2_6F6737B0(self, Src, opcodeDword);   // TriggerSelectionEvent
    case 28: return W3GSAction2_6F67B830(self, Src, opcodeDword);   // SelectGroundItem
    case 29: return 1;                                             // CancelHeroRevive per the enum, but no handler here (default sentinel in the real table)
    case 30: return 1;
    case 31: return 1;
    case 32: return 1;
    case 33: return 1;
    case 34: return 1;
    case 35: return 1;
    case 36: return 1;
    case 37: return 1;
    case 38: return 1;
    case 39: return 1;
    case 40: return 1;
    case 41: return 1;
    case 42: return 1;
    case 43: return 1;
    case 44: return 1;
    case 45: return 1;
    case 46: return 1;
    case 47: return 1;
    case 48: return 1;
    case 49: return 1;
    case 50: return 1;
    case 51: return 1;
    case 52: return 1;
    case 53: return 1;
    case 54: return 1;
    case 55: return 1;
    case 56: return 1;
    case 57: return 1;
    case 58: return 1;
    case 59: return 1;
    case 60: return 1;
    case 61: return W3GSAction2_6F67EC10(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x3D), but a real handler here
    case 62: return W3GSAction2_6F67EE10(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x3E), but a real handler here
    case 63: return W3GSAction2_6F67EF90(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x3F), but a real handler here
    case 64: return W3GSAction2_6F67F0E0(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x40), but a real handler here
    case 65: return 1;
    case 66: return 1;
    case 67: return W3GSAction2_6F67F930(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x43), but a real handler here
    case 68: return W3GSAction2_6F67FC60(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x44), but a real handler here
    case 69: return W3GSAction2_6F67FD30(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x45), but a real handler here
    case 70: return 1;
    case 71: return W3GSAction2_6F67B8C0(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x47), but a real handler here
    case 72: return W3GSAction2_6F67E8B0(self, Src, opcodeDword);   // undefined in the GameActionId enum supplied (0x48), but a real handler here
    case 73: return 1;
    case 74: return 1;
    case 75: return 1;
    case 76: return 1;
    case 77: return 1;
    case 78: return 1;
    case 79: return 1;
    case 80: return W3GSAction2_6F67BB60(self, Src, opcodeDword);   // ChangeAllyOptions (0x50)
    case 81: return W3GSAction2_6F67BB80(self, Src, opcodeDword);   // TransferResources (0x51)
    default: return 1;
    }
}
