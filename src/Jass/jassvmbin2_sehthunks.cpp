//============================================================================
//  jscript-bin2's own SEH-framed / genuinely-out-of-budget addresses: every
//  one opens `push -1 / push offset SEH_.../mov eax,fs:0` (`_except_handler4`)
//  or is a large multi-callee function whose closure reaches unclaimed
//  addresses this round's own budget does not cover.  Self-redirect THUNKs
//  (`mov eax, <own address> / jmp eax`) - real, unhooked shipped code keeps
//  running; see pipeline/CLAUDE.md's own SEH note (this fixed
//  `/GS- /EHs-c-` toolchain cannot emit an `_except_handler4` frame from any
//  C++ source shape) for why the SEH ones are structural, not a skipped
//  attempt.
//============================================================================
#include "jassscanner.h"

//  0x6F011D70 - SFile-based map-script existence probe (calls the sibling
//  0x6F00E580 reconstructed in currentarchivecache.cpp); plain `/GS` cookie
//  only, no SEH, but its own callee sub_6F00CC00 has no reconstruction
//  anywhere in this tree yet - out of this round's own budget.  Shipped
//  `retn 4` (one stack dword) - declared `__stdcall` with one dummy `int`
//  so this placeholder cleans the right number of bytes if anything ever
//  calls through it live (thunk_abi_audit.py's own FATAL class).
__declspec(naked) void __stdcall Thunk_6F011D70(int)
{
    __asm { mov eax, 06F011D70h }
    __asm { jmp eax }
}

//  0x6F340550 - RCString-style constructor, `_except_handler4`-framed.
__declspec(naked) void Thunk_6F340550()
{
    __asm { mov eax, 06F340550h }
    __asm { jmp eax }
}

//  0x6F3B1D00 - the round's own flagship (LoadJassScriptSources: opens
//  common.j/blizzard.j/the map's own numbered script).  Two of its three
//  identical calls into sub_6F3B1970 pass their arguments through `esi`/
//  `edi` with no stack push at all ahead of the `call` - a whole-program-
//  optimisation-derived calling convention (the same class of thing
//  `pipeline/CLAUDE.md`'s own `CConfigFile::GetIntValue`/`edx`,
//  `ServerTock_0x27_PacketHandler`/`edi` examples document) no MSVC C++
//  spelling this toolchain offers can reproduce, and sub_6F3B1970 itself
//  is claimed by a sibling session (jscript-bin4) rather than this one.
//  Self-redirect rather than a doomed C++ attempt.
__declspec(naked) void Thunk_6F3B1D00()
{
    __asm { mov eax, 06F3B1D00h }
    __asm { jmp eax }
}

//  0x6F3B5310 - a caller referencing "scripts\\common.j" directly; its own
//  closure reaches sub_6F455800, which has no reconstruction anywhere in
//  this tree - out of this round's own budget.
__declspec(naked) void Thunk_6F3B5310()
{
    __asm { mov eax, 06F3B5310h }
    __asm { jmp eax }
}

//  0x6F44D3D0 - a per-bucket TSExplicitList walk that also dispatches
//  through an unconfirmed vtable slot (`call [eax]` on `edi`, no committed
//  vtable for whatever class `edi` is) - real work, but guessing the slot's
//  argument shape risks exactly the live crash
//  `tools/vtable_dispatch_audit.py` exists to catch; out of this round's
//  own budget to pin down which class owns that vtable.
__declspec(naked) void Thunk_6F44D3D0()
{
    __asm { mov eax, 06F44D3D0h }
    __asm { jmp eax }
}

//  0x6F44DAA0 - a growable-array insert whose own fields split across two
//  different base addresses in a way this closure alone could not resolve
//  cleanly (the top-level count/alloc/data triple does not sit at the
//  offsets its own ComputeChunk/SetAlloc sub-object - 4 bytes further in -
//  uses for the identical fields) - guessing the layout risks a wrong
//  field write, not just a lower score.
__declspec(naked) void Thunk_6F44DAA0()
{
    __asm { mov eax, 06F44DAA0h }
    __asm { jmp eax }
}

//  0x6F453C50 - GetThreadLocalSlot(kThreadLocalJass) plus a call into
//  sub_6F453AF0, a ~120-instruction JassInstance array-teardown method with
//  no reconstruction anywhere in this tree - out of this round's own
//  budget.
__declspec(naked) void Thunk_6F453C50()
{
    __asm { mov eax, 06F453C50h }
    __asm { jmp eax }
}

