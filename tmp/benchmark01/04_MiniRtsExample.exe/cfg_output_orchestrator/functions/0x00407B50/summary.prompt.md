# Function summary: sub_407B50 (0x00407B50)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00407B50
- Name: sub_407B50 (auto-generated placeholder)
- Size: 1158 bytes, 332 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_407B50

## How callers use this function

### From sub_401750 (0x00401750) - call at 0x00401DA9

What that caller does overall (preliminary): Initializes and configures a large resource/state block ('RM' header structure) passed from main, setting up internal sub-systems, tables, processing item loops, and validation checks before invoking subsystem teardown or startup functions.

Called with a pointer to a status/control structure (`ebx+571D4h`), after zeroing fields within it, to perform validation or reset operations.

## Functions it calls or references

### _CIsqrt (0x0041A212) - call at 0x00407C23, 0x00407C8A

- Summary: Import thunk: jumps straight to the imported API MSVCR80!_CIsqrt.

### sub_418050 [CalculateSpatialOrPathMetric] (0x00418050) - call at 0x00407D85

- Prototype: `double __cdecl CalculateSpatialOrPathMetric()`
- Summary: Performs complex floating-point geometry, clamping, and polynomial evaluation calculations using values on the x87 FPU stack.

### __ftol2_sse (0x0041A160) - call at 0x00407E4F, 0x00407E96

- Prototype: `int __cdecl __ftol2_sse(void)`
- Summary: Converts the top-of-stack x87 floating-point value to a 32-bit signed integer using SSE or fallback FPU routines, returning the integer result in EAX.

### sub_402560 [CreateAndRegisterResourceEntity] (0x00402560) - call at 0x00407F3B

