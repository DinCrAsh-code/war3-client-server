//============================================================================
//  0x6F676DA0 - the function Probably_W3GS_ActionHandler_MaybeServerVersion's
//  own case for opcode 0x13 of *its* dispatch table (that dispatcher's own
//  opcode-to-function mapping is confirmed against the real byte_6F682788
//  remap table the user pulled from a debugger - see
//  w3gsactionhandler2.cpp's own header comment).
//
//  NOT `GameActionId.DropOrGiveItem`. That name was this session's own
//  mistake: it read this dispatcher's opcode range lining up positionally
//  with the WC3.Protocol `GameActionId` enum as proof this switch decodes
//  individual in-game action verbs, and opcode 0x13 in that enum happens to
//  be `DropOrGiveItem` - but this dispatcher (like its sibling,
//  Probably_W3GS_ActionHandler @ 0x6F672340) switches on a top-level W3GS
//  *packet* opcode, the same kind of value case 38/`0x26` of the sibling
//  dispatcher switches on to reach `GameAction_0x26_PacketHandler`
//  (gameaction0x26_handler.cpp) - not a `GameActionId` verb carried *inside*
//  a `GameAction` packet's own payload. The positional match to
//  `GameActionId` was coincidental, not semantic; nothing in this dump
//  establishes what real W3GS packet type opcode 0x13 here corresponds to.
//  Kept the address-based name (`W3GSAction2_6F676DA0`, matching every
//  other not-yet-identified case handler in this same file) rather than any
//  name implying confirmed item semantics.
//
//  What it does, independent of naming: parses a single 4-byte field
//  (ParseSingleDwordAction, w3gsaction2_parsesingledword.cpp) - rejecting
//  the packet (`return 1`, no state touched) unless that dword exactly
//  fills the declared payload length - then, on success, writes the
//  constant 14 (0xE) into a field at `self+0x44` and returns 0.
//  `self+0x44` is a field several *other* handlers in this same dispatcher
//  also write, each with its own small constant or raw register value - a
//  shared "last opcode handled" or similar tag, not anything item-specific.
//  Nothing in this call tree says what `self` is, what `+0x44` means beyond
//  "written by many unrelated handlers", what the one parsed dword actually
//  encodes, or what a caller does with the 0/1 return value.
//============================================================================

int __fastcall ParseSingleDwordAction(void* src, unsigned int declaredLength, unsigned int* out);

int __stdcall W3GSAction2_6F676DA0(void* self, unsigned int Src, unsigned int sizeDword)
{
    unsigned int field;
    if (!ParseSingleDwordAction((void*)Src, sizeDword, &field))
        return 1;

    *(unsigned int*)((char*)self + 0x44) = 0xE;
    return 0;
}
