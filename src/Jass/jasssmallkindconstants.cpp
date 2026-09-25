//============================================================================
//  0x6F455AC0-0x6F455BC0 - seventeen one-instruction leaves, laid out at a
//  uniform 0x10 stride and reached only indirectly (no static caller in
//  this closure's own xrefs) - the shape of a table of small per-class
//  overrides compiled consecutively from a source list, not a single
//  function's own switch.  Two return a 32-bit `int` (5, 6 - the second
//  matching `NodeStartFunc`'s own already-documented kind literal,
//  Jass/jassnodectors.cpp), the other fifteen an 8-bit value in `al`
//  (0x18-0x26) - the exact width split IDA's own disassembly shows, kept
//  rather than guessed at.  What table actually holds these addresses is
//  not resolved from this closure; named by their own return value (the
//  "distinguishing fact" the naming standard asks for when the real role
//  isn't known yet) rather than left `sub_`.
//============================================================================

//  0x6F455AC0.  (IDA's own auto-name, `__RTC_NumErrors_61`, is a stock
//  runtime-check-table artifact that happens to land here - not a real
//  name for this function.)
unsigned int GetSmallKindConstant5() { return 5; }

//  0x6F455AD0 - matches NodeStartFunc's own kind literal (jassnodectors.cpp).
unsigned int GetSmallKindConstant6() { return 6; }

unsigned char GetSmallKindConstant0x18() { return 0x18; }
unsigned char GetSmallKindConstant0x19() { return 0x19; }
unsigned char GetSmallKindConstant0x1A() { return 0x1A; }
unsigned char GetSmallKindConstant0x1B() { return 0x1B; }
unsigned char GetSmallKindConstant0x1C() { return 0x1C; }
unsigned char GetSmallKindConstant0x1D() { return 0x1D; }
unsigned char GetSmallKindConstant0x1E() { return 0x1E; }
unsigned char GetSmallKindConstant0x1F() { return 0x1F; }
unsigned char GetSmallKindConstant0x20() { return 0x20; }
unsigned char GetSmallKindConstant0x21() { return 0x21; }
unsigned char GetSmallKindConstant0x22() { return 0x22; }
unsigned char GetSmallKindConstant0x23() { return 0x23; }
unsigned char GetSmallKindConstant0x24() { return 0x24; }
unsigned char GetSmallKindConstant0x25() { return 0x25; }
unsigned char GetSmallKindConstant0x26() { return 0x26; }
