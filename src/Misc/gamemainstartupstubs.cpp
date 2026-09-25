//============================================================================
//  gamemain-batch-3: three genuinely empty one-`retn` stubs, each reached
//  by a tail `jmp`/`call` from its own two-instruction caller elsewhere in
//  the GameMain closure (neither caller is in this session's batch).  Same
//  shape as nullsub_45 (Misc/nullstub.cpp): IDA never resolved a name for
//  any of the three, and nothing in this call tree gives one either.
//============================================================================

//  0x6F481230 - reached by `jmp nullsub_35` from sub_6F4674F0.
void GameMainNoOpStub1() {}

//  0x6F481240 - reached by `call nullsub_17` from sub_6F467500.
void GameMainNoOpStub2() {}

//  0x6F4B73C0 - reached by `jmp nullsub_36`, right after GameMainNoOpStub2,
//  from the same sub_6F467500.
void GameMainNoOpStub3() {}

//----------------------------------------------------------------------------
//  0x6F7AD8E0 - `mov eax,1 / retn`, reached only by a tail `jmp` from
//  sub_6F7AD910 (outside this session's batch).  A fixed "yes" answer with
//  no state read; named for its shape rather than a guessed purpose, the
//  same way GameMainNoOpStub1-3 above are.
//----------------------------------------------------------------------------
int AlwaysTrueStub()
{
    return 1;
}
