# Function summary: sub_406540 (0x00406540)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00406540
- Name: sub_406540 (auto-generated placeholder)
- Size: 541 bytes, 161 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_406760 -> sub_406540

## How callers use this function

### From sub_406760 (0x00406760) - call at 0x004068F7, 0x00406B21

What that caller does overall (preliminary): Performs state machine updates, distance/target calculations, and behavior checks for an individual simulation entity (passed in ebx) based on its current state, flags, and spatial proximity.

Called conditionally during state-dependent branching (at 0x004068F7 and 0x00406B21) with the entity pointer (ebx) and an offset/substructure pointer (edi) as arguments, returning a boolean condition (in al) that determines whether the entity transitions states or performs specific actions.

## Functions it calls or references

### sub_417E10 [CheckEntityGridFilter] (0x00417E10) - call at 0x004065EE

- Prototype: `bool __stdcall CheckEntityGridFilter(void *entityData)`
- Summary: Converts spatial coordinates into grid/lookup indices via floating-point arithmetic and integer clamping, then checks a bit flag within a lookup table or grid structure.

### sub_417F80 [CalculateDistance] (0x00417F80) - call at 0x004065FF

- Prototype: `double __cdecl CalculateDistance(float x1, float y1, float x2, float y2)`
- Summary: Computes the Euclidean distance between two 2D points (x1, y1) and (x2, y2).

### _CIsqrt (0x0041A212) - call at 0x004066F9

