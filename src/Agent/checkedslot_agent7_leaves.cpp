//============================================================================
//  cunit_agent7 batch: two rawcode leaves for SCheckedAAbtSlot::Assign
//  (checkedaabtslot.cpp) and SCheckedAHbuSlot::Assign (checkedahbuslot.cpp).
//
//  Given their own TU rather than `static` in the caller's own file: a
//  `static` one-instruction function inlines away entirely at this
//  build's fixed /Ob2, which loses the dump's own out-of-line `call` and
//  its `mov esi, eax` result copy (cost 4 of 42 instructions on
//  SCheckedAAbtSlot::Assign before this fix - GetTypeTag_6F028840,
//  Misc/trivialaccessors_02_03.cpp's own precedent, avoids exactly this
//  by living in a different TU already).
//============================================================================

//  0x6F0241E0 - 'AAbt'.  Nothing in this batch's own closure names what
//  ability or class this tags (see checkedaabtslot.cpp).
unsigned int GetAAbtTypeTag()
{
    return 0x41416274u;
}

//  0x6F0D9020 - 'AHbu' - cunit_agent5's own worklist address (see
//  checkedahbuslot.cpp for why a second, unnamed copy here is harmless).
unsigned int GetAHbuTypeTag()
{
    return 0x41486275u;
}
