//============================================================================
//  0x6F001700 - a genuinely empty function: one `retn`, no stack cleanup, no
//  register touched.  Reached only through the scope table appended after
//  sub_6F42CB50 (0x6F82B368: `call nullsub_45` from the __finally
//  trampoline, with `add esp, 8` afterwards - i.e. two dword arguments the
//  callee never looks at, consistent with a plain __cdecl stub).  IDA never
//  resolved a name for it and nothing in this call tree gives it one
//  either; it is not itself part of sub_6F42CB50's __try/__finally
//  reconstruction (left TODO this pass - see docs/msvc-vc8-idioms.md), only
//  a leaf its scope table happens to call.
//============================================================================
void NoOpFinallyStub(int, int) {}
