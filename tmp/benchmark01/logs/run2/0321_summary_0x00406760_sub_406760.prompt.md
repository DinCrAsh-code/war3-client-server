# Function summary: sub_406760 (0x00406760)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00406760
- Name: sub_406760 (auto-generated placeholder)
- Size: 1303 bytes, 413 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_406760

## How callers use this function

### From sub_403460 (0x00403460) - call at 0x004034BD

What that caller does overall (preliminary): Iterates over a collection of simulation or resource entities (up to a count at [esi+5BBBCh], stepping by 4FCh bytes from offset 0x630), performing modulo-10 and flag-based checks on each element before invoking a batch of update/processing routines.

Called with `eax` set to `edi` (the current entity structure) during the periodic update block for the entity.

## Functions it calls or references

### sub_405AD0 [CalculateEntityStatsOrAttributes] (0x00405AD0) - call at 0x0040677D

- Prototype: `float __fastcall CalculateEntityStatsOrAttributes(void *entity)`
- Summary: Performs aggregate mathematical computations on collections of attributes or modifier pointers stored within an entity structure, iterating over arrays in chunks of 4.

### sub_4062E0 [CalculateEntityCentroidOrAverage] (0x004062E0) - call at 0x0040678C

- Prototype: `void* __stdcall CalculateEntityCentroidOrAverage(void *entity, void *outPoint)`
- Summary: Calculates the average spatial position or centroid from an entity's internal collections of points and referenced objects, storing the result in an output structure.

### sub_417FD0 [Vec3_NormalizeOrZero] (0x00417FD0) - call at 0x004067CB

- Prototype: `float __cdecl Vec3_NormalizeOrZero(float x, float y, float z)`
- Summary: Computes the Euclidean length of a 3D vector and normalizes it, returning 1/length or 0 if the length is zero or near-zero.

### sub_406540 [FindNearestValidEntityOrTarget] (0x00406540) - call at 0x004068F7, 0x00406B21

- Prototype: `bool __stdcall FindNearestValidEntityOrTarget(void *entity, void *outTarget)`
- Summary: Scans entity lists and grid structures to find a valid target or entity meeting specific distance and filtering criteria relative to the given entity.

### sub_404880 [CUnit::BuildOrSerializeCommandBatch] (0x00404880) - call at 0x00406920, 0x00406989, 0x004069F5, 0x00406A6D, 0x00406A87, 0x00406B5D, 0x00406B96, 0x00406C3A (+1 more)

- Prototype: `void __stdcall CUnit::BuildOrSerializeCommandBatch(int arg_0, int arg_4)`
- Summary: Gathers selected entities or unit references from an entity's internal lists, builds a local command/target array, and serializes them using a helper routine.

### sub_408610 [CEntity::FindBestTargetOrNode] (0x00408610) - call at 0x00406957

- Prototype: `int __stdcall CEntity::FindBestTargetOrNode(void *entity_ptr, int arg0, int arg1)`
- Summary: Iterates through an entity's internal list of potential pathfinding/target nodes or entries, validates spatial/state constraints, computes distances, and returns the index of the best match or -1 if none qualifies.

### _CIsqrt (0x0041A212) - call at 0x004069D0, 0x00406BD2

- Summary: Import thunk: jumps straight to the imported API MSVCR80!_CIsqrt.

### sub_406000 [CalculateEntityProximityValue] (0x00406000) - call at 0x00406A2F, 0x00406AA6

- Prototype: `float __stdcall CalculateEntityProximityValue(void *entity, void *grid_data)`
- Summary: Queries nearby entities using a spatial grid and a criteria filter, then loops over the results to accumulate a proximity or distance-based value returned in st(0).

### sub_417F80 [CalculateDistance] (0x00417F80) - call at 0x00406B00

