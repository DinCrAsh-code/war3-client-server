//============================================================================
//  0x6F4C19F0 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp).  Two raw Storm ordinal calls, neither
//  seen elsewhere in this closure - a 9-count registration against a fixed
//  table (unk_6F954558) and a (0,0) call whose purpose is not established
//  by this call tree.
//============================================================================

//  Storm ordinal 476/479 - not otherwise declared in this tree.  Storm_476's
//  own push order (`push 9` then `push offset table` right before the
//  call) puts the table pointer as the first (stdcall, right-to-left)
//  parameter, the count second.
void __stdcall Storm_476(void* table, unsigned int count);
void __stdcall Storm_479(int a, int b);

extern void* g_handlerTable9;   // unk_6F954558

void __fastcall GameMainRegisterHandlerTable()
{
    Storm_476(&g_handlerTable9, 9);
    Storm_479(0, 0);
}