//  0x6F454EC0 / 0x6F454F40 - JASS symbol-table internals reaching several
//  more unreconstructed sibling addresses (sub_6F44FD40, sub_6F44B790,
//  sub_6F44B860, sub_6F3A0480, sub_6F3A1AA0) - real work, out of this
//  round's own budget to chase the whole closure.
__declspec(naked) void Thunk_6F454EC0()
{
    __asm { mov eax, 06F454EC0h }
    __asm { jmp eax }
}
__declspec(naked) void Thunk_6F454F40()
{
    __asm { mov eax, 06F454F40h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  jass-multiscript-A - the yy_scan multi-script compile chain
//  `LoadAndCompileCommonScripts` (0x6F3B1C30) funnels into.  Full mechanism
//  write-up: docs/notes/jass-multi-script-compile-mechanism.md.  All four
//  below open `push -1 / push offset SEH_.../mov eax, fs:0` -
//  `_except_handler4`-framed, structurally uncompilable by this fixed
//  `/GS- /EHs-c-` toolchain - so all four stay THUNK regardless of how
//  precisely their logic is understood; the comments record the finding
//  even though the bodies themselves cannot be transcribed.
//
//  The real loop this chain drives (walking the caller-supplied
//  {name,buffer} array and switching buffers as each one is exhausted) is
//  NOT in any of these four - it lives entirely in `yy_scan`'s own vtable
//  slot 1, `yy_scan::AdvanceScriptBuffer` (0x6F4587D0), a real
//  reconstruction - see Jass/jassscanner.h/.cpp.
//----------------------------------------------------------------------------

//  0x6F458900 - `CompileScriptChain`, `retn 8`.  Called with ecx = &array
//  (a caller-built stack array of `{const char* name; void* buffer;}`
//  pairs terminated by a `{0,0}` pair - see LoadAndCompileCommonScripts,
//  jscriptbin3_scriptloaderthunks.cpp) and edx = a diagnostic-formatting
//  callback (sub_6F3B18A0).  Spills both to the stack (`arg_0`/`arg_4`),
//  then makes exactly ONE call to `yy_scan::yy_scan` (0x6F458870, builds
//  the scanner from the array) and exactly ONE call to
//  `CompileOneScriptChainPass` (0x6F458510, runs one parse-and-install
//  pass over it).  Confirms the task's own open question: this function is
//  NOT unrolled over 2-3 calls and does NOT loop itself - the multi-script
//  chaining happens entirely inside the scanner object these two calls set
//  up, driven later by the lexer's own EOF handling through
//  `yy_scan::AdvanceScriptBuffer`.  On success (a nonzero flag `yy_scan`
//  leaves at +0x18), builds a `SHashedStringTablePrimer` (already
//  reconstructed, `DIFFERS`) from the parsed function name/checksum pair
//  and calls `sub_6F4550F0`/`sub_6F460530` (the scanner's own teardown -
//  `yy_scan::~yy_scan`) either way.
__declspec(naked) void __fastcall CompileScriptChain(void*, void*)
{
    __asm { mov eax, 06F458900h }
    __asm { jmp eax }
}

//  0x6F458870 - `yy_scan::yy_scan`, `retn 10h` (4 stack args + `this`).
//  Stamps `??_7yy_scan@@6B@`, default-constructs the embedded
//  compile-context object at +0x2C via `yy_scan::ConstructCompileContext`
//  (0x6F4586D0, below), then reads the SAME two register values
//  (`&array`/callback) back out of its own spilled args TWICE - once as
//  `arg_0`/`arg_4`, again (byte-identical) as `arg_8`/`arg_C` - because its
//  own caller (`CompileScriptChain` above) forwards `ecx`/`edx` into a
//  4-stack-arg call without ever reassigning them in between.  Forwards
//  all four (with a fixed size constant, 0x834) into
//  `yy_scan::InitFromScriptChain` (0x6F460570, a real reconstruction - see
//  jassscanner.cpp) and separately stores the callback/array pointers at
//  +0xC8/+0xCC/+0xD8 for the parser's own later use (error reporting via
//  sub_6F3B18A0 - NOT a second buffer, correcting this round's own
//  preliminary read of +0xD8).
struct yy_scan_ctor_helper { void Construct(void*, void*, void*, void*); };
__declspec(naked) void __thiscall yy_scan_ctor_helper::Construct(void*, void*, void*, void*)
{
    __asm { mov eax, 06F458870h }
    __asm { jmp eax }
}

//  0x6F4586D0 - `yy_scan::ConstructCompileContext`, `retn 0`.  Default-
//  constructs a not-otherwise-identified compile-context object embedded
//  at `yy_scan+0x2C` - two nested
//  `TAllocatorHashTable<Symbol,HASHKEY_CONSTSTR,1024>` members at its own
//  +0x04/+0x2C (i.e. scanner+0x30/scanner+0x58, via sub_6F457C50, itself
//  unreconstructed), then declares a "handle" symbol into the first of
//  them (`Storm_590` + `sub_6F458440`).  Out of this round's own scope -
//  unrelated to the multi-script mechanism itself, only present because
//  `yy_scan::yy_scan` calls it before seeding the script chain.
struct yy_scan_CompileContext_ctor_helper { void Construct(); };
__declspec(naked) void yy_scan_CompileContext_ctor_helper::Construct()
{
    __asm { mov eax, 06F4586D0h }
    __asm { jmp eax }
}

//  0x6F458510 - `yy_scan::CompileOneScriptChainPass`, `retn 8` - see
//  jassscanner.h for the declaration.  Builds a fixed-size (0x258 =
//  600-entry) `SJassParserBuffers` value/state stack pair on its own
//  frame (0x6F460790, freed by the already-`EXACT`
//  `SJassParserBuffers::ReleaseBuffers`), preloads it with the scanner's
//  own diagnostic callback (+0xD8), then makes exactly ONE call into the
//  flagship AST builder (`JassAstBuilder_Owner::Build`, 0x6F464460 -
//  `JassVM.md`'s own flagship function, THUNK).  This single call is what
//  actually compiles every script the chain resolves to as ONE continuous
//  token stream: `sub_6F464460`'s own lexer main loop
//  (`JassLexer_NextToken_6F45FF30`) calls through `yy_scan`'s own vtable
//  slots 0/1 (`NextChar`/`AdvanceScriptBuffer`) whenever the current
//  buffer runs out, and slot 1 is what walks from one array entry to the
//  next.  So "one call here" does not mean "one script": it means "one
//  compile unit", however many buffers `AdvanceScriptBuffer`'s own walk
//  chains into it.  On success, clones the compiled buffer
//  (`JassBuffer_Owner::Clone`) and writes the parsed name/checksum back
//  through its own out-pointers before releasing the scanner's
//  SJassParserBuffers state.
__declspec(naked) void yy_scan::CompileOneScriptChainPass(void*, void*)
{
    __asm { mov eax, 06F458510h }
    __asm { jmp eax }
}

//  0x6F45ABA0 - a TSExplicitList<T> rehash/reserve (Storm_401/405 realloc
//  dance, ~115 instructions) with no matching generic member in
//  Containers/storm.h yet - real work, out of this round's own budget.
__declspec(naked) void Thunk_6F45ABA0()
{
    __asm { mov eax, 06F45ABA0h }
    __asm { jmp eax }
}

//  0x6F45AFA0 - a masked hash-bucket lookup whose own bucket-struct field
//  order this closure could not pin down with confidence (two different
//  offset conventions read from what looks like the same bucket shape) -
//  out of this round's own budget to resolve without guessing at a layout
//  a wrong guess would silently corrupt.
__declspec(naked) void Thunk_6F45AFA0()
{
    __asm { mov eax, 06F45AFA0h }
    __asm { jmp eax }
}

//  0x6F45E7A0 - `_except_handler4`-framed JassInstance internals.
__declspec(naked) void Thunk_6F45E7A0()
{
    __asm { mov eax, 06F45E7A0h }
    __asm { jmp eax }
}

//  0x6F45FA30 - 258-instruction function, out of this round's own budget.
//  Shipped `retn 24` (six stack dwords) - declared `__stdcall` with six
//  dummy `int` args so this placeholder cleans the right number of bytes.
__declspec(naked) void __stdcall Thunk_6F45FA30(int, int, int, int, int, int)
{
    __asm { mov eax, 06F45FA30h }
    __asm { jmp eax }
}
