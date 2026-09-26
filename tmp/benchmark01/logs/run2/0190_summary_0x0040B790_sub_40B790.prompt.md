# Function summary: sub_40B790 (0x0040B790)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x0040B790
- Name: sub_40B790 (auto-generated placeholder)
- Size: 826 bytes, 253 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402050 -> sub_402560 -> sub_40B790

## How callers use this function

### From sub_402560 (0x00402560) - call at 0x00402722

What that caller does overall (preliminary): Factory and initialization routine that allocates, configures, and registers resource sub-elements (such as graphics or state structures) based on lookup table flags and index criteria.

Performs initialization or registration on the newly created resource block (passed via ebx) before it is added to internal arrays.

## Functions it calls or references

### sub_404460 [LookupResourceProperty] (0x00404460) - call at 0x0040B805

- Prototype: `float __stdcall LookupResourceProperty(int arg_0)`
- Summary: Searches a static configuration table in read-only data against properties of a given resource structure to compute or retrieve a floating-point resource parameter.

### sub_4043B0 [EvaluateResourceProperty] (0x004043B0) - call at 0x0040B8B2

- Prototype: `float __cdecl EvaluateResourceProperty(void *resourceBlock)`
- Summary: Evaluates a resource block's configuration data and lookup tables to compute a scaling factor or dimension, returning the resulting floating-point value.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x0040B790  fldz
0x0040B792  sub esp, 48h
0x0040B795  fcom
0x0040B798  fnstsw ax
0x0040B79A  test ah, 5
0x0040B79D  jp loc_40B7A7
0x0040B79F  fld
0x0040B7A2  fdiv
0x0040B7A5  jmp loc_40B7A9
0x0040B7A7  fld1
0x0040B7A9  fstp
0x0040B7AD  fcomp
0x0040B7B0  fnstsw ax
0x0040B7B2  test ah, 5
0x0040B7B5  jp loc_40B7BF
0x0040B7B7  fld
0x0040B7BA  fdiv
0x0040B7BD  jmp loc_40B7C5
0x0040B7BF  fld                                          ; -> 0x0041D8C8 in .rdata
0x0040B7C5  push ebp
0x0040B7C6  fstp
0x0040B7CA  mov ebp, dword_42AC60                        ; -> 0x0042AC60 in .data
0x0040B7D0  push esi
0x0040B7D1  mov esi, [ebx+1Ch]
0x0040B7D4  movzx eax, word ptr [esi+4]
0x0040B7D8  push edi
0x0040B7D9  movzx edi, byte ptr [ebx+1Bh]
0x0040B7DD  mov [esp+54h+var_48], eax
0x0040B7E1  imul edi
0x0040B7E7  fild
0x0040B7EB  fmul
0x0040B7EE  fstp
0x0040B7F2  movzx ecx, word ptr [esi+6]
0x0040B7F6  mov [esp+54h+var_48], ecx
0x0040B7FA  add edi, ebp
0x0040B7FC  push edi
0x0040B7FD  fild
0x0040B801  fstp
0x0040B805  call sub_404460                              ; -> sub_404460 [LookupResourceProperty]
0x0040B80A  movsx edx, word ptr [esi+8]
0x0040B80E  mov [esp+54h+var_48], edx
0x0040B812  push esi
0x0040B813  fild
0x0040B817  fmul                                         ; -> 0x0041D8E0 in .rdata
0x0040B81D  faddp st(1), st
0x0040B81F  fstp
0x0040B823  movzx eax, word ptr [esi+10h]
0x0040B827  mov [esp+58h+var_48], eax
0x0040B82B  fild
0x0040B82F  fmul
0x0040B832  fstp
0x0040B836  movzx ecx, byte ptr [esi+0Dh]
0x0040B83A  mov [esp+58h+var_2C], ecx
0x0040B83E  movzx edx, byte ptr [esi+0Eh]
0x0040B842  mov [esp+58h+var_28], edx
0x0040B846  movzx eax, word ptr [esi+12h]
0x0040B84A  mov [esp+58h+var_48], eax
0x0040B84E  fild
0x0040B852  fmul                                         ; -> 0x0041DAA8 in .rdata
0x0040B858  fstp
0x0040B85C  fld1
0x0040B85E  fst
0x0040B862  movzx ecx, word ptr [esi+1Eh]
0x0040B866  mov [esp+58h+var_48], ecx
0x0040B86A  fild
0x0040B86E  fstp
0x0040B872  movzx edx, word ptr [esi+18h]
0x0040B876  mov [esp+58h+var_48], edx
0x0040B87A  fild
0x0040B87E  fstp
0x0040B882  movzx eax, word ptr [esi+30h]
0x0040B886  mov [esp+58h+var_48], eax
0x0040B88A  fild
0x0040B88E  fld                                          ; -> 0x0041DA58 in .rdata
0x0040B894  fmul st(1), st
0x0040B896  fxch
0x0040B898  fstp
0x0040B89C  movzx ecx, word ptr [esi+32h]
0x0040B8A0  mov [esp+58h+var_48], ecx
0x0040B8A4  fild
0x0040B8A8  fmulp st(1), st
0x0040B8AA  fstp
0x0040B8AE  fstp
0x0040B8B2  call sub_4043B0                              ; -> sub_4043B0 [EvaluateResourceProperty]
0x0040B8B7  fstp
0x0040B8BB  fld                                          ; -> 0x0041D2BC in .rdata
0x0040B8C1  fcomp
0x0040B8C7  fnstsw ax
0x0040B8C9  test ah, 41h
0x0040B8CC  jz loc_40B8E1
0x0040B8CE  fld                                          ; -> 0x0041D8F4 in .rdata
0x0040B8D4  fcomp
0x0040B8DA  fnstsw ax
0x0040B8DC  test ah, 41h
0x0040B8DF  jp loc_40B8E7
0x0040B8E1  movzx eax, word ptr [esi+26h]
0x0040B8E5  jmp loc_40B8EB
0x0040B8E7  movzx eax, word ptr [esi+24h]
0x0040B8EB  movzx edx, ax
0x0040B8EE  mov [esp+54h+var_48], edx
0x0040B8F2  fild
0x0040B8F6  fstp
0x0040B8FA  mov cl, [esi+2]
0x0040B8FD  cmp cl, 1
0x0040B900  ja loc_40B973
0x0040B902  xor ebp, ebp
0x0040B904  mov eax, offset unk_41CB18                   ; -> 0x0041CB18 in .rdata
0x0040B909  mov dl, 3
0x0040B90B  jmp loc_40B910
0x0040B910  cmp [eax], cl
0x0040B912  jnz loc_40B919
0x0040B914  cmp [eax+1], dl
0x0040B917  jz loc_40B928
0x0040B919  add eax, 10h
0x0040B91C  add ebp, 1
0x0040B91F  cmp eax, offset flt_41CB98                   ; -> 0x0041CB98 in .rdata
0x0040B924  jl loc_40B910
0x0040B926  jmp loc_40B973
0x0040B928  cmp [esi+3Bh], dl
0x0040B92B  jnz loc_40B973
0x0040B92D  movzx eax, byte ptr [edi+ebp+0D8h]
0x0040B935  test eax, eax
0x0040B937  mov [esp+54h+var_48], eax
0x0040B93B  jle loc_40B973
0x0040B93D  fild
0x0040B941  fstp
0x0040B945  fld
0x0040B949  fld                                          ; -> 0x0041DAA0 in .rdata
0x0040B94F  fmul st, st(1)
0x0040B951  fadd
0x0040B955  fstp
0x0040B959  fld                                          ; -> 0x0041DA98 in .rdata
0x0040B95F  fmul st, st(1)
0x0040B961  fadd
0x0040B965  fstp
0x0040B969  fadd st, st
0x0040B96B  fadd
0x0040B96F  fstp
0x0040B973  mov eax, [ebx]
0x0040B975  mov edx, [eax+1Ch]
0x0040B978  lea ecx, [esp+54h+var_3C]
0x0040B97C  push ecx
0x0040B97D  mov ecx, ebx
0x0040B97F  call edx
0x0040B981  mov edi, [ebx+20Ch]
0x0040B987  test edi, edi
0x0040B989  jz loc_40B9AB
0x0040B98B  jmp loc_40B990
0x0040B990  cmp byte ptr [edi+0Eh], 0
0x0040B994  jnz loc_40B9A4
0x0040B996  mov eax, [edi]
0x0040B998  mov edx, [eax+10h]
0x0040B99B  lea ecx, [esp+54h+var_3C]
0x0040B99F  push ecx
0x0040B9A0  mov ecx, edi
0x0040B9A2  call edx
0x0040B9A4  mov edi, [edi+1Ch]
0x0040B9A7  test edi, edi
0x0040B9A9  jnz loc_40B990
0x0040B9AB  fld
0x0040B9AF  fcom                                         ; -> 0x0041DA90 in .rdata
0x0040B9B5  fnstsw ax
0x0040B9B7  test ah, 5
0x0040B9BA  jp loc_40B9CA
0x0040B9BC  fstp
0x0040B9BE  fld                                          ; -> 0x0041DA8C in .rdata
0x0040B9C4  fstp
0x0040B9C8  jmp loc_40B9E3
0x0040B9CA  fld                                          ; -> 0x0041CF30 in .rdata
0x0040B9D0  fcom
0x0040B9D2  fnstsw ax
0x0040B9D4  fstp
0x0040B9D6  test ah, 5
0x0040B9D9  jp loc_40B9E1
0x0040B9DB  fstp
0x0040B9DF  jmp loc_40B9E3
0x0040B9E1  fstp
0x0040B9E3  cmp word ptr [esi+1Eh], 0
0x0040B9E8  jbe loc_40BA22
0x0040B9EA  fld
0x0040B9EE  fcom                                         ; -> 0x0041DAA0 in .rdata
0x0040B9F4  fnstsw ax
0x0040B9F6  test ah, 5
0x0040B9F9  jp loc_40BA09
0x0040B9FB  fstp
0x0040B9FD  fld                                          ; -> 0x0041DA54 in .rdata
0x0040BA03  fstp
0x0040BA07  jmp loc_40BA22
0x0040BA09  fld                                          ; -> 0x0041DA88 in .rdata
0x0040BA0F  fcom
0x0040BA11  fnstsw ax
0x0040BA13  fstp
0x0040BA15  test ah, 5
0x0040BA18  jp loc_40BA20
0x0040BA1A  fstp
0x0040BA1E  jmp loc_40BA22
0x0040BA20  fstp
0x0040BA22  fld1
0x0040BA24  fld
0x0040BA28  fcom
0x0040BA2A  fnstsw ax
0x0040BA2C  test ah, 5
0x0040BA2F  jp loc_40BA3B
0x0040BA31  fstp
0x0040BA33  fst
0x0040BA37  fld
0x0040BA3B  mov al, [ebx+18h]
0x0040BA3E  fst
0x0040BA41  lea edi, [ebx+20h]
0x0040BA44  mov ecx, 0Fh
0x0040BA49  lea esi, [esp+54h+var_3C]
0x0040BA4D  movsd
0x0040BA4F  pop edi
0x0040BA50  not al
0x0040BA52  test al, 1
0x0040BA54  pop esi
0x0040BA55  pop ebp
0x0040BA56  jz loc_40BAB1
0x0040BA58  fmul
0x0040BA5C  fstp
0x0040BA60  fld
0x0040BA64  fcom                                         ; -> 0x0041D980 in .rdata
0x0040BA6A  fnstsw ax
0x0040BA6C  test ah, 5
0x0040BA6F  jp loc_40BA91
0x0040BA71  and word ptr [ebx+18h], 0FBFFh
0x0040BA77  fstp
0x0040BA79  fstp
0x0040BA7C  fld
0x0040BA7F  fstp
0x0040BA82  fld
0x0040BA86  fmul
0x0040BA8A  fstp
0x0040BA8D  add esp, 48h
0x0040BA90  retn
0x0040BA91  and word ptr [ebx+18h], 0FBFFh
0x0040BA97  fstp
0x0040BA99  fstp
0x0040BA9C  fld
0x0040BA9F  fstp
0x0040BAA2  fld
0x0040BAA6  fmul
0x0040BAAA  fstp
0x0040BAAD  add esp, 48h
0x0040BAB0  retn
0x0040BAB1  and word ptr [ebx+18h], 0FBFFh
0x0040BAB7  fstp
0x0040BAB9  fstp
0x0040BABB  fld
0x0040BABF  fmul
0x0040BAC3  fstp
0x0040BAC6  add esp, 48h
0x0040BAC9  retn
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
