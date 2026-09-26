# Function summary: sub_404460 (0x00404460)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00404460
- Name: sub_404460 (auto-generated placeholder)
- Size: 108 bytes, 33 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402050 -> sub_402560 -> sub_40B790 -> sub_404460

## How callers use this function

### From sub_40B790 (0x0040B790) - call at 0x0040B805

What that caller does overall (preliminary): Performs geometric, sizing, or configuration calculations on a newly allocated resource/object block (passed in ebx), evaluating floating-point parameters, structures, and lookup tables before invoking virtual methods on the object and its linked sub-elements.

Called during the initial series of floating-point and integer sizing calculations on fields extracted from the resource block's structure (`[ebx+1Ch]`), passing an adjusted integer index/value as an argument (pushed via edi + ebp) to perform a lookup or conversion helper operation.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00404460  mov dl, [esi+2]
0x00404463  cmp dl, 1
0x00404466  ja loc_404487
0x00404468  xor ecx, ecx
0x0040446A  mov eax, offset unk_41CB18                   ; -> 0x0041CB18 in .rdata
0x0040446F  nop
0x00404470  cmp [eax], dl
0x00404472  jnz loc_40447A
0x00404474  cmp byte ptr [eax+1], 2
0x00404478  jz loc_4044A0
0x0040447A  add eax, 10h
0x0040447D  add ecx, 1
0x00404480  cmp eax, offset flt_41CB98                   ; -> 0x0041CB98 in .rdata
0x00404485  jl loc_404470
0x00404487  mov [esp+arg_0], 0
0x0040448F  fild
0x00404493  fadd st, st
0x00404495  fstp
0x00404499  fld
0x0040449D  retn 4
0x004044A0  mov al, [esi+3Bh]
0x004044A3  test al, al
0x004044A5  jz loc_404487
0x004044A7  cmp al, 3
0x004044A9  jz loc_404487
0x004044AB  mov eax, [esp+arg_0]
0x004044AF  movzx ecx, byte ptr [ecx+eax+0D8h]
0x004044B7  mov [esp+arg_0], ecx
0x004044BB  fild
0x004044BF  fadd st, st
0x004044C1  fstp
0x004044C5  fld
0x004044C9  retn 4
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
