# Function summary: sub_405520 (0x00405520)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00405520
- Name: sub_405520 (auto-generated placeholder)
- Size: 1455 bytes, 427 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404AD0 -> sub_405300 -> sub_405520

## How callers use this function

### From sub_404EA0 (0x00404EA0) - call at 0x00404F35

What that caller does overall (preliminary): Interprets and executes a specific command or bytecode instruction from a configuration stream (pointed to by ebx) acting on an entity/object state (pointed to by edi), validating various bounds and state conditions via a switch statement before dispatching sub-operations.

Called to perform a conditional check or complex operation on the entity, passing the entity pointer, an index from ebx+1, and a pointer to a local stack variable whose value is populated during the call.

### From sub_405300 (0x00405300) - call at 0x0040537F

What that caller does overall (preliminary): Performs validation checks on an entity's state, timing, and resource requirements against global configuration limits before executing a state or action transition.

Called to check whether the entity (passed in edi) meets specific capability or prerequisite criteria identified by index esi, using an argument from the caller and a local work variable buffer (ptr at esp+var_8). Aborts if it returns false (al == 0).

## Functions it calls or references

### sub_402E50 [FindNearestValidEntityInRadius] (0x00402E50) - call at 0x004055B7

- Prototype: `void* __stdcall FindNearestValidEntityInRadius(float x, float y)`
- Summary: Iterates through a global list of entities, filtering by flags, health, and a squared distance radius, and returns a pointer to the closest matching entity (or null if none found).

### _CIsqrt (0x0041A212) - call at 0x00405674, 0x004056CE, 0x00405955, 0x004059B5

- Summary: Import thunk: jumps straight to the imported API MSVCR80!_CIsqrt.

### sub_417FD0 [Vec3_NormalizeOrZero] (0x00417FD0) - call at 0x0040577D

- Prototype: `float __cdecl Vec3_NormalizeOrZero(float x, float y, float z)`
- Summary: Computes the Euclidean length of a 3D vector and normalizes it, returning 1/length or 0 if the length is zero or near-zero.

### __ftol2_sse (0x0041A160) - call at 0x0040579A, 0x004057D9

- Prototype: `int __cdecl __ftol2_sse(void)`
- Summary: Converts the top-of-stack x87 floating-point value to a 32-bit signed integer using SSE or fallback FPU routines, returning the integer result in EAX.

### sub_414CE0 [CheckGridAreaPassesCriteria] (0x00414CE0) - call at 0x004058C1

- Prototype: `bool __fastcall CheckGridAction(void *pDataStruct, int edx_val, int ecx_val, int arg0)`
- Summary: Tests whether a 2D rectangular grid region within a structured data array meets specific validity and value criteria, returning true if all checked elements satisfy the conditions.

### sub_4182F0 [IntersectRaySegmentOrLine] (0x004182F0) - call at 0x00405920