- Summary: Import thunk: jumps straight to the imported API MSVCR80!_CIsqrt.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00406540  push ebp
0x00406541  mov ebp, esp
0x00406543  and esp, 0FFFFFFF8h
0x00406546  sub esp, 14h
0x00406549  mov eax, [ebp+arg_0]
0x0040654C  fld                                          ; -> 0x0041D9C0 in .rdata
0x00406552  mov edx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406558  fstp
0x0040655C  push ebx
0x0040655D  push esi
0x0040655E  mov esi, [eax]
0x00406560  imul esi
0x00406566  xor eax, eax
0x00406568  add esi, edx
0x0040656A  cmp [edx+57390h], eax
0x00406570  push edi
0x00406571  mov [esp+20h+var_D], 0
0x00406576  mov [esp+20h+var_8], eax
0x0040657A  jle loc_406662
0x00406580  mov ecx, [edx+5738Ch]
0x00406586  mov edi, [ecx+eax*4]
0x00406589  mov al, [edi+18h]
0x0040658C  not al
0x0040658E  test al, 1
0x00406590  jz loc_406639
0x00406596  mov ecx, [edi+1Ch]
0x00406599  test byte ptr [ecx], 1
0x0040659C  jz loc_406639
0x004065A2  mov al, [edi+1Bh]
0x004065A5  cmp al, 4
0x004065A7  jnb loc_406639
0x004065AD  movzx ecx, al
0x004065B0  movzx eax, byte ptr [esi]
0x004065B3  cmp ecx, eax
0x004065B5  jz loc_406639
0x004065BB  cmp ecx, 5
0x004065BE  ja loc_406639
0x004065C0  test byte ptr [esi+3], 10h
0x004065C4  jnz loc_406639
0x004065C6  cmp ecx, 5
0x004065C9  jz loc_406639
0x004065CB  mov eax, 1
0x004065D0  shl eax, cl
0x004065D2  test [esi+0E8h], eax
0x004065D8  setz al
0x004065DB  test al, al
0x004065DD  jz loc_406639
0x004065DF  mov ecx, [ebp+arg_0]
0x004065E2  mov ebx, [ecx]
0x004065E4  add edx, 4467Ch
0x004065EA  add edi, 8
0x004065ED  push edx
0x004065EE  call sub_417E10                              ; -> sub_417E10 [CheckEntityGridFilter]
0x004065F3  test al, al
0x004065F5  jz loc_406633
0x004065F7  mov eax, [ebp+arg_0]
0x004065FA  add eax, 34h
0x004065FD  mov ecx, edi
0x004065FF  call sub_417F80                              ; -> sub_417F80 [CalculateDistance]
0x00406604  fstp
0x00406608  fld
0x0040660C  fld
0x00406610  fcomp
0x00406612  fnstsw ax
0x00406614  test ah, 41h
0x00406617  jnz loc_406631
0x00406619  mov edx, [edi]
0x0040661B  fstp
0x0040661F  mov eax, [ebp+arg_4]
0x00406622  mov [eax], edx
0x00406624  mov ecx, [edi+4]
0x00406627  mov [eax+4], ecx
0x0040662A  mov [esp+20h+var_D], 1
0x0040662F  jmp loc_406633
0x00406631  fstp
0x00406633  mov edx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406639  mov eax, [esp+20h+var_8]
0x0040663D  add eax, 1
0x00406640  cmp eax, [edx+57390h]
0x00406646  mov [esp+20h+var_8], eax
0x0040664A  jl loc_406580
0x00406650  cmp [esp+20h+var_D], 0
0x00406655  jz loc_406662
0x00406657  mov al, 1
0x00406659  pop edi
0x0040665A  pop esi
0x0040665B  pop ebx
0x0040665C  mov esp, ebp
0x0040665E  pop ebp
0x0040665F  retn 8
0x00406662  xor edi, edi
0x00406664  cmp [edx+5BBBCh], edi
0x0040666A  jle loc_406750
0x00406670  lea ebx, [edx+0ECh]
0x00406676  mov eax, [ebp+arg_0]
0x00406679  cmp edi, [eax]
0x0040667B  jz loc_40673B
0x00406681  test byte ptr [ebx-0E9h], 4
0x00406688  jnz loc_40673B
0x0040668E  movzx ecx, byte ptr [esi]
0x00406691  cmp edi, ecx
0x00406693  jz loc_40673B
0x00406699  cmp edi, 5
0x0040669C  ja loc_40673B
0x004066A2  test byte ptr [esi+3], 10h
0x004066A6  jnz loc_40673B
0x004066AC  cmp edi, 5
0x004066AF  jz loc_40673B
0x004066B5  mov eax, 1
0x004066BA  mov ecx, edi
0x004066BC  shl eax, cl
0x004066BE  test [esi+0E8h], eax
0x004066C4  setz al
0x004066C7  test al, al
0x004066C9  jz loc_40673B
0x004066CB  mov eax, [ebp+arg_0]
0x004066CE  fld
0x004066D0  fsub
0x004066D3  fstp
0x004066D7  fld
0x004066DA  fsub
0x004066DD  fstp
0x004066E1  fld
0x004066E5  fld
0x004066E9  fmul st, st
0x004066EB  fld
0x004066ED  fmulp st(2), st
0x004066EF  faddp st(1), st
0x004066F1  fstp
0x004066F5  fld
0x004066F9  call _CIsqrt                                 ; -> _CIsqrt
0x004066FE  fstp
0x00406702  fld
0x00406706  fstp
0x0040670A  fld
0x0040670E  fld
0x00406712  fcomp
0x00406714  fnstsw ax
0x00406716  test ah, 41h
0x00406719  jnz loc_406733
0x0040671B  mov ecx, [ebx]
0x0040671D  fstp
0x00406721  mov eax, [ebp+arg_4]
0x00406724  mov [eax], ecx
0x00406726  mov edx, [ebx+4]
0x00406729  mov [eax+4], edx
0x0040672C  mov [esp+20h+var_D], 1
0x00406731  jmp loc_406735
0x00406733  fstp
0x00406735  mov edx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x0040673B  add edi, 1
0x0040673E  add ebx, 108h
0x00406744  cmp edi, [edx+5BBBCh]
0x0040674A  jl loc_406676
0x00406750  mov al, [esp+20h+var_D]
0x00406754  pop edi
0x00406755  pop esi
0x00406756  pop ebx
0x00406757  mov esp, ebp
0x00406759  pop ebp
0x0040675A  retn 8
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
