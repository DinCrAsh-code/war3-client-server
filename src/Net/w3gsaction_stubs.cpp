//============================================================================
//  Seven adjacent one-instruction opcode handlers from the
//  Probably_W3GS_ActionHandler (0x6F672340) switch, all packed into the same
//  0x6F664500-0x6F664572 neighbourhood (address adjacency = shipped
//  together, CLAUDE.md) - two of them (0x6F664500/0x6F664550) IDA left as
//  plain sub_ names, the other five (0x6F6644E0/F0/6F664510/20/30) it named
//  `_DllMain@12_N`, almost certainly because a 3-arg/retn 0xC/all-paths-
//  return-a-constant shape happens to be exactly what a trivial DllMain
//  looks like to IDA's signature matcher - nothing in this call tree treats
//  them as an actual DLL entry point (they are called from
//  w3gsactionhandler.cpp's switch like every other opcode handler here, with
//  the same 3-argument shape the whole switch forwards). Kept the IDA names
//  as-is rather than inventing opcode-based ones since renaming across a
//  whole funcmap for a name that already reads fine would only make the
//  history harder to follow.
//============================================================================

int __stdcall _DllMain_at_6F6644E0(void*, unsigned int, unsigned int) { return 1; }
int __stdcall _DllMain_at_6F6644F0(void*, unsigned int, unsigned int) { return 1; }
int __stdcall _DllMain_at_6F664510(void*, unsigned int, unsigned int) { return 1; }
int __stdcall _DllMain_at_6F664520(void*, unsigned int, unsigned int) { return 1; }
int __stdcall _DllMain_at_6F664530(void*, unsigned int, unsigned int) { return 1; }

int __stdcall W3GSAction_6F664500(void*, unsigned int, unsigned int) { return 0; }
int __stdcall W3GSAction_6F664550(void*, unsigned int, unsigned int) { return 0; }