- Prototype: `double __cdecl CalculateDistance(float x1, float y1, float x2, float y2)`
- Summary: Computes the Euclidean distance between two 2D points (x1, y1) and (x2, y2).

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00406760  push ebp
0x00406761  mov ebp, esp
0x00406763  and esp, 0FFFFFFF8h
0x00406766  sub esp, 2Ch
0x00406769  push ebx
0x0040676A  mov ebx, eax
0x0040676C  mov eax, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406771  fld
0x00406777  push esi
0x00406778  push edi
0x00406779  fstp
0x0040677D  call sub_405AD0                              ; -> sub_405AD0 [CalculateEntityStatsOrAttributes]
0x00406782  fstp
0x00406786  lea ecx, [esp+38h+var_18]
0x0040678A  push ecx
0x0040678B  push ebx
0x0040678C  call sub_4062E0                              ; -> sub_4062E0 [CalculateEntityCentroidOrAverage]
0x00406791  fld                                          ; -> 0x0041F024 in .data
0x00406797  fmul                                         ; -> 0x0041D8C8 in .rdata
0x0040679D  lea edi, [esp+38h+var_24]
0x004067A1  lea esi, [esp+38h+var_8]
0x004067A5  fstp
0x004067A9  fld
0x004067AD  fst
0x004067B1  fstp
0x004067B5  fld
0x004067B9  fsub
0x004067BC  fstp
0x004067C0  fld
0x004067C4  fsub
0x004067C7  fstp
0x004067CB  call sub_417FD0                              ; -> sub_417FD0 [Vec3_NormalizeOrZero]
0x004067D0  fld
0x004067D2  mov [esp+38h+var_2A], 1
0x004067D7  fld                                          ; -> 0x0041DC00 in .rdata
0x004067DD  fmul st(1), st
0x004067DF  fxch
0x004067E1  fstp
0x004067E5  fmul
0x004067E8  fstp
0x004067EC  fld
0x004067EF  fadd
0x004067F3  fstp
0x004067F7  fld
0x004067FA  fadd
0x004067FE  fstp
0x00406802  fld
0x00406806  fld
0x00406808  fsub
0x0040680B  fcomp                                        ; -> 0x0041DBF8 in .rdata
0x00406811  fnstsw ax
0x00406813  test ah, 41h
0x00406816  jz loc_40681D
0x00406818  mov [esp+38h+var_2A], 0
0x0040681D  cmp byte ptr [ebx+1Ch], 0
0x00406821  jz loc_406837
0x00406823  fsub
0x00406826  fcomp                                        ; -> 0x0041DA08 in .rdata
0x0040682C  fnstsw ax
0x0040682E  test ah, 5
0x00406831  jp loc_406839
0x00406833  mov cl, 1
0x00406835  jmp loc_40683E
0x00406837  fstp
0x00406839  xor cl, cl
0x0040683B  mov [ebx+1Ch], cl
0x0040683E  xor edx, edx
0x00406840  cmp [ebx+3DCh], edx
0x00406846  mov [esp+38h+var_24], edx
0x0040684A  mov [esp+38h+var_29], dl
0x0040684E  jle loc_40688B
0x00406850  mov edi, [ebx+3DCh]
0x00406856  lea esi, [ebx+3D0h]
0x0040685C  lea esp, [esp+0]
0x00406860  mov eax, [esi]
0x00406862  mov dl, [eax+18h]
0x00406865  not dl
0x00406867  test dl, 1
0x0040686A  jz loc_406881
0x0040686C  mov eax, [eax+250h]
0x00406872  cmp [esp+38h+var_24], eax
0x00406876  mov [esp+38h+var_29], 1
0x0040687B  jg loc_406881
0x0040687D  mov [esp+38h+var_24], eax
0x00406881  add esi, 4
0x00406884  sub edi, 1
0x00406887  jnz loc_406860
0x00406889  xor edx, edx
0x0040688B  test cl, cl
0x0040688D  fld                                          ; -> 0x0041D998 in .rdata
0x00406893  jz loc_4068BD
0x00406895  mov eax, [ebx+0Ch]
0x00406898  cmp eax, 4
0x0040689B  jz loc_4068BD
0x0040689D  cmp eax, 2
0x004068A0  jz loc_4068BD
0x004068A2  mov esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x004068A8  mov dword ptr [ebx+0Ch], 4
0x004068AF  fld
0x004068B5  fstp
0x004068B8  fst
0x004068BB  jmp loc_4068C3
0x004068BD  mov esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x004068C3  mov eax, [ebx+0Ch]
0x004068C6  cmp eax, 4
0x004068C9  ja def_4068CF
0x004068CF  jmp ds:jpt_4068CF[eax*4]                     ; -> 0x00406C78 in .text
0x004068D6  mov eax, [ebx+3Ch]
0x004068D9  fstp
0x004068DB  cmp eax, edx
0x004068DD  jle loc_40692C
0x004068DF  cmp [ebx+3CCh], eax
0x004068E5  jl loc_40692C
0x004068E7  cmp [esp+38h+var_29], 0
0x004068EC  jz loc_406995
0x004068F2  lea edi, [ebx+2Ch]
0x004068F5  push edi
0x004068F6  push ebx
0x004068F7  call sub_406540                              ; -> sub_406540 [FindNearestValidEntityOrTarget]
0x004068FC  test al, al
0x004068FE  jz loc_40692C
0x00406900  add dword ptr [ebx+14h], 1
0x00406904  mov dword ptr [ebx+0Ch], 2
0x0040690B  fld
0x00406911  fstp
0x00406914  push edi
0x00406915  fld                                          ; -> 0x0041D998 in .rdata
0x0040691B  push 5
0x0040691D  fstp
0x00406920  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x00406925  pop edi
0x00406926  pop esi
0x00406927  pop ebx
0x00406928  mov esp, ebp
0x0040692A  pop ebp
0x0040692B  retn
0x0040692C  cmp [esp+38h+var_29], 0
0x00406931  jz loc_406995
0x00406933  cmp dword ptr [ebx+3CCh], 4
0x0040693A  jl loc_406995
0x0040693C  mov eax, [esp+38h+var_24]
0x00406940  fld
0x00406944  add eax, 2
0x00406947  push eax
0x00406948  push ecx
0x00406949  lea ecx, [esp+40h+var_18]
0x0040694D  fstp
0x00406950  push ecx
0x00406951  lea eax, [esi+571D4h]
0x00406957  call sub_408610                              ; -> sub_408610 [CEntity::FindBestTargetOrNode]
0x0040695C  test eax, eax
0x0040695E  jl loc_406995
0x00406960  mov [ebx+18h], eax
0x00406963  lea edx, [eax+eax*8]
0x00406966  mov dword ptr [ebx+0Ch], 1
0x0040696D  fld
0x00406973  lea eax, [esi+edx*4+571D4h]
0x0040697A  fstp
0x0040697D  fld                                          ; -> 0x0041D998 in .rdata
0x00406983  push eax
0x00406984  push 5
0x00406986  fstp
0x00406989  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x0040698E  pop edi
0x0040698F  pop esi
0x00406990  pop ebx
0x00406991  mov esp, ebp
0x00406993  pop ebp
0x00406994  retn
0x00406995  cmp [esp+38h+var_2A], 0
0x0040699A  jz loc_406C70
0x004069A0  fld
0x004069A4  fsub
0x004069A8  fstp
0x004069AC  fld
0x004069B0  fsub
0x004069B4  fstp
0x004069B8  fld
0x004069BC  fld
0x004069C0  fmul st, st
0x004069C2  fld
0x004069C4  fmulp st(2), st
0x004069C6  faddp st(1), st
0x004069C8  fstp
0x004069CC  fld
0x004069D0  call _CIsqrt                                 ; -> _CIsqrt
0x004069D5  fstp
0x004069D9  fld
0x004069DD  fcomp                                        ; -> 0x0041DBF0 in .rdata
0x004069E3  fnstsw ax
0x004069E5  test ah, 41h
0x004069E8  jnz loc_406C70
0x004069EE  lea ecx, [esp+38h+var_10]
0x004069F2  push ecx
0x004069F3  push 5
0x004069F5  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x004069FA  pop edi
0x004069FB  pop esi
0x004069FC  pop ebx
0x004069FD  mov esp, ebp
0x004069FF  pop ebp
0x00406A00  retn
0x00406A01  mov eax, [ebx+18h]
0x00406A04  lea eax, [eax+eax*8]
0x00406A07  cmp byte ptr [esi+eax*4+571DEh], 0
0x00406A0F  lea edi, [esi+eax*4+571D4h]
0x00406A16  jz def_4068CF
0x00406A1C  fstp
0x00406A1E  push ecx
0x00406A1F  fld                                          ; -> 0x0041D2B8 in .rdata
0x00406A25  lea ecx, [esp+3Ch+var_18]
0x00406A29  fstp
0x00406A2C  push ecx
0x00406A2D  mov eax, ebx
0x00406A2F  call sub_406000                              ; -> sub_406000 [CalculateEntityProximityValue]
0x00406A34  fld
0x00406A38  fmul                                         ; -> 0x0041DAB8 in .rdata
0x00406A3E  fcompp
0x00406A40  fnstsw ax
0x00406A42  test ah, 5
0x00406A45  jp loc_406A79
0x00406A47  mov edx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406A4D  mov dword ptr [ebx+0Ch], 3
0x00406A54  fld
0x00406A5A  lea eax, [esp+38h+var_10]
0x00406A5E  fstp
0x00406A61  fld                                          ; -> 0x0041D998 in .rdata
0x00406A67  push eax
0x00406A68  push 3
0x00406A6A  fstp
0x00406A6D  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x00406A72  pop edi
0x00406A73  pop esi
0x00406A74  pop ebx
0x00406A75  mov esp, ebp
0x00406A77  pop ebp
0x00406A78  retn
0x00406A79  cmp [esp+38h+var_2A], 0
0x00406A7E  jz loc_406C70
0x00406A84  push edi
0x00406A85  push 5
0x00406A87  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x00406A8C  pop edi
0x00406A8D  pop esi
0x00406A8E  pop ebx
0x00406A8F  mov esp, ebp
0x00406A91  pop ebp
0x00406A92  retn
0x00406A93  fstp
0x00406A95  push ecx
0x00406A96  fld                                          ; -> 0x0041D2B8 in .rdata
0x00406A9C  lea ecx, [esp+3Ch+var_18]
0x00406AA0  fstp
0x00406AA3  push ecx
0x00406AA4  mov eax, ebx
0x00406AA6  call sub_406000                              ; -> sub_406000 [CalculateEntityProximityValue]
0x00406AAB  fstp
0x00406AAF  fld
0x00406AB3  fld
0x00406AB7  fmul                                         ; -> 0x0041DAB8 in .rdata
0x00406ABD  fcompp
0x00406ABF  fnstsw ax
0x00406AC1  test ah, 5
0x00406AC4  jp loc_406ADE
0x00406AC6  fld
0x00406ACA  fsub
0x00406ACD  fcomp                                        ; -> 0x0041DB60 in .rdata
0x00406AD3  fnstsw ax
0x00406AD5  test ah, 41h
0x00406AD8  jz loc_406B70
0x00406ADE  mov eax, [ebx+3CCh]
0x00406AE4  lea edx, [eax+eax*2]
0x00406AE7  cmp edx, [ebx+3Ch]
0x00406AEA  jl loc_406B70
0x00406AF0  cmp [esp+38h+var_29], 0
0x00406AF5  jz loc_406B70
0x00406AF7  lea esi, [ebx+2Ch]
0x00406AFA  mov eax, esi
0x00406AFC  lea ecx, [esp+38h+var_18]
0x00406B00  call sub_417F80                              ; -> sub_417F80 [CalculateDistance]
0x00406B05  fcomp                                        ; -> 0x0041DBF0 in .rdata
0x00406B0B  fnstsw ax
0x00406B0D  test ah, 5
0x00406B10  jp loc_406B4F
0x00406B12  fldz
0x00406B14  fcomp
0x00406B18  fnstsw ax
0x00406B1A  test ah, 1
0x00406B1D  jnz loc_406B4F
0x00406B1F  push esi
0x00406B20  push ebx
0x00406B21  call sub_406540                              ; -> sub_406540 [FindNearestValidEntityOrTarget]
0x00406B26  test al, al
0x00406B28  jnz loc_406B5A
0x00406B2A  mov eax, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406B2F  mov dword ptr [ebx+0Ch], 0
0x00406B36  fld
0x00406B3C  fstp
0x00406B3F  fld                                          ; -> 0x0041D998 in .rdata
0x00406B45  fstp
0x00406B48  pop edi
0x00406B49  pop esi
0x00406B4A  pop ebx
0x00406B4B  mov esp, ebp
0x00406B4D  pop ebp
0x00406B4E  retn
0x00406B4F  cmp [esp+38h+var_2A], 0
0x00406B54  jz loc_406C70
0x00406B5A  push esi
0x00406B5B  push 5
0x00406B5D  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x00406B62  fld
0x00406B66  fstp
0x00406B69  pop edi
0x00406B6A  pop esi
0x00406B6B  pop ebx
0x00406B6C  mov esp, ebp
0x00406B6E  pop ebp
0x00406B6F  retn
0x00406B70  mov ecx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406B76  mov dword ptr [ebx+0Ch], 3
0x00406B7D  fld
0x00406B83  lea edx, [esp+38h+var_10]
0x00406B87  fstp
0x00406B8A  fld                                          ; -> 0x0041D998 in .rdata
0x00406B90  push edx
0x00406B91  push 3
0x00406B93  fstp
0x00406B96  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x00406B9B  pop edi
0x00406B9C  pop esi
0x00406B9D  pop ebx
0x00406B9E  mov esp, ebp
0x00406BA0  pop ebp
0x00406BA1  retn
0x00406BA2  fstp
0x00406BA4  fld
0x00406BA8  fsub
0x00406BAB  fstp
0x00406BAF  fld
0x00406BB3  fsub
0x00406BB6  fstp
0x00406BBA  fld
0x00406BBE  fld
0x00406BC2  fmul st, st
0x00406BC4  fld
0x00406BC6  fmulp st(2), st
0x00406BC8  faddp st(1), st
0x00406BCA  fstp
0x00406BCE  fld
0x00406BD2  call _CIsqrt                                 ; -> _CIsqrt
0x00406BD7  fstp
0x00406BDB  fld
0x00406BDF  fcomp                                        ; -> 0x0041DB9C in .rdata
0x00406BE5  fnstsw ax
0x00406BE7  test ah, 5
0x00406BEA  jnp loc_406C00
0x00406BEC  fld
0x00406BF0  fsub
0x00406BF3  fcomp                                        ; -> 0x0041DA78 in .rdata
0x00406BF9  fnstsw ax
0x00406BFB  test ah, 41h
0x00406BFE  jnz loc_406C70
0x00406C00  mov dword ptr [ebx+0Ch], 0
0x00406C07  fld
0x00406C0D  fstp
0x00406C10  fld                                          ; -> 0x0041D998 in .rdata
0x00406C16  fstp
0x00406C19  pop edi
0x00406C1A  pop esi
0x00406C1B  pop ebx
0x00406C1C  mov esp, ebp
0x00406C1E  pop ebp
0x00406C1F  retn
0x00406C20  test cl, cl
0x00406C22  jnz loc_406C46
0x00406C24  mov [ebx+0Ch], edx
0x00406C27  fld
0x00406C2D  lea eax, [esp+38h+var_10]
0x00406C31  fstp
0x00406C34  push eax
0x00406C35  push 3
0x00406C37  fstp
0x00406C3A  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x00406C3F  pop edi
0x00406C40  pop esi
0x00406C41  pop ebx
0x00406C42  mov esp, ebp
0x00406C44  pop ebp
0x00406C45  retn
0x00406C46  cmp [esp+38h+var_2A], 0
0x00406C4B  fstp
0x00406C4D  jz loc_406C70
0x00406C4F  lea ecx, [ebx+20h]
0x00406C52  push ecx
0x00406C53  push 5
0x00406C55  call sub_404880                              ; -> sub_404880 [CUnit::BuildOrSerializeCommandBatch]
0x00406C5A  pop edi
0x00406C5B  pop esi
0x00406C5C  pop ebx
0x00406C5D  mov esp, ebp
0x00406C5F  pop ebp
0x00406C60  retn
0x00406C61  mov [ebx+0Ch], edx
0x00406C64  fld
0x00406C6A  fstp
0x00406C6D  fstp
0x00406C70  pop edi
0x00406C71  pop esi
0x00406C72  pop ebx
0x00406C73  mov esp, ebp
0x00406C75  pop ebp
0x00406C76  retn
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
