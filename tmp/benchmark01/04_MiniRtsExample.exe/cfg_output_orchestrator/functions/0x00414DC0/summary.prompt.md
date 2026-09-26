# Function summary: sub_414DC0 (0x00414DC0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00414DC0
- Name: sub_414DC0 (auto-generated placeholder)
- Size: 120 bytes, 49 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402050 -> sub_402560 -> sub_414DC0

## How callers use this function

### From sub_402560 (0x00402560) - call at 0x0040270F

What that caller does overall (preliminary): Factory and initialization routine that allocates, configures, and registers resource sub-elements (such as graphics or state structures) based on lookup table flags and index criteria.

Called with geometry/coordinate parameters and flags to transform or store spatial data into a sub-structure at offset 0x1A20 of the main resource block.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00414DC0  push ebp
0x00414DC1  push esi
0x00414DC2  mov esi, [esp+8+arg_4]
0x00414DC6  push edi
0x00414DC7  mov edi, eax
0x00414DC9  cdq
0x00414DCA  sub eax, edx
0x00414DCC  sar eax, 1
0x00414DCE  sub ecx, eax
0x00414DD0  sub esi, eax
0x00414DD2  lea edx, [ecx+edi]
0x00414DD5  cmp ecx, edx
0x00414DD7  mov ebp, esi
0x00414DD9  jge loc_414E27
0x00414DDB  mov esi, [esp+0Ch+arg_0]
0x00414DDF  lea eax, [ecx+ecx*2]
0x00414DE2  add edi, ebp
0x00414DE4  shl eax, 5
0x00414DE7  lea esi, [eax+esi+0B704h]
0x00414DEE  mov edi, edi
0x00414DF0  cmp ebp, edi
0x00414DF2  mov eax, ebp
0x00414DF4  jge loc_414E1D
0x00414DF6  test eax, eax
0x00414DF8  jl loc_414E16
0x00414DFA  test ecx, ecx
0x00414DFC  jl loc_414E16
0x00414DFE  cmp eax, 60h
0x00414E01  jge loc_414E16
0x00414E03  cmp ecx, 60h
0x00414E06  jge loc_414E16
0x00414E08  test bl, bl
0x00414E0A  jz loc_414E12
0x00414E0C  or byte ptr [esi+eax], 13h
0x00414E10  jmp loc_414E16
0x00414E12  and byte ptr [esi+eax], 0ECh
0x00414E16  add eax, 1
0x00414E19  cmp eax, edi
0x00414E1B  jl loc_414DF6
0x00414E1D  add ecx, 1
0x00414E20  add esi, 60h
0x00414E23  cmp ecx, edx
0x00414E25  jl loc_414DF0
0x00414E27  mov eax, [esp+0Ch+arg_0]
0x00414E2B  add dword ptr [eax+0E748h], 1
0x00414E32  pop edi
0x00414E33  pop esi
0x00414E34  pop ebp
0x00414E35  retn 8
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
