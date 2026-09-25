//============================================================================
//  CUnit batch-14 (agent-cunit-batch-14) - naked redirects for addresses
//  this batch confirmed open a genuine SEH frame this toolchain's fixed
//  /GS- /EHs-c- flags cannot reproduce: either a full __except_handler4
//  prologue (`push -1 / push offset SEH_<addr> / mov eax, large fs:0`) or
//  the plain /GS stack-cookie shape (`mov eax, dword_6FAAE140; xor eax,
//  esp`) this build compiles without.  Same class of finding as the
//  sixteen already documented in docs/targets/CUnit__vtable.md - not
//  reconstruction targets, never offer any of these as hookable (a detour
//  cannot detour a detour).
//
//  Argument byte counts are read straight off each dump's own trailing
//  `retn` line (tools/thunk_abi_audit.py checks every one against its
//  dump).  The functions with no caller anywhere in this tree use
//  `__stdcall` with N plain `int` parameters, the simplest spelling
//  that cleans exactly 4*N bytes without claiming a real calling
//  convention. The two Storm-side helpers two of this batch's leaves do
//  call for real (unit_storm_wrappers.cpp) are declared as members of a
//  minimal proxy class instead, so `this` lands in `ecx` and the stack
//  args push in the exact right-to-left order their own dumps read.
//  0x6F2A0420/0x6F2A4880 got a real caller in a later pass (slot 3's own
//  Method_0x0C, unit_dispatch.cpp) and were widened from zero-arg
//  __stdcall to a one-argument __fastcall taking `this` for exactly that
//  reason - see their own comments below.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                          \
    {                                               \
        __asm { mov eax, addr }                     \
        __asm { jmp eax }                            \
    }

//  0x6F2A0420 - `retn 0`.  Real __except_handler4 prologue.  CUnit vtable
//  slot-finalize pass: this address is one of slot 3's own message-case
//  handlers (Method_0x0C, unit_dispatch.cpp) and IS called there with
//  `this` in ecx (`mov ecx, esi`) - so it is widened from a bare
//  zero-parameter __stdcall to a one-argument __fastcall taking `this`
//  explicitly, which still cleans exactly 0 stack bytes (retn 0
//  unchanged; a __fastcall's first two arguments are register-only).
ADDR_THUNK(void __fastcall CUnitClosure_SEH_6F2A0420(void* self), 0x6F2A0420)

//  0x6F2A46F0 - `retn 8`.  Real __except_handler4 prologue.
ADDR_THUNK(void __stdcall CUnitClosure_SEH_6F2A46F0(int, int), 0x6F2A46F0)

//  0x6F2A4880 - plain /GS-shaped stack cookie, `retn 0`.  Same
//  slot-3-caller widening as 0x6F2A0420 above.
ADDR_THUNK(void __fastcall CUnitClosure_SEH_6F2A4880(void* self), 0x6F2A4880)

//  0x6F2D8F20 - `retn 4`.  Real __except_handler4 prologue.
ADDR_THUNK(void __stdcall CUnitClosure_SEH_6F2D8F20(int), 0x6F2D8F20)

//  0x6F310040 - `retn 0`.  Real __except_handler4 prologue.  Lazily
//  constructs the singleton at dword_6FAB5548 (an unnamed icon/art-path
//  registry - see sub_6F3143D0/sub_6F314420/sub_6F314470/sub_6F3202F0,
//  none of which this batch reconstructed: their own 4-argument calls
//  into the two members below depend on that registry's real member
//  layout, which nothing in this tree establishes).
ADDR_THUNK(void __stdcall CUnitClosure_SEH_6F310040(), 0x6F310040)

//  0x6F3103C0 - `retn 10h`.  Plain /GS-shaped stack cookie.  Same
//  unestablished-registry caveat as 0x6F310040 above.
ADDR_THUNK(void __stdcall CUnitClosure_SEH_6F3103C0(int, int, int, int),
           0x6F3103C0)

//  0x6F3104C0 - `retn 10h`.  Plain /GS-shaped stack cookie.  Same
//  unestablished-registry caveat as 0x6F310040 above.
ADDR_THUNK(void __stdcall CUnitClosure_SEH_6F3104C0(int, int, int, int),
           0x6F3104C0)

#undef ADDR_THUNK

//  0x6F76DE70 (`retn 0Ch`) and 0x6F74D450 (`retn 8`) - both real
//  __except_handler4 prologues, both reached only through
//  GetStormSingletonA()'s own return value as `this`.  Declared and
//  redirected as `CStormSingletonAThunkProxy` members in
//  unit_storm_wrappers.cpp, the one TU that calls them, so `this` lands in
//  `ecx` the way a plain free-function declaration cannot reach.
