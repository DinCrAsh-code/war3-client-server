//============================================================================
//  0x6F3B2ED0 - OrderResultIsSuccess: did an order check succeed?
//
//  Zero is success and so is 0xDC, the one refusal code the ability layer
//  raises that still lets the order be issued (CUnit::CheckTargetOrder
//  records it in its own module flag on the way past).  Everything else is
//  a refusal.
//
//  __fastcall with one argument and `retn 0`: the code arrives in ecx at
//  both of its call sites in 0x6F3B3090 and nothing is ever pushed.
//============================================================================

int __fastcall OrderResultIsSuccess(int code)
{
    if (code != 0 && code != 0xDC)
        return 0;

    return 1;
}
