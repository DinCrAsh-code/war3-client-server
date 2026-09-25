//============================================================================
//  The four fixed-constant getters checkedslot_batch25.cpp's Assign bodies
//  call - own translation unit, deliberately, the same way every other
//  member of the agenttypedslots.h family keeps its own '+xxx' getter in a
//  separate TU from its Assign (misc_rawcode_getters.cpp vs.
//  ordertypedassign.cpp etc.): a same-TU definition is a compile-time
//  constant the optimiser inlines straight into the caller (`mov edx,
//  <imm32>`), where the shipped Assign genuinely calls out and keeps the
//  result in a register - that gap is what a first draft with both
//  functions in one file measured (0.927, `mov edx, 1970496868` instead of
//  `call .../mov esi, eax`).
//============================================================================
unsigned int GetFixedRawcode_6F2700B0() { return 0x75736564; }
unsigned int GetFixedRawcode_6F270130() { return 0x75646464; }
unsigned int GetFixedRawcode_6F40BD90() { return 0x70757364; }
unsigned int GetFixedRawcode_6F40BE10() { return 0x7073656C; }
