//============================================================================
//  The two fixed-constant getters checkedslot_createunit_assign.cpp's
//  Assign bodies call - own translation unit, deliberately, the same way
//  every other member of the agenttypedslots.h family keeps its own getter
//  separate from its Assign (misc_rawcode_getters.cpp vs.
//  ordertypedassign.cpp, checkedslot_batch25_getters.cpp vs.
//  checkedslot_batch25.cpp): a same-TU definition is a compile-time
//  constant the optimiser inlines straight into the caller, where the
//  shipped Assign genuinely calls out and keeps the result in a register.
//============================================================================

//  0x6F26C3E0 - "uref" MSB-first.
unsigned int GetFixedRawcode_6F26C3E0() { return 0x75726566u; }

//  0x6F2E59B0 - "AI-U" MSB-first.
unsigned int GetFixedRawcode_6F2E59B0() { return 0x41492D55u; }