- Prototype: `void* __fastcall CreateAndRegisterResourceEntity(void *resourceBlock, int unused_edx, int type_index, int sub_variant_flag, int flags)`
- Summary: Allocates, initializes, and configures a new resource or RTS object entity based on type lookup tables, coordinates, and flags, registering it into spatial grid and resource tracking containers.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00407B50  push ebp
0x00407B51  mov ebp, esp
0x00407B53  and esp, 0FFFFFFF8h
0x00407B56  sub esp, 44h
0x00407B59  fld                                          ; -> 0x0041F028 in .data
0x00407B5F  push ebx
0x00407B60  fmul                                         ; -> 0x0041D8C8 in .rdata
0x00407B66  push esi
0x00407B67  mov esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00407B6D  cmp dword ptr [esi+10164h], 0
0x00407B74  fstp
0x00407B78  lea eax, [esi+10164h]
0x00407B7E  fld
0x00407B82  push edi
0x00407B83  lea edi, [esi+51EF8h]
0x00407B89  fst
0x00407B8D  fstp
0x00407B91  mov [esp+50h+var_34], edi
0x00407B95  mov [esp+50h+var_28], 0
0x00407B9D  mov [esp+50h+var_1C], eax
0x00407BA1  jle loc_407FC7
0x00407BA7  lea eax, [esi+10104h]
0x00407BAD  mov [esp+50h+var_2C], eax
0x00407BB1  jmp loc_407BBD
0x00407BB3  mov eax, [esp+50h+var_2C]
0x00407BB7  mov esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00407BBD  mov ecx, [ebp+arg_0]
0x00407BC0  cmp dword ptr [ecx+1B0h], 0Ch
0x00407BC7  jge loc_407FC7
0x00407BCD  mov edx, [eax]
0x00407BCF  fld                                          ; -> 0x0041D9C0 in .rdata
0x00407BD5  mov eax, [eax+4]
0x00407BD8  fstp
0x00407BDC  mov [esp+50h+var_14], eax
0x00407BE0  mov eax, [esi+5BBBCh]
0x00407BE6  test eax, eax
0x00407BE8  mov [esp+50h+var_18], edx
0x00407BEC  jle loc_407C5A
0x00407BEE  add esi, 0ECh
0x00407BF4  mov ebx, eax
0x00407BF6  fld
0x00407BFA  fsub
0x00407BFC  fstp
0x00407C00  fld
0x00407C04  fsub
0x00407C07  fstp
0x00407C0B  fld
0x00407C0F  fld
0x00407C13  fmul st, st
0x00407C15  fld
0x00407C17  fmulp st(2), st
0x00407C19  faddp st(1), st
0x00407C1B  fstp
0x00407C1F  fld
0x00407C23  call _CIsqrt                                 ; -> _CIsqrt
0x00407C28  fstp
0x00407C2C  fld
0x00407C30  fstp
0x00407C34  fld
0x00407C38  fld
0x00407C3C  fcom
0x00407C3E  fnstsw ax
0x00407C40  fstp
0x00407C42  test ah, 41h
0x00407C45  jz loc_407C4D
0x00407C47  fstp
0x00407C4B  jmp loc_407C4F
0x00407C4D  fstp
0x00407C4F  add esi, 108h
0x00407C55  sub ebx, 1
0x00407C58  jnz loc_407BF6
0x00407C5A  fld
0x00407C5E  fsub
0x00407C62  fstp
0x00407C66  fld
0x00407C6A  fsub
0x00407C6E  fstp
0x00407C72  fld
0x00407C76  fld
0x00407C7A  fmul st, st
0x00407C7C  fld
0x00407C7E  fmulp st(2), st
0x00407C80  faddp st(1), st
0x00407C82  fstp
0x00407C86  fld
0x00407C8A  call _CIsqrt                                 ; -> _CIsqrt
0x00407C8F  fstp
0x00407C93  fld
0x00407C97  fcomp                                        ; -> 0x0041DBE8 in .rdata
0x00407C9D  fnstsw ax
0x00407C9F  test ah, 5
0x00407CA2  jp loc_407CAB
0x00407CA4  mov eax, 6
0x00407CA9  jmp loc_407D15
0x00407CAB  mov edx, [edi+4]
0x00407CAE  fld
0x00407CB2  fcomp                                        ; -> 0x0041DBE0 in .rdata
0x00407CB8  mov ecx, edx
0x00407CBA  shl ecx, 0Dh
0x00407CBD  xor ecx, edx
0x00407CBF  mov edx, ecx
0x00407CC1  shr edx, 11h
0x00407CC4  fnstsw ax
0x00407CC6  xor edx, ecx
0x00407CC8  mov ecx, edx
0x00407CCA  shl ecx, 5
0x00407CCD  xor ecx, edx
0x00407CCF  test ah, 5
0x00407CD2  mov eax, [edi]
0x00407CD4  mov [edi+4], ecx
0x00407CD7  jp loc_407CFD
0x00407CD9  imul eax
0x00407CDF  add eax, 3C6EF35Fh
0x00407CE4  mov [edi], eax
0x00407CE6  shr eax, 10h
0x00407CE9  xor eax, ecx
0x00407CEB  xor edx, edx
0x00407CED  mov ecx, 3
0x00407CF2  div
0x00407CF4  mov eax, ds:dword_41D33C[edx*4]              ; -> 0x0041D33C in .rdata
0x00407CFB  jmp loc_407D15
0x00407CFD  imul eax
0x00407D03  add eax, 3C6EF35Fh
0x00407D08  mov [edi], eax
0x00407D0A  shr eax, 10h
0x00407D0D  xor eax, ecx
0x00407D0F  and eax, 3
0x00407D12  add eax, 2
0x00407D15  mov esi, [ebp+arg_0]
0x00407D18  fldz
0x00407D1A  mov ecx, [esi+1B0h]
0x00407D20  lea ecx, [ecx+ecx*8]
0x00407D23  lea esi, [esi+ecx*4]
0x00407D26  mov ecx, [esp+50h+var_18]
0x00407D2A  lea edx, [eax+eax*2]
0x00407D2D  mov [esi], ecx
0x00407D2F  mov ecx, [esp+50h+var_14]
0x00407D33  lea edx, ds:41CE54h[edx*2]                   ; -> 0x0041CE54 in .rdata
0x00407D3A  mov [esi+4], ecx
0x00407D3D  mov [esi+8], al
0x00407D40  mov al, [edx]
0x00407D42  fstp
0x00407D45  fld
0x00407D49  sub esp, 8
0x00407D4C  fsub
0x00407D50  mov [esp+58h+var_24], edx
0x00407D54  mov [esi+9], al
0x00407D57  mov byte ptr [esi+0Ah], 0
0x00407D5B  fstp
0x00407D5F  mov byte ptr [esi+0Bh], 0
0x00407D63  fld
0x00407D67  mov dword ptr [esi+20h], 0
0x00407D6E  fstp
0x00407D72  fld
0x00407D76  fsub
0x00407D7A  fstp
0x00407D7E  fld
0x00407D82  fstp
0x00407D85  call sub_418050                              ; -> sub_418050 [CalculateSpatialOrPathMetric]
0x00407D8A  fstp
0x00407D8E  add esp, 8
0x00407D91  fldz
0x00407D93  fld
0x00407D97  fcom
0x00407D99  fnstsw ax
0x00407D9B  test ah, 5
0x00407D9E  jp loc_407DC3
0x00407DA0  fld                                          ; -> 0x0041D8A8 in .rdata
0x00407DA6  jmp loc_407DAA
0x00407DA8  fxch
0x00407DAA  fadd st(1), st
0x00407DAC  fxch
0x00407DAE  fstp
0x00407DB2  fld
0x00407DB6  fcom
0x00407DB8  fnstsw ax
0x00407DBA  test ah, 5
0x00407DBD  jnp loc_407DA8
0x00407DBF  fstp
0x00407DC1  jmp loc_407DCB
0x00407DC3  fstp
0x00407DC5  fld                                          ; -> 0x0041D8A8 in .rdata
0x00407DCB  fcom
0x00407DCD  fnstsw ax
0x00407DCF  test ah, 41h
0x00407DD2  jp loc_407DEB
0x00407DD4  fsub st(1), st
0x00407DD6  fxch
0x00407DD8  fstp
0x00407DDC  fld
0x00407DE0  fcom
0x00407DE2  fnstsw ax
0x00407DE4  fxch
0x00407DE6  test ah, 1
0x00407DE9  jz loc_407DD4
0x00407DEB  xor ecx, ecx
0x00407DED  lea eax, [esi+0Ch]
0x00407DF0  mov [esp+50h+var_30], ecx
0x00407DF4  mov [esp+50h+var_3C], eax
0x00407DF8  jmp loc_407DFE
0x00407DFA  mov edx, [esp+50h+var_24]
0x00407DFE  mov eax, [esp+50h+var_3C]
0x00407E02  mov dword ptr [eax], 0
0x00407E08  movzx eax, byte ptr [edx+1]
0x00407E0C  cmp ecx, eax
0x00407E0E  mov [esp+50h+var_38], eax
0x00407E12  jge loc_407F84
0x00407E18  mov bl, [ecx+edx+2]
0x00407E1C  cmp bl, 0FFh
0x00407E1F  jz loc_407F84
0x00407E25  fild
0x00407E29  fdivr st, st(1)
0x00407E2B  fimul
0x00407E2F  fadd st, st(2)
0x00407E31  fstp
0x00407E35  fld
0x00407E39  fld
0x00407E3B  fld                                          ; -> 0x0041D8B0 in .rdata
0x00407E41  fmul st(1), st
0x00407E43  fxch
0x00407E45  fstp
0x00407E49  fld
0x00407E4D  fld
0x00407E4F  call __ftol2_sse                             ; -> __ftol2_sse
0x00407E54  mov edi, eax
0x00407E56  mov [esp+50h+var_38], edi
0x00407E5A  fild
0x00407E5E  fcomp
0x00407E60  fnstsw ax
0x00407E62  test ah, 41h
0x00407E65  jnz loc_407E6E
0x00407E67  sub edi, 1
0x00407E6A  mov [esp+50h+var_38], edi
0x00407E6E  fisub
0x00407E72  and edi, 3FFh
0x00407E78  fstp
0x00407E7C  fxch
0x00407E7E  fadd                                         ; -> 0x0041D918 in .rdata
0x00407E84  fstp
0x00407E88  fmul
0x00407E8C  fstp
0x00407E90  fld
0x00407E94  fld
0x00407E96  call __ftol2_sse                             ; -> __ftol2_sse
0x00407E9B  mov ecx, eax
0x00407E9D  mov [esp+50h+var_38], ecx
0x00407EA1  fild
0x00407EA5  fcomp
0x00407EA7  fnstsw ax
0x00407EA9  test ah, 41h
0x00407EAC  jnz loc_407EB5
0x00407EAE  sub ecx, 1
0x00407EB1  mov [esp+50h+var_38], ecx
0x00407EB5  fisub
0x00407EB9  and ecx, 3FFh
0x00407EBF  lea eax, ds:421818h[ecx*4]                   ; -> 0x00421818 in .data
0x00407EC6  movzx edx, bl
0x00407EC9  test edx, edx
0x00407ECB  fstp
0x00407ECF  fld                                          ; -> 0x0042181C in .data
0x00407ED6  fsub
0x00407ED8  fmul
0x00407EDC  fadd
0x00407EDE  lea eax, ds:421818h[edi*4]                   ; -> 0x00421818 in .data
0x00407EE5  fstp
0x00407EE9  fld
0x00407EED  fld                                          ; -> 0x0041DB30 in .rdata
0x00407EF3  fmul st(1), st
0x00407EF5  fld
0x00407EF9  faddp st(2), st
0x00407EFB  fxch
0x00407EFD  fstp
0x00407F01  fld                                          ; -> 0x0042181C in .data
0x00407F08  fsub
0x00407F0A  fmul
0x00407F0E  fadd
0x00407F10  fstp
0x00407F14  fmul
0x00407F18  fadd
0x00407F1C  fstp
0x00407F20  jl loc_407FD0
0x00407F26  push 0
0x00407F28  fstp
0x00407F2A  push ecx
0x00407F2B  mov ecx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00407F31  fstp
0x00407F34  push 4
0x00407F36  push ecx
0x00407F37  lea ecx, [esp+60h+var_8]
0x00407F3B  call sub_402560                              ; -> sub_402560 [CreateAndRegisterResourceEntity]
0x00407F40  fld                                          ; -> 0x0041D8A8 in .rdata
0x00407F46  test eax, eax
0x00407F48  fld
0x00407F4C  mov edi, [esp+50h+var_34]
0x00407F50  jz loc_407F82
0x00407F52  mov edx, [ebp+arg_0]
0x00407F55  mov ecx, [edx+1B0h]
0x00407F5B  mov edx, [esp+50h+var_8]
0x00407F5F  mov [eax+238h], ecx
0x00407F65  mov ecx, [esp+50h+var_4]
0x00407F69  mov [eax+230h], edx
0x00407F6F  mov edx, [eax+4]
0x00407F72  mov [eax+234h], ecx
0x00407F78  mov eax, [esp+50h+var_3C]
0x00407F7C  mov [eax], edx
0x00407F7E  add byte ptr [esi+0Ah], 1
0x00407F82  fxch
0x00407F84  mov ecx, [esp+50h+var_30]
0x00407F88  add [esp+50h+var_3C], 4
0x00407F8D  add ecx, 1
0x00407F90  cmp ecx, 4
0x00407F93  mov [esp+50h+var_30], ecx
0x00407F97  jl loc_407DFA
0x00407F9D  mov eax, [ebp+arg_0]
0x00407FA0  fstp
0x00407FA2  add dword ptr [eax+1B0h], 1
0x00407FA9  fstp
0x00407FAB  mov eax, [esp+50h+var_28]
0x00407FAF  mov ecx, [esp+50h+var_1C]
0x00407FB3  add [esp+50h+var_2C], 8
0x00407FB8  add eax, 1
0x00407FBB  cmp eax, [ecx]
0x00407FBD  mov [esp+50h+var_28], eax
0x00407FC1  jl loc_407BB3
0x00407FC7  pop edi
0x00407FC8  pop esi
0x00407FC9  pop ebx
0x00407FCA  mov esp, ebp
0x00407FCC  pop ebp
0x00407FCD  retn 4
0x00407FD0  mov edi, [esp+50h+var_34]
0x00407FD4  jmp loc_407F84
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
