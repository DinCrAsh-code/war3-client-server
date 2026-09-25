//============================================================================
//  Small standalone leaves reached directly off CUnit's own vtable BFS
//  (cunit_agent1_worklist.md, batch cunit_agent1) - each is its own
//  top-level entry in that dump rather than a callee pulled in by one of
//  the make-agent functions (cunit_agent1_makeagent.cpp).
//============================================================================

//  0x6F0D9220 - a bare FourCC constant, 'AGbu' (0x41476275).  Same
//  `mov eax, <imm32> / retn` shape as every other agile-type-id leaf in
//  this batch (cunit_agent1_fourcc_getters.cpp); no caller in this dump
//  gives it any further context, so it keeps the literal tag in its name.
unsigned int GetFourCC_6F0D9220()
{
    return 0x41476275;
}

//  0x6F1D6180 ('ACsp') and 0x6F0D90A0 ('AObu') are NOT defined here: both
//  turned out to already be cunit_agent6's own worklist items, properly
//  claimed and scored there (GetFourCC_6F1D6180/GetFourCC_6F0D90A0,
//  src/Misc/cunit_agent6_leaves.cpp). Their callers below declare them
//  extern instead of duplicating the definition - a second definition
//  under the identical name would be a real LNK2005 duplicate symbol,
//  not just a funcmap-row collision like this batch's other cross-agent
//  overlaps (see e.g. cunit_agent1_agentptr_hero_ctor.cpp's own note).

//  0x6F15C300 - 'Aupg'. Turned out to already be cunit_agent8's own
//  worklist item, properly claimed and scored there as
//  GetConstant_6F15C300 (src/Misc/misc_rawcode_getters.cpp) - a distinct
//  name, so this copy compiles and links fine, just carries no funcmap
//  row of its own.
unsigned int GetFourCC_6F15C300()
{
    return 0x41757067;
}