- Prototype: `Vector2* __fastcall IntersectRaySegmentOrLine(void *this, void *edx_buffer)`
- Summary: Performs a 2D geometric intersection or closest-point/projection calculation (likely between a ray/line segment and a point or other geometric primitive), populating a coordinate buffer and returning its pointer.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00405520  push ebp
0x00405521  mov ebp, esp
0x00405523  and esp, 0FFFFFFF8h
0x00405526  sub esp, 7Ch
0x00405529  cmp eax, 30h
0x0040552C  push ebx
0x0040552D  push esi
0x0040552E  push edi
0x0040552F  ja loc_405545
0x00405531  mov ecx, eax
0x00405533  shl ecx, 4
0x00405536  sub ecx, eax
0x00405538  lea edx, ds:41B178h[ecx*4]                   ; -> 0x0041B178 in .rdata
0x0040553F  mov [esp+88h+var_60], edx
0x00405543  jmp loc_40554D
0x00405545  mov [esp+88h+var_60], offset byte_41B178     ; -> 0x0041B178 in .rdata
0x0040554D  fld                                          ; -> 0x0041F024 in .data
0x00405553  mov ecx, [esp+88h+var_60]
0x00405557  fmul                                         ; -> 0x0041D8C8 in .rdata
0x0040555D  mov ebx, [ebp+arg_0]
0x00405560  movzx edx, byte ptr [ecx+0Fh]
0x00405564  mov ecx, [ebx+38h]
0x00405567  mov esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x0040556D  fstp
0x00405571  fld
0x00405575  add ebx, 34h
0x00405578  fst
0x0040557C  lea eax, [esi+1A20h]
0x00405582  fstp
0x00405586  push ecx
0x00405587  fld                                          ; -> 0x0041D2B8 in .rdata
0x0040558D  mov [esp+8Ch+var_38], eax
0x00405591  mov eax, [ebx]
0x00405593  fstp
0x00405596  push ebx
0x00405597  mov [esp+90h+var_64], edx
0x0040559B  mov [esp+90h+var_44], ebx
0x0040559F  mov [esp+90h+var_28], eax
0x004055A3  mov [esp+90h+var_24], ecx
0x004055A7  mov [esp+90h+var_4C], 4
0x004055AF  mov [esp+90h+var_54], 12h
0x004055B7  call sub_402E50                              ; -> sub_402E50 [FindNearestValidEntityInRadius]
0x004055BC  cmp [ebp+arg_4], 0
0x004055C0  mov [esp+88h+var_48], eax
0x004055C4  jz loc_40574E
0x004055CA  mov ecx, [esi+57390h]
0x004055D0  fld                                          ; -> 0x0041D9C0 in .rdata
0x004055D6  xor eax, eax
0x004055D8  fstp
0x004055DC  cmp ecx, eax
0x004055DE  mov [esp+88h+var_5C], eax
0x004055E2  mov [esp+88h+var_54], eax
0x004055E6  mov [esp+88h+var_58], ecx
0x004055EA  jle loc_405AA7
0x004055F0  mov edx, [esi+5738Ch]
0x004055F6  mov [esp+88h+var_48], edx
0x004055FA  jmp loc_405604
0x00405600  mov edx, [esp+88h+var_48]
0x00405604  mov edi, [edx+eax*4]
0x00405607  mov eax, [edi+1Ch]
0x0040560A  test byte ptr [eax], 80h
0x0040560D  jz loc_405703
0x00405613  mov al, [edi+18h]
0x00405616  not al
0x00405618  test al, 1
0x0040561A  jz loc_405703
0x00405620  xor esi, esi
0x00405622  test ecx, ecx
0x00405624  mov [esp+88h+var_75], 0
0x00405629  jle loc_4056A3
0x0040562B  mov ebx, edx
0x0040562D  cmp [esp+88h+var_75], 0
0x00405632  jnz loc_40569F
0x00405634  mov eax, [ebx]
0x00405636  mov cl, [eax+18h]
0x00405639  not cl
0x0040563B  test cl, 1
0x0040563E  jz loc_405693
0x00405640  mov edx, [eax+1Ch]
0x00405643  test byte ptr [edx], 40h
0x00405646  jz loc_405693
0x00405648  fld
0x0040564B  fsub
0x0040564E  fstp
0x00405652  fld
0x00405655  fsub
0x00405658  fstp
0x0040565C  fld
0x00405660  fld
0x00405664  fmul st, st
0x00405666  fld
0x00405668  fmulp st(2), st
0x0040566A  faddp st(1), st
0x0040566C  fstp
0x00405670  fld
0x00405674  call _CIsqrt                                 ; -> _CIsqrt
0x00405679  fstp
0x0040567D  fld
0x00405681  fcomp                                        ; -> 0x0041DAD0 in .rdata
0x00405687  fnstsw ax
0x00405689  test ah, 5
0x0040568C  jp loc_405693
0x0040568E  mov [esp+88h+var_75], 1
0x00405693  add esi, 1
0x00405696  add ebx, 4
0x00405699  cmp esi, [esp+88h+var_58]
0x0040569D  jl loc_40562D
0x0040569F  mov ebx, [esp+88h+var_44]
0x004056A3  fld
0x004056A6  fsub
0x004056A8  fstp
0x004056AC  fld
0x004056AF  fsub
0x004056B2  fstp
0x004056B6  fld
0x004056BA  fld
0x004056BE  fmul st, st
0x004056C0  fld
0x004056C2  fmulp st(2), st
0x004056C4  faddp st(1), st
0x004056C6  fstp
0x004056CA  fld
0x004056CE  call _CIsqrt                                 ; -> _CIsqrt
0x004056D3  fstp
0x004056D7  cmp [esp+88h+var_75], 0
0x004056DC  fld
0x004056E0  fstp
0x004056E4  jnz loc_405703
0x004056E6  fld
0x004056EA  fld
0x004056EE  fcomp
0x004056F0  fnstsw ax
0x004056F2  test ah, 41h
0x004056F5  jnz loc_405701
0x004056F7  fstp
0x004056FB  mov [esp+88h+var_5C], edi
0x004056FF  jmp loc_405703
0x00405701  fstp
0x00405703  mov eax, [esp+88h+var_54]
0x00405707  mov ecx, [esp+88h+var_58]
0x0040570B  add eax, 1
0x0040570E  cmp eax, ecx
0x00405710  mov [esp+88h+var_54], eax
0x00405714  jl loc_405600
0x0040571A  mov eax, [esp+88h+var_5C]
0x0040571E  test eax, eax
0x00405720  jz loc_405AA7
0x00405726  mov ecx, [eax+8]
0x00405729  mov edx, [eax+0Ch]
0x0040572C  mov [esp+88h+var_48], eax
0x00405730  mov [esp+88h+var_28], ecx
0x00405734  mov [esp+88h+var_24], edx
0x00405738  mov [esp+88h+var_4C], 6
0x00405740  mov [esp+88h+var_54], 0Ah
0x00405748  lea eax, [esp+88h+var_28]
0x0040574C  jmp loc_405750
0x0040574E  mov eax, ebx
0x00405750  fld
0x00405754  mov ecx, [eax]
0x00405756  fsub
0x00405758  mov edx, [eax+4]
0x0040575B  lea edi, [esp+88h+var_20]
0x0040575F  lea esi, [esp+88h+var_8]
0x00405766  fstp
0x0040576A  mov [esp+88h+var_18], ecx
0x0040576E  fld
0x00405772  mov [esp+88h+var_14], edx
0x00405776  fsub
0x00405779  fstp
0x0040577D  call sub_417FD0                              ; -> sub_417FD0 [Vec3_NormalizeOrZero]
0x00405782  fld
0x00405786  fld                                          ; -> 0x0041D960 in .rdata
0x0040578C  fmul st(1), st
0x0040578E  fxch
0x00405790  fstp
0x00405794  fld
0x00405798  fld
0x0040579A  call __ftol2_sse                             ; -> __ftol2_sse
0x0040579F  mov ecx, eax
0x004057A1  mov [esp+88h+var_50], ecx
0x004057A5  fild
0x004057A9  fcompp
0x004057AB  fnstsw ax
0x004057AD  test ah, 41h
0x004057B0  jnz loc_4057B5
0x004057B2  sub ecx, 1
0x004057B5  xor esi, esi
0x004057B7  cmp ecx, esi
0x004057B9  jge loc_4057BF
0x004057BB  xor ebx, ebx
0x004057BD  jmp loc_4057CB
0x004057BF  cmp ecx, 5Fh
0x004057C2  mov ebx, 5Fh
0x004057C7  jg loc_4057CB
0x004057C9  mov ebx, ecx
0x004057CB  fmul
0x004057CF  fstp
0x004057D3  fld
0x004057D7  fld
0x004057D9  call __ftol2_sse                             ; -> __ftol2_sse
0x004057DE  mov ecx, eax
0x004057E0  mov [esp+88h+var_50], ecx
0x004057E4  fild
0x004057E8  fcompp
0x004057EA  fnstsw ax
0x004057EC  test ah, 41h
0x004057EF  jnz loc_4057F4
0x004057F1  sub ecx, 1
0x004057F4  cmp ecx, esi
0x004057F6  jge loc_4057FE
0x004057F8  mov [esp+88h+var_74], esi
0x004057FC  jmp loc_40580F
0x004057FE  cmp ecx, 5Fh
0x00405801  mov [esp+88h+var_74], 5Fh
0x00405809  jg loc_40580F
0x0040580B  mov [esp+88h+var_74], ecx
0x0040580F  fld                                          ; -> 0x0041D8C0 in .rdata
0x00405815  mov [esp+88h+var_50], esi
0x00405819  mov edi, [esp+88h+var_4C]
0x0040581D  cmp edi, [esp+88h+var_54]
0x00405821  jg loc_405A91
0x00405827  mov edx, [esp+88h+var_74]
0x0040582B  lea eax, ds:0[edi*8]
0x00405832  mov [esp+88h+var_68], eax
0x00405836  mov eax, ebx
0x00405838  sub eax, edx
0x0040583A  mov [esp+88h+var_40], eax
0x0040583E  mov ecx, edx
0x00405840  sub ecx, edi
0x00405842  mov eax, edx
0x00405844  sub eax, ebx
0x00405846  mov [esp+88h+var_58], ecx
0x0040584A  mov [esp+88h+var_3C], eax
0x0040584E  cmp [esp+88h+var_68], esi
0x00405852  mov [esp+88h+var_5C], esi
0x00405856  jle loc_405A78
0x0040585C  mov edx, [esp+88h+var_64]
0x00405860  add edx, 2
0x00405863  mov [esp+88h+var_34], edx
0x00405867  mov eax, [esp+88h+var_5C]
0x0040586B  cdq
0x0040586C  lea esi, [edi+edi]
0x0040586F  idiv
0x00405871  sub edx, edi
0x00405873  sub eax, 0
0x00405876  mov esi, edx
0x00405878  jz loc_4058AD
0x0040587A  sub eax, 1
0x0040587D  jz loc_4058A1
0x0040587F  sub eax, 1
0x00405882  jz loc_405893
0x00405884  mov eax, [esp+88h+var_40]
0x00405888  lea edx, [eax+ecx]
0x0040588B  mov ecx, [esp+88h+var_74]
0x0040588F  sub ecx, esi
0x00405891  jmp loc_4058B0
0x00405893  mov ecx, [esp+88h+var_3C]
0x00405897  mov edx, ebx
0x00405899  add ecx, edi
0x0040589B  sub edx, esi
0x0040589D  add ecx, ebx
0x0040589F  jmp loc_4058B0
0x004058A1  mov eax, [esp+88h+var_74]
0x004058A5  lea edx, [edi+ebx]
0x004058A8  lea ecx, [esi+eax]
0x004058AB  jmp loc_4058B0
0x004058AD  lea edx, [esi+ebx]
0x004058B0  mov eax, [esp+88h+var_38]
0x004058B4  push eax
0x004058B5  mov eax, [esp+8Ch+var_34]
0x004058B9  mov [esp+8Ch+var_6C], ecx
0x004058BD  mov [esp+8Ch+var_70], edx
0x004058C1  call sub_414CE0                              ; -> sub_414CE0 [CheckGridAreaPassesCriteria]
0x004058C6  test al, al
0x004058C8  jz loc_405A55
0x004058CE  test byte ptr [esp+88h+var_64], 1
0x004058D3  jz loc_4058DD
0x004058D5  fld                                          ; -> 0x0041D9F0 in .rdata
0x004058DB  jmp loc_4058DF
0x004058DD  fldz
0x004058DF  fstp
0x004058E3  mov eax, [esp+88h+var_48]
0x004058E7  test eax, eax
0x004058E9  fild
0x004058ED  fmul st, st(1)
0x004058EF  fld
0x004058F3  fld
0x004058F5  faddp st(2), st
0x004058F7  fxch
0x004058F9  fstp
0x004058FD  fild
0x00405901  fmulp st(2), st
0x00405903  faddp st(1), st
0x00405905  fstp
0x00405909  jz loc_405981
0x0040590B  cmp [ebp+arg_4], 0
0x0040590F  jnz loc_405987
0x00405911  add eax, 8
0x00405914  lea esi, [esp+88h+var_28]
0x00405918  lea ecx, [esp+88h+var_18]
0x0040591C  lea edx, [esp+88h+var_10]
0x00405920  call sub_4182F0                              ; -> sub_4182F0 [IntersectRaySegmentOrLine]
0x00405925  fld
0x00405929  fsub
0x0040592D  fstp
0x00405931  fld
0x00405935  fsub
0x00405939  fstp
0x0040593D  fld
0x00405941  fld
0x00405945  fmul st, st
0x00405947  fld
0x00405949  fmulp st(2), st
0x0040594B  faddp st(1), st
0x0040594D  fstp
0x00405951  fld
0x00405955  call _CIsqrt                                 ; -> _CIsqrt
0x0040595A  fstp
0x0040595E  fld
0x00405962  fild
0x00405966  fmul                                         ; -> 0x0041D9F0 in .rdata
0x0040596C  fadd                                         ; -> 0x0041DAC8 in .rdata
0x00405972  fcompp
0x00405974  fnstsw ax
0x00405976  test ah, 41h
0x00405979  jz loc_405A57
0x0040597F  jmp loc_4059D3
0x00405981  cmp [ebp+arg_4], 0
0x00405985  jz loc_4059D3
0x00405987  fld
0x0040598B  fsub
0x0040598E  fstp
0x00405992  fld
0x00405996  fsub
0x00405999  fstp
0x0040599D  fld
0x004059A1  fld
0x004059A5  fmul st, st
0x004059A7  fld
0x004059A9  fmulp st(2), st
0x004059AB  faddp st(1), st
0x004059AD  fstp
0x004059B1  fld
0x004059B5  call _CIsqrt                                 ; -> _CIsqrt
0x004059BA  fstp
0x004059BE  fld
0x004059C2  fcomp                                        ; -> 0x0041DAC4 in .rdata
0x004059C8  fnstsw ax
0x004059CA  test ah, 5
0x004059CD  jnp loc_405A57
0x004059D3  cmp [esp+88h+var_50], 0
0x004059D8  jnz loc_405AB4
0x004059DE  cmp [ebp+arg_4], 0
0x004059E2  jnz loc_405AB4
0x004059E8  mov eax, [esp+88h+var_44]
0x004059EC  fld
0x004059F0  fsub
0x004059F2  mov edx, [esp+88h+var_60]
0x004059F6  movzx ecx, word ptr [edx]
0x004059F9  test ecx, 800h
0x004059FF  fstp
0x00405A03  fld
0x00405A07  fsub
0x00405A0A  fstp
0x00405A0E  fld
0x00405A15  fmul
0x00405A19  fld
0x00405A1D  fmul
0x00405A24  faddp st(1), st
0x00405A26  fstp
0x00405A2A  fld
0x00405A2E  jz loc_405A3B
0x00405A30  fldz
0x00405A32  fcomp
0x00405A34  fnstsw ax
0x00405A36  test ah, 41h
0x00405A39  jz loc_405A55
0x00405A3B  cmp byte ptr [edx+2Eh], 0
0x00405A3F  jge loc_405AB2
0x00405A41  test cl, 40h
0x00405A44  jnz loc_405AB2
0x00405A46  fcomp                                        ; -> 0x0041DAC0 in .rdata
0x00405A4C  fnstsw ax
0x00405A4E  test ah, 41h
0x00405A51  jnz loc_405AB4
0x00405A53  jmp loc_405A57
0x00405A55  fstp
0x00405A57  mov eax, [esp+88h+var_5C]
0x00405A5B  fld                                          ; -> 0x0041D8C0 in .rdata
0x00405A61  mov ecx, [esp+88h+var_58]
0x00405A65  add eax, 1
0x00405A68  cmp eax, [esp+88h+var_68]
0x00405A6C  mov [esp+88h+var_5C], eax
0x00405A70  jl loc_405867
0x00405A76  xor esi, esi
0x00405A78  add [esp+88h+var_68], 8
0x00405A7D  add edi, 1
0x00405A80  sub ecx, 1
0x00405A83  cmp edi, [esp+88h+var_54]
0x00405A87  mov [esp+88h+var_58], ecx
0x00405A8B  jle loc_40584E
0x00405A91  mov eax, [esp+88h+var_50]
0x00405A95  add eax, 1
0x00405A98  cmp eax, 2
0x00405A9B  mov [esp+88h+var_50], eax
0x00405A9F  jl loc_405819
0x00405AA5  fstp
0x00405AA7  xor al, al
0x00405AA9  pop edi
0x00405AAA  pop esi
0x00405AAB  pop ebx
0x00405AAC  mov esp, ebp
0x00405AAE  pop ebp
0x00405AAF  retn 0Ch
0x00405AB2  fstp
0x00405AB4  mov eax, [ebp+arg_8]
0x00405AB7  mov ecx, [esp+88h+var_28]
0x00405ABB  mov edx, [esp+88h+var_24]
0x00405ABF  pop edi
0x00405AC0  pop esi
0x00405AC1  mov [eax], ecx
0x00405AC3  mov [eax+4], edx
0x00405AC6  mov al, 1
0x00405AC8  pop ebx
0x00405AC9  mov esp, ebp
0x00405ACB  pop ebp
0x00405ACC  retn 0Ch
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
