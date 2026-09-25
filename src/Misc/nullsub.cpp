//============================================================================
//  0x6F20BC60 - IDA's `nullsub_1`: one `retn` and nothing else. The shipped
//  binary uses it as the do-nothing target for the CDebugLock diagnostic
//  logging calls (debuglock.cpp, threadwaitpool.cpp), which `call` it on
//  every slot they walk.
//
//  Reproduced as its own naked `ret` rather than as a RAW_THUNK jump in
//  unreconstructed_thunks.cpp on purpose: this is a complete reconstruction,
//  not a stand-in, and the thunk macro would `mov eax, <addr>` on the way
//  through - the original touches no register at all, so a thunk would be
//  observably different for any caller that reads eax afterwards.
//
//  `extern "C"` here is load-bearing: every call site is a transcribed
//  `__asm { call nullsub_1 }`, which names the C-decorated symbol
//  `_nullsub_1`.  A C++ definition would mangle to ?nullsub_1@@YAXXZ and
//  none of those call sites would resolve.
//============================================================================

extern "C" __declspec(naked) void nullsub_1()
{
    __asm { ret }
}
