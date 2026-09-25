//============================================================================
//  0x6F313740 - IsPlayerSlotOccupied.
//
//  A slot really has a player in it when its +0xF0 handle ref is set and its
//  +0xE0 one is not.  Both are read through the same
//  HandleRefFieldOwner_6F473170::QueryField78 (Misc/queryfield.cpp) the rest
//  of this batch uses, which is what the two `lea`s of a sub-object address
//  say and what two independent field loads would not.
//
//  Own translation unit: 0x6F314A70 calls it for real, and the two sit in
//  different shipped modules.
//============================================================================
struct HandleRefFieldOwner_6F473170 { int __thiscall QueryField78(); };

int __fastcall IsPlayerSlotOccupied(const void* slot);

int __fastcall IsPlayerSlotOccupied(const void* slot)
{
    if (((HandleRefFieldOwner_6F473170*)((char*)slot + 0xF0))->QueryField78() != 0
        && ((HandleRefFieldOwner_6F473170*)((char*)slot + 0xE0))->QueryField78() == 0)
        return 1;

    return 0;
}
