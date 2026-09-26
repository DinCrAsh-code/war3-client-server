# Function summary: sub_402E50 (0x00402E50)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00402E50
- Name: sub_402E50 (auto-generated placeholder)
- Size: 621 bytes, 207 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404AD0 -> sub_402E50

## How callers use this function

### From sub_404AD0 (0x00404AD0) - call at 0x00404B3A, 0x00404CC8

What that caller does overall (preliminary): Iterates over a collection of sub-objects/elements stored within an entity (passed in eax), performing distance-to-target calculations or state checks to conditionally trigger sub-routines, management actions, or status updates on the entity.

Called twice (at 0x00404B3A and 0x00404CC8) to query or retrieve an associated object/target related to coordinates or state offset [edi+34h]. The returned pointer is checked for validity (non-zero) to determine subsequent conditional execution paths or status thresholds.

### From sub_405520 (0x00405520) - call at 0x004055B7

What that caller does overall (preliminary): Performs complex spatial collision, proximity, or intersection checks for an entity against environment elements or other entities, iterating over collections of candidate structures and returning a boolean success status along with target location coordinates.

Called once near the beginning of the function (at 0x004055B7) to initialize or retrieve state data using a local parameter block containing pointers and configuration values derived from the entity and global tables.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00402E50  sub esp, 0Ch
0x00402E53  fld
0x00402E57  push ebx
0x00402E58  mov ebx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00402E5E  fmul st, st
0x00402E60  push ebp
0x00402E61  mov ebp, [esp+14h+arg_0]
0x00402E65  push esi
0x00402E66  fstp
0x00402E6A  mov esi, [ebx+57390h]
0x00402E70  xor eax, eax
0x00402E72  cmp esi, 4
0x00402E75  mov [esp+18h+var_8], eax
0x00402E79  jl loc_403038
0x00402E7F  mov edx, [ebx+5738Ch]
0x00402E85  push edi
0x00402E86  lea edi, [esi-4]
0x00402E89  shr edi, 2
0x00402E8C  add edx, 8
0x00402E8F  add edi, 1
0x00402E92  lea eax, ds:0[edi*4]
0x00402E99  mov [esp+1Ch+var_4], eax
0x00402E9D  lea ecx, [ecx+0]
0x00402EA0  mov ecx, [edx-8]
0x00402EA3  mov eax, [ecx+1Ch]
0x00402EA6  test byte ptr [eax], 80h
0x00402EA9  jz loc_402F02
0x00402EAB  mov al, [ecx+18h]
0x00402EAE  not al
0x00402EB0  test al, 1
0x00402EB2  jz loc_402F02
0x00402EB4  cmp dword ptr [ecx+2B8h], 0
0x00402EBB  jle loc_402F02
0x00402EBD  fld
0x00402EC0  fsub
0x00402EC3  fstp
0x00402EC7  fld
0x00402ECA  fsub
0x00402ECD  fstp
0x00402ED1  fld
0x00402ED5  fld
0x00402ED9  fmul st, st
0x00402EDB  fld
0x00402EDD  fmulp st(2), st
0x00402EDF  faddp st(1), st
0x00402EE1  fstp
0x00402EE5  fld
0x00402EE9  fld
0x00402EED  fcomp
0x00402EEF  fnstsw ax
0x00402EF1  test ah, 41h
0x00402EF4  jnz loc_402F00
0x00402EF6  fstp
0x00402EFA  mov [esp+1Ch+var_8], ecx
0x00402EFE  jmp loc_402F02
0x00402F00  fstp
0x00402F02  mov ecx, [edx-4]
0x00402F05  mov eax, [ecx+1Ch]
0x00402F08  test byte ptr [eax], 80h
0x00402F0B  jz loc_402F64
0x00402F0D  mov al, [ecx+18h]
0x00402F10  not al
0x00402F12  test al, 1
0x00402F14  jz loc_402F64
0x00402F16  cmp dword ptr [ecx+2B8h], 0
0x00402F1D  jle loc_402F64
0x00402F1F  fld
0x00402F22  fsub
0x00402F25  fstp
0x00402F29  fld
0x00402F2C  fsub
0x00402F2F  fstp
0x00402F33  fld
0x00402F37  fld
0x00402F3B  fmul st, st
0x00402F3D  fld
0x00402F3F  fmulp st(2), st
0x00402F41  faddp st(1), st
0x00402F43  fstp
0x00402F47  fld
0x00402F4B  fld
0x00402F4F  fcomp
0x00402F51  fnstsw ax
0x00402F53  test ah, 41h
0x00402F56  jnz loc_402F62
0x00402F58  fstp
0x00402F5C  mov [esp+1Ch+var_8], ecx
0x00402F60  jmp loc_402F64
0x00402F62  fstp
0x00402F64  mov ecx, [edx]
0x00402F66  mov eax, [ecx+1Ch]
0x00402F69  test byte ptr [eax], 80h
0x00402F6C  jz loc_402FC5
0x00402F6E  mov al, [ecx+18h]
0x00402F71  not al
0x00402F73  test al, 1
0x00402F75  jz loc_402FC5
0x00402F77  cmp dword ptr [ecx+2B8h], 0
0x00402F7E  jle loc_402FC5
0x00402F80  fld
0x00402F83  fsub
0x00402F86  fstp
0x00402F8A  fld
0x00402F8D  fsub
0x00402F90  fstp
0x00402F94  fld
0x00402F98  fld
0x00402F9C  fmul st, st
0x00402F9E  fld
0x00402FA0  fmulp st(2), st
0x00402FA2  faddp st(1), st
0x00402FA4  fstp
0x00402FA8  fld
0x00402FAC  fld
0x00402FB0  fcomp
0x00402FB2  fnstsw ax
0x00402FB4  test ah, 41h
0x00402FB7  jnz loc_402FC3
0x00402FB9  fstp
0x00402FBD  mov [esp+1Ch+var_8], ecx
0x00402FC1  jmp loc_402FC5
0x00402FC3  fstp
0x00402FC5  mov ecx, [edx+4]
0x00402FC8  mov eax, [ecx+1Ch]
0x00402FCB  test byte ptr [eax], 80h
0x00402FCE  jz loc_403027
0x00402FD0  mov al, [ecx+18h]
0x00402FD3  not al
0x00402FD5  test al, 1
0x00402FD7  jz loc_403027
0x00402FD9  cmp dword ptr [ecx+2B8h], 0
0x00402FE0  jle loc_403027
0x00402FE2  fld
0x00402FE5  fsub
0x00402FE8  fstp
0x00402FEC  fld
0x00402FEF  fsub
0x00402FF2  fstp
0x00402FF6  fld
0x00402FFA  fld
0x00402FFE  fmul st, st
0x00403000  fld
0x00403002  fmulp st(2), st
0x00403004  faddp st(1), st
0x00403006  fstp
0x0040300A  fld
0x0040300E  fld
0x00403012  fcomp
0x00403014  fnstsw ax
0x00403016  test ah, 41h
0x00403019  jnz loc_403025
0x0040301B  fstp
0x0040301F  mov [esp+1Ch+var_8], ecx
0x00403023  jmp loc_403027
0x00403025  fstp
0x00403027  add edx, 10h
0x0040302A  sub edi, 1
0x0040302D  jnz loc_402EA0
0x00403033  mov eax, [esp+1Ch+var_4]
0x00403037  pop edi
0x00403038  cmp eax, esi
0x0040303A  jge loc_4030B0
0x0040303C  mov ecx, [ebx+5738Ch]
0x00403042  lea edx, [ecx+eax*4]
0x00403045  sub esi, eax
0x00403047  mov ecx, [edx]
0x00403049  mov eax, [ecx+1Ch]
0x0040304C  test byte ptr [eax], 80h
0x0040304F  jz loc_4030A8
0x00403051  mov al, [ecx+18h]
0x00403054  not al
0x00403056  test al, 1
0x00403058  jz loc_4030A8
0x0040305A  cmp dword ptr [ecx+2B8h], 0
0x00403061  jle loc_4030A8
0x00403063  fld
0x00403066  fsub
0x00403069  fstp
0x0040306D  fld
0x00403070  fsub
0x00403073  fstp
0x00403077  fld
0x0040307B  fld
0x0040307F  fmul st, st
0x00403081  fld
0x00403083  fmulp st(2), st
0x00403085  faddp st(1), st
0x00403087  fstp
0x0040308B  fld
0x0040308F  fld
0x00403093  fcomp
0x00403095  fnstsw ax
0x00403097  test ah, 41h
0x0040309A  jnz loc_4030A6
0x0040309C  fstp
0x004030A0  mov [esp+18h+var_8], ecx
0x004030A4  jmp loc_4030A8
0x004030A6  fstp
0x004030A8  add edx, 4
0x004030AB  sub esi, 1
0x004030AE  jnz loc_403047
0x004030B0  mov eax, [esp+18h+var_8]
0x004030B4  pop esi
0x004030B5  pop ebp
0x004030B6  pop ebx
0x004030B7  add esp, 0Ch
0x004030BA  retn 8
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
