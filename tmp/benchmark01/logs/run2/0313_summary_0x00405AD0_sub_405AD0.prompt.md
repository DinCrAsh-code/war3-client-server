# Function summary: sub_405AD0 (0x00405AD0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00405AD0
- Name: sub_405AD0 (auto-generated placeholder)
- Size: 1320 bytes, 444 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_406760 -> sub_405AD0

## How callers use this function

### From sub_406760 (0x00406760) - call at 0x0040677D

What that caller does overall (preliminary): Performs state machine updates, distance/target calculations, and behavior checks for an individual simulation entity (passed in ebx) based on its current state, flags, and spatial proximity.

Called near the beginning of the function (at 0x0040677D) to fetch some initial value or perform a preliminary computation involving global data before setting up entity behavior state checks.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00405AD0  sub esp, 8
0x00405AD3  fldz
0x00405AD5  push ebp
0x00405AD6  push esi
0x00405AD7  fst
0x00405ADB  fld                                          ; -> 0x0041D9A8 in .rdata
0x00405AE1  push edi
0x00405AE2  mov edi, [ebx+3C8h]
0x00405AE8  fld                                          ; -> 0x0041D9A0 in .rdata
0x00405AEE  xor ebp, ebp
0x00405AF0  cmp edi, 4
0x00405AF3  jl loc_405CD6
0x00405AF9  lea esi, [edi-4]
0x00405AFC  shr esi, 2
0x00405AFF  add esi, 1
0x00405B02  lea ecx, [ebx+24Ch]
0x00405B08  lea ebp, ds:0[esi*4]
0x00405B0F  mov edx, [ecx-4]
0x00405B12  mov al, [edx+18h]
0x00405B15  not al
0x00405B17  test al, 1
0x00405B19  jz loc_405B6A
0x00405B1B  mov eax, [edx+1Ch]
0x00405B1E  cmp byte ptr [eax+0Ch], 0
0x00405B22  jz loc_405B6A
0x00405B24  fld
0x00405B27  fstp
0x00405B2B  fld
0x00405B2F  fcom
0x00405B31  fnstsw ax
0x00405B33  test ah, 41h
0x00405B36  jnz loc_405B4E
0x00405B38  fstp
0x00405B3C  fld
0x00405B3F  fadd st, st(1)
0x00405B41  fmul
0x00405B44  fdiv
0x00405B48  fstp
0x00405B4C  jmp loc_405B72
0x00405B4E  fstp
0x00405B50  fxch
0x00405B52  fst
0x00405B56  fxch
0x00405B58  fld
0x00405B5B  fadd st, st(1)
0x00405B5D  fmul
0x00405B60  fdiv
0x00405B64  fstp
0x00405B68  jmp loc_405B72
0x00405B6A  fxch
0x00405B6C  fst
0x00405B70  fxch
0x00405B72  mov edx, [ecx]
0x00405B74  fld
0x00405B78  mov al, [edx+18h]
0x00405B7B  fadd
0x00405B7F  not al
0x00405B81  test al, 1
0x00405B83  fstp
0x00405B87  jz loc_405BD8
0x00405B89  mov eax, [edx+1Ch]
0x00405B8C  cmp byte ptr [eax+0Ch], 0
0x00405B90  jz loc_405BD8
0x00405B92  fld
0x00405B95  fstp
0x00405B99  fld
0x00405B9D  fcom
0x00405B9F  fnstsw ax
0x00405BA1  test ah, 41h
0x00405BA4  jnz loc_405BBC
0x00405BA6  fstp
0x00405BAA  fld
0x00405BAD  fadd st, st(1)
0x00405BAF  fmul
0x00405BB2  fdiv
0x00405BB6  fstp
0x00405BBA  jmp loc_405BE0
0x00405BBC  fstp
0x00405BBE  fxch
0x00405BC0  fst
0x00405BC4  fxch
0x00405BC6  fld
0x00405BC9  fadd st, st(1)
0x00405BCB  fmul
0x00405BCE  fdiv
0x00405BD2  fstp
0x00405BD6  jmp loc_405BE0
0x00405BD8  fxch
0x00405BDA  fst
0x00405BDE  fxch
0x00405BE0  mov edx, [ecx+4]
0x00405BE3  fld
0x00405BE7  mov al, [edx+18h]
0x00405BEA  fadd
0x00405BEE  not al
0x00405BF0  test al, 1
0x00405BF2  fstp
0x00405BF6  jz loc_405C47
0x00405BF8  mov eax, [edx+1Ch]
0x00405BFB  cmp byte ptr [eax+0Ch], 0
0x00405BFF  jz loc_405C47
0x00405C01  fld
0x00405C04  fstp
0x00405C08  fld
0x00405C0C  fcom
0x00405C0E  fnstsw ax
0x00405C10  test ah, 41h
0x00405C13  jnz loc_405C2B
0x00405C15  fstp
0x00405C19  fld
0x00405C1C  fadd st, st(1)
0x00405C1E  fmul
0x00405C21  fdiv
0x00405C25  fstp
0x00405C29  jmp loc_405C4F
0x00405C2B  fstp
0x00405C2D  fxch
0x00405C2F  fst
0x00405C33  fxch
0x00405C35  fld
0x00405C38  fadd st, st(1)
0x00405C3A  fmul
0x00405C3D  fdiv
0x00405C41  fstp
0x00405C45  jmp loc_405C4F
0x00405C47  fxch
0x00405C49  fst
0x00405C4D  fxch
0x00405C4F  mov edx, [ecx+8]
0x00405C52  fld
0x00405C56  mov al, [edx+18h]
0x00405C59  fadd
0x00405C5D  not al
0x00405C5F  test al, 1
0x00405C61  fstp
0x00405C65  jz loc_405CB6
0x00405C67  mov eax, [edx+1Ch]
0x00405C6A  cmp byte ptr [eax+0Ch], 0
0x00405C6E  jz loc_405CB6
0x00405C70  fld
0x00405C73  fstp
0x00405C77  fld
0x00405C7B  fcom
0x00405C7D  fnstsw ax
0x00405C7F  test ah, 41h
0x00405C82  jnz loc_405C9A
0x00405C84  fstp
0x00405C88  fld
0x00405C8B  fadd st, st(1)
0x00405C8D  fmul
0x00405C90  fdiv
0x00405C94  fstp
0x00405C98  jmp loc_405CBE
0x00405C9A  fstp
0x00405C9C  fxch
0x00405C9E  fst
0x00405CA2  fxch
0x00405CA4  fld
0x00405CA7  fadd st, st(1)
0x00405CA9  fmul
0x00405CAC  fdiv
0x00405CB0  fstp
0x00405CB4  jmp loc_405CBE
0x00405CB6  fxch
0x00405CB8  fst
0x00405CBC  fxch
0x00405CBE  fld
0x00405CC2  add ecx, 10h
0x00405CC5  sub esi, 1
0x00405CC8  fadd
0x00405CCC  fstp
0x00405CD0  jnz loc_405B0F
0x00405CD6  cmp ebp, edi
0x00405CD8  jge loc_405D5F
0x00405CDE  sub edi, ebp
0x00405CE0  lea edx, [ebx+ebp*4+248h]
0x00405CE7  mov esi, edi
0x00405CE9  mov ecx, [edx]
0x00405CEB  mov al, [ecx+18h]
0x00405CEE  not al
0x00405CF0  test al, 1
0x00405CF2  jz loc_405D43
0x00405CF4  mov eax, [ecx+1Ch]
0x00405CF7  cmp byte ptr [eax+0Ch], 0
0x00405CFB  jz loc_405D43
0x00405CFD  fld
0x00405D00  fstp
0x00405D04  fld
0x00405D08  fcom
0x00405D0A  fnstsw ax
0x00405D0C  test ah, 41h
0x00405D0F  jnz loc_405D27
0x00405D11  fstp
0x00405D15  fld
0x00405D18  fadd st, st(1)
0x00405D1A  fmul
0x00405D1D  fdiv
0x00405D21  fstp
0x00405D25  jmp loc_405D4B
0x00405D27  fstp
0x00405D29  fxch
0x00405D2B  fst
0x00405D2F  fxch
0x00405D31  fld
0x00405D34  fadd st, st(1)
0x00405D36  fmul
0x00405D39  fdiv
0x00405D3D  fstp
0x00405D41  jmp loc_405D4B
0x00405D43  fxch
0x00405D45  fst
0x00405D49  fxch
0x00405D4B  fld
0x00405D4F  add edx, 4
0x00405D52  sub esi, 1
0x00405D55  fadd
0x00405D59  fstp
0x00405D5D  jnz loc_405CE9
0x00405D5F  mov edi, [ebx+3DCh]
0x00405D65  fld                                          ; -> 0x0041D958 in .rdata
0x00405D6B  xor ebp, ebp
0x00405D6D  cmp edi, 4
0x00405D70  jl loc_405F5B
0x00405D76  lea esi, [edi-4]
0x00405D79  shr esi, 2
0x00405D7C  add esi, 1
0x00405D7F  lea ecx, [ebx+3D4h]
0x00405D85  lea ebp, ds:0[esi*4]
0x00405D8C  mov edx, [ecx-4]
0x00405D8F  mov al, [edx+18h]
0x00405D92  not al
0x00405D94  test al, 1
0x00405D96  jz loc_405DE7
0x00405D98  mov eax, [edx+1Ch]
0x00405D9B  cmp byte ptr [eax+0Ch], 0
0x00405D9F  jz loc_405DE7
0x00405DA1  fld
0x00405DA4  fstp
0x00405DA8  fld
0x00405DAC  fcom
0x00405DAE  fnstsw ax
0x00405DB0  test ah, 41h
0x00405DB3  jnz loc_405DCB
0x00405DB5  fstp
0x00405DB9  fld
0x00405DBC  fadd st, st(2)
0x00405DBE  fmul
0x00405DC1  fdiv
0x00405DC5  fstp
0x00405DC9  jmp loc_405DEF
0x00405DCB  fstp
0x00405DCD  fxch
0x00405DCF  fst
0x00405DD3  fxch
0x00405DD5  fld
0x00405DD8  fadd st, st(2)
0x00405DDA  fmul
0x00405DDD  fdiv
0x00405DE1  fstp
0x00405DE5  jmp loc_405DEF
0x00405DE7  fxch
0x00405DE9  fst
0x00405DED  fxch
0x00405DEF  fld
0x00405DF3  mov edx, [ecx]
0x00405DF5  mov al, [edx+18h]
0x00405DF8  fmul st, st(1)
0x00405DFA  not al
0x00405DFC  test al, 1
0x00405DFE  fadd
0x00405E02  fstp
0x00405E06  jz loc_405E57
0x00405E08  mov eax, [edx+1Ch]
0x00405E0B  cmp byte ptr [eax+0Ch], 0
0x00405E0F  jz loc_405E57
0x00405E11  fld
0x00405E14  fstp
0x00405E18  fld
0x00405E1C  fcom
0x00405E1E  fnstsw ax
0x00405E20  test ah, 41h
0x00405E23  jnz loc_405E3B
0x00405E25  fstp
0x00405E29  fld
0x00405E2C  fadd st, st(2)
0x00405E2E  fmul
0x00405E31  fdiv
0x00405E35  fstp
0x00405E39  jmp loc_405E5F
0x00405E3B  fstp
0x00405E3D  fxch
0x00405E3F  fst
0x00405E43  fxch
0x00405E45  fld
0x00405E48  fadd st, st(2)
0x00405E4A  fmul
0x00405E4D  fdiv
0x00405E51  fstp
0x00405E55  jmp loc_405E5F
0x00405E57  fxch
0x00405E59  fst
0x00405E5D  fxch
0x00405E5F  fld
0x00405E63  mov edx, [ecx+4]
0x00405E66  mov al, [edx+18h]
0x00405E69  fmul st, st(1)
0x00405E6B  not al
0x00405E6D  test al, 1
0x00405E6F  fadd
0x00405E73  fstp
0x00405E77  jz loc_405EC8
0x00405E79  mov eax, [edx+1Ch]
0x00405E7C  cmp byte ptr [eax+0Ch], 0
0x00405E80  jz loc_405EC8
0x00405E82  fld
0x00405E85  fstp
0x00405E89  fld
0x00405E8D  fcom
0x00405E8F  fnstsw ax
0x00405E91  test ah, 41h
0x00405E94  jnz loc_405EAC
0x00405E96  fstp
0x00405E9A  fld
0x00405E9D  fadd st, st(2)
0x00405E9F  fmul
0x00405EA2  fdiv
0x00405EA6  fstp
0x00405EAA  jmp loc_405ED0
0x00405EAC  fstp
0x00405EAE  fxch
0x00405EB0  fst
0x00405EB4  fxch
0x00405EB6  fld
0x00405EB9  fadd st, st(2)
0x00405EBB  fmul
0x00405EBE  fdiv
0x00405EC2  fstp
0x00405EC6  jmp loc_405ED0
0x00405EC8  fxch
0x00405ECA  fst
0x00405ECE  fxch
0x00405ED0  fld
0x00405ED4  mov edx, [ecx+8]
0x00405ED7  mov al, [edx+18h]
0x00405EDA  fmul st, st(1)
0x00405EDC  not al
0x00405EDE  test al, 1
0x00405EE0  fadd
0x00405EE4  fstp
0x00405EE8  jz loc_405F39
0x00405EEA  mov eax, [edx+1Ch]
0x00405EED  cmp byte ptr [eax+0Ch], 0
0x00405EF1  jz loc_405F39
0x00405EF3  fld
0x00405EF6  fstp
0x00405EFA  fld
0x00405EFE  fcom
0x00405F00  fnstsw ax
0x00405F02  test ah, 41h
0x00405F05  jnz loc_405F1D
0x00405F07  fstp
0x00405F0B  fld
0x00405F0E  fadd st, st(2)
0x00405F10  fmul
0x00405F13  fdiv
0x00405F17  fstp
0x00405F1B  jmp loc_405F41
0x00405F1D  fstp
0x00405F1F  fxch
0x00405F21  fst
0x00405F25  fxch
0x00405F27  fld
0x00405F2A  fadd st, st(2)
0x00405F2C  fmul
0x00405F2F  fdiv
0x00405F33  fstp
0x00405F37  jmp loc_405F41
0x00405F39  fxch
0x00405F3B  fst
0x00405F3F  fxch
0x00405F41  fld
0x00405F45  add ecx, 10h
0x00405F48  sub esi, 1
0x00405F4B  fmul st, st(1)
0x00405F4D  fadd
0x00405F51  fstp
0x00405F55  jnz loc_405D8C
0x00405F5B  cmp ebp, edi
0x00405F5D  jge loc_405FE6
0x00405F63  sub edi, ebp
0x00405F65  lea edx, [ebx+ebp*4+3D0h]
0x00405F6C  mov esi, edi
0x00405F6E  mov ecx, [edx]
0x00405F70  mov al, [ecx+18h]
0x00405F73  not al
0x00405F75  test al, 1
0x00405F77  jz loc_405FC8
0x00405F79  mov eax, [ecx+1Ch]
0x00405F7C  cmp byte ptr [eax+0Ch], 0
0x00405F80  jz loc_405FC8
0x00405F82  fld
0x00405F85  fstp
0x00405F89  fld
0x00405F8D  fcom
0x00405F8F  fnstsw ax
0x00405F91  test ah, 41h
0x00405F94  jnz loc_405FAC
0x00405F96  fstp
0x00405F9A  fld
0x00405F9D  fadd st, st(2)
0x00405F9F  fmul
0x00405FA2  fdiv
0x00405FA6  fstp
0x00405FAA  jmp loc_405FD0
0x00405FAC  fstp
0x00405FAE  fxch
0x00405FB0  fst
0x00405FB4  fxch
0x00405FB6  fld
0x00405FB9  fadd st, st(2)
0x00405FBB  fmul
0x00405FBE  fdiv
0x00405FC2  fstp
0x00405FC6  jmp loc_405FD0
0x00405FC8  fxch
0x00405FCA  fst
0x00405FCE  fxch
0x00405FD0  fld
0x00405FD4  add edx, 4
0x00405FD7  sub esi, 1
0x00405FDA  fmul st, st(1)
0x00405FDC  fadd
0x00405FE0  fstp
0x00405FE4  jnz loc_405F6E
0x00405FE6  fstp
0x00405FE8  pop edi
0x00405FE9  fstp
0x00405FEB  pop esi
0x00405FEC  fstp
0x00405FEE  pop ebp
0x00405FEF  fstp
0x00405FF1  fld
0x00405FF4  add esp, 8
0x00405FF7  retn
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
