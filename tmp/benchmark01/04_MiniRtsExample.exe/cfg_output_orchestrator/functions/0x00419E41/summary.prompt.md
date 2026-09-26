# Function summary: __except_handler4 (0x00419E41)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00419E41
- Name: __except_handler4 (real name)
- Size: 35 bytes, 9 instructions
- One call chain from the analysis root down to it: _main -> sub_4012B0 -> ??_L@YGXPAXIHP6EX0@Z1@Z -> __SEH_prolog4 -> __except_handler4

## How callers use this function

### From __SEH_prolog4 (0x00419DE8) - address taken at 0x00419DE8

What that caller does overall (preliminary): Standard MSVC compiler runtime helper (__SEH_prolog4) that sets up a Structured Exception Handling (SEH) frame, security cookies, and custom stack allocation for functions requiring exception guards.

Pushed onto the stack at the very beginning of the prolog (address 0x00419DE8) to be registered as the SEH exception filter/handler callback (`__except_handler4`) for the current activation record, managing unwinding when exceptions occur during operations like array construction or destruction.

Reference kinds used above:
- *address taken*: the function's address is used as data there - e.g. passed as a callback or stored as a function pointer - rather than called directly.

## Functions it calls or references

### @__security_check_cookie@4 [__security_check_cookie] (0x004198F2) - address taken at 0x00419E51

- Prototype: `void __fastcall __security_check_cookie(uintptr_t stack_cookie)`
- Summary: Validates that the stack canary value passed in ecx matches the global security cookie, jumping to the GS failure handler if a mismatch is detected.

### _except_handler4_common (0x0041A120) - call at 0x00419E5B

- Summary: Import thunk: jumps straight to the imported API MSVCR80!_except_handler4_common.

Reference kinds used above:
- *address taken*: the function's address is used as data there - e.g. passed as a callback or stored as a function pointer - rather than called directly.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00419E41  push [esp+arg_C]
0x00419E45  push [esp+4+arg_8]
0x00419E49  push [esp+8+arg_4]
0x00419E4D  push [esp+0Ch+arg_0]
0x00419E51  push offset @__security_check_cookie@4       ; -> @__security_check_cookie@4 [__security_check_cookie]
0x00419E56  push offset ___security_cookie               ; -> 0x0041F000 in .data
0x00419E5B  call _except_handler4_common                 ; -> _except_handler4_common
0x00419E60  add esp, 18h
0x00419E63  retn
```

## Output

Reply with exactly one JSON object - no prose before or after it, no markdown code fences - with these keys:

{
  "suggested_name": "descriptive identifier-style name, e.g. \"ParseExpression\" or \"CUnit::TakeDamage\"",
  "prototype": "best-guess C/C++ prototype including calling convention, e.g. \"double __cdecl ParseExpression(Parser *p)\"",
  "short_summary": "1-3 sentences on WHAT the function does - this is what the analysis of its callers will see",
  "detailed_summary": "comprehensive description of HOW it does it: control flow, loops/branches and their conditions, the algorithm, what it uses each callee for, error handling, notable constants",
  "parameters": [{"name": "...", "location": "e.g. [esp+4] / ecx / eax", "type": "...", "description": "..."}],
  "return_value": "what is returned and where (eax / edx:eax / st(0)), or \"none\"",
  "side_effects": ["e.g. \"frees every node of the list\", \"writes to stdout\", \"modifies global 0x0040C004\""],
  "data_structures": ["inferred struct layouts / field meanings, e.g. \"Node: +0x0 int value, +0x4 Node *next\""],
  "confidence": "high | medium | low, followed by a short reason"
}

Use empty lists where there's nothing to report.
