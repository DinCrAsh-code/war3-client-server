# Function summary: sub_402A30 (0x00402A30)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00402A30
- Name: sub_402A30 (auto-generated placeholder)
- Size: 282 bytes, 94 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404AD0 -> sub_402A30

## How callers use this function

### From sub_404AD0 (0x00404AD0) - call at 0x00404C1F

What that caller does overall (preliminary): Iterates over a collection of sub-objects/elements stored within an entity (passed in eax), performing distance-to-target calculations or state checks to conditionally trigger sub-routines, management actions, or status updates on the entity.

Called at 0x00404C1F with a pointer to [edi+34h] to check or query a condition/state. If it returns a non-zero pointer, fields from the result are used in subsequent entity processing.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00402A30  sub esp, 14h
0x00402A33  fld
0x00402A37  push ebp
0x00402A38  mov ebp, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00402A3E  fmul st, st
0x00402A40  mov eax, [ebp+573B4h]
0x00402A46  xor ecx, ecx
0x00402A48  test eax, eax
0x00402A4A  fstp
0x00402A4E  mov [esp+18h+var_C], ecx
0x00402A52  jle loc_402B41
0x00402A58  mov ecx, [ebp+573B0h]
0x00402A5E  push ebx
0x00402A5F  push esi
0x00402A60  mov [esp+20h+arg_4], ecx
0x00402A64  mov [esp+20h+var_10], eax
0x00402A68  push edi
0x00402A69  lea esp, [esp+0]
0x00402A70  mov edx, [esp+24h+arg_4]
0x00402A74  mov ebx, [edx]
0x00402A76  mov al, [ebx+18h]
0x00402A79  not al
0x00402A7B  test al, 1
0x00402A7D  jz loc_402B23
0x00402A83  mov eax, [esp+24h+arg_0]
0x00402A87  fld
0x00402A8A  fsub
0x00402A8C  fstp
0x00402A90  fld
0x00402A93  fsub
0x00402A96  fstp
0x00402A9A  fld
0x00402A9E  fld
0x00402AA2  fmul st, st
0x00402AA4  fld
0x00402AA6  fmulp st(2), st
0x00402AA8  faddp st(1), st
0x00402AAA  fstp
0x00402AAE  fld
0x00402AB2  fld
0x00402AB6  fcomp
0x00402AB8  fnstsw ax
0x00402ABA  test ah, 41h
0x00402ABD  jnp loc_402B21
0x00402ABF  movzx eax, word ptr [ebx+1Ch]
0x00402AC3  cdq
0x00402AC4  mov ecx, 60h
0x00402AC9  idiv
0x00402ACB  xor cl, cl
0x00402ACD  xor esi, esi
0x00402ACF  test cl, cl
0x00402AD1  jnz loc_402B17
0x00402AD3  mov ecx, ds:dword_41D32C[esi]                ; -> 0x0041D32C in .rdata
0x00402AD9  mov edi, ds:dword_41D31C[esi]                ; -> 0x0041D31C in .rdata
0x00402ADF  add ecx, eax
0x00402AE1  add edi, edx
0x00402AE3  js loc_402AF3
0x00402AE5  test ecx, ecx
0x00402AE7  jl loc_402AF3
0x00402AE9  cmp edi, 60h
0x00402AEC  jge loc_402AF3
0x00402AEE  cmp ecx, 60h
0x00402AF1  jl loc_402AF7
0x00402AF3  mov cl, 23h
0x00402AF5  jmp loc_402B06
0x00402AF7  lea ecx, [ecx+ecx*2]
0x00402AFA  shl ecx, 5
0x00402AFD  add ecx, edi
0x00402AFF  mov cl, [ecx+ebp+0D124h]
0x00402B06  not cl
0x00402B08  add esi, 4
0x00402B0B  and cl, 1
0x00402B0E  cmp esi, 10h
0x00402B11  jl loc_402ACF
0x00402B13  test cl, cl
0x00402B15  jz loc_402B21
0x00402B17  fstp
0x00402B1B  mov [esp+24h+var_C], ebx
0x00402B1F  jmp loc_402B23
0x00402B21  fstp
0x00402B23  add [esp+24h+arg_4], 4
0x00402B28  sub [esp+24h+var_10], 1
0x00402B2D  jnz loc_402A70
0x00402B33  mov eax, [esp+24h+var_C]
0x00402B37  pop edi
0x00402B38  pop esi
0x00402B39  pop ebx
0x00402B3A  pop ebp
0x00402B3B  add esp, 14h
0x00402B3E  retn 8
0x00402B41  mov eax, ecx
0x00402B43  pop ebp
0x00402B44  add esp, 14h
0x00402B47  retn 8
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
