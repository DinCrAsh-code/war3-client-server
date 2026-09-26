# Function summary: sub_404DF0 (0x00404DF0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00404DF0
- Name: sub_404DF0 (auto-generated placeholder)
- Size: 175 bytes, 57 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404AD0 -> sub_404DF0

## How callers use this function

### From sub_404EA0 (0x00404EA0) - call at 0x00404ECF, 0x00404F0A

What that caller does overall (preliminary): Interprets and executes a specific command or bytecode instruction from a configuration stream (pointed to by ebx) acting on an entity/object state (pointed to by edi), validating various bounds and state conditions via a switch statement before dispatching sub-operations.

Called to query or retrieve a metric/count value associated with an entity and an index (passed in ebx+1 and edi/entity pointer), used to compute bounds for command validation.

### From sub_404AD0 (0x00404AD0) - call at 0x00404D00

What that caller does overall (preliminary): Iterates over a collection of sub-objects/elements stored within an entity (passed in eax), performing distance-to-target calculations or state checks to conditionally trigger sub-routines, management actions, or status updates on the entity.

Called at 0x00404D00 when entity state counters and flags meet specific requirements, passing the main entity pointer in edi and a calculated index/parameter in esi on the stack.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00404DF0  sub esp, 8
0x00404DF3  push edi
0x00404DF4  mov edi, [esp+0Ch+arg_0]
0x00404DF8  mov eax, [edi+244h]
0x00404DFE  test eax, eax
0x00404E00  mov [esp+0Ch+var_8], 0
0x00404E08  jle loc_404E79
0x00404E0A  push ebx
0x00404E0B  push ebp
0x00404E0C  push esi
0x00404E0D  add edi, 144h
0x00404E13  mov [esp+18h+var_4], eax
0x00404E17  mov ebx, 1
0x00404E1C  lea esp, [esp+0]
0x00404E20  mov esi, [edi]
0x00404E22  cmp byte ptr [esi+70h], 0Ah
0x00404E26  jnz loc_404E69
0x00404E28  movzx eax, word ptr [esi+72h]
0x00404E2C  cmp eax, [esp+18h+arg_4]
0x00404E30  jnz loc_404E69
0x00404E32  movzx edx, byte ptr [esi+5Eh]
0x00404E36  xor eax, eax
0x00404E38  test edx, edx
0x00404E3A  jle loc_404E65
0x00404E3C  lea ecx, [esi+210h]
0x00404E42  mov ebp, [ecx]
0x00404E44  cmp [ebp+10h], bl
0x00404E47  jz loc_404E54
0x00404E49  add eax, ebx
0x00404E4B  add ecx, 4
0x00404E4E  cmp eax, edx
0x00404E50  jl loc_404E42
0x00404E52  jmp loc_404E65
0x00404E54  mov eax, [esi+eax*4+210h]
0x00404E5B  test eax, eax
0x00404E5D  jz loc_404E65
0x00404E5F  cmp dword ptr [eax+1Ch], 0
0x00404E63  jnz loc_404E69
0x00404E65  add [esp+18h+var_8], ebx
0x00404E69  add edi, 4
0x00404E6C  sub [esp+18h+var_4], ebx
0x00404E70  jnz loc_404E20
0x00404E72  mov edi, [esp+18h+arg_0]
0x00404E76  pop esi
0x00404E77  pop ebp
0x00404E78  pop ebx
0x00404E79  mov ecx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00404E7F  mov eax, [esp+0Ch+arg_4]
0x00404E83  mov edx, [ecx+5BBC0h]
0x00404E89  sub edx, [edi+eax*4+48h]
0x00404E8D  xor eax, eax
0x00404E8F  cmp edx, 1Eh
0x00404E92  setl al
0x00404E95  pop edi
0x00404E96  add eax, [esp+8+var_8]
0x00404E99  add esp, 8
0x00404E9C  retn 8
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
