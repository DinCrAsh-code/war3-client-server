//============================================================================
//  Two adjacent one-instruction opcode handlers from the
//  Probably_W3GS_ActionHandler_MaybeServerVersion (0x6F682300) switch:
//  0x6F6737A0 (opcode 3, GameActionId.SetGameSpeed) and 0x6F6737B0 (opcode
//  27, GameActionId.TriggerSelectionEvent - confirmed by the real
//  byte_6F682788 table, see w3gsactionhandler2.cpp), both a bare
//  `xor eax,eax / retn 0xC` - unconditional `return 0`, same shape as this
//  dump's sibling root's own trivial handlers (w3gsaction_stubs.cpp).
//============================================================================

int __stdcall W3GSAction2_6F6737A0(void*, unsigned int, unsigned int) { return 0; }
int __stdcall W3GSAction2_6F6737B0(void*, unsigned int, unsigned int) { return 0; }
