//============================================================================
//  The two callees JASS_CreateUnit (0x6F3C5230) has that nothing else in
//  this repo already declares.  See jasscreateunit_native.cpp.
//============================================================================
#ifndef JASSCREATEUNIT_H
#define JASSCREATEUNIT_H

#include "CFloat.h"

class CUnit;

//  The optional progress callback JassNativePrologue pumps, and the tick
//  it last pumped it at (dword_6FAB6694 / dword_6FAB6698).  __stdcall
//  because the shipped call site does not clean the one float it pushes.
typedef void (__stdcall *ProgressTickFn)(float progress);
extern ProgressTickFn g_progressTick;
extern unsigned int   g_progressTickLast;

//  0x6F3B28F0 - called first, unconditionally, before the player handle is
//  even resolved: a 100ms-throttled pump of that callback.  See
//  jassnativeprologue.cpp.  Takes nothing and returns nothing, so its
//  `retn` is 0 under every convention.
void __fastcall JassNativePrologue();

//  0x6F24F1F0 - place a new unit of type `unitId`, owned by player
//  `playerIndex`, at (*posX, *posY) facing `*radians`, and hand back the
//  agent it created.
//
//  __fastcall (`playerIndex` in ecx, `unitId` in edx, three stack dwords),
//  and the dump confirms `retn 0Ch`.  It was declared this way by analogy
//  with its item twin 0x6F2B9940 `CreateItemAtPosition` before any dump of
//  it existed; the analogy held, but it was an inference until
//  `asm/sub_6F3C5230_*.md` arrived and thunk_abi_audit.py could check it.
CUnit* __fastcall CreateUnitForPlayer(int playerIndex, int unitId,
                                      const CFloat* posX,
                                      const CFloat* posY,
                                      const CFloat* radians);

//  0x6F29F990 - the real unit factory underneath it, and the only thing
//  CreateUnitForPlayer does beyond one animation call.  `retn 18h` read
//  straight off its own epilogue, which is six stack dwords past the two
//  __fastcall registers.  Not reconstructed - a 278-instruction SEH-framed
//  body with a 0x1E0-byte frame.
CUnit* __fastcall CreateUnitAtPosition(int playerIndex, int unitId,
                                       const CFloat* posX,
                                       const CFloat* posY,
                                       const CFloat* radians,
                                       int kind, int flag, int slot);

#endif
