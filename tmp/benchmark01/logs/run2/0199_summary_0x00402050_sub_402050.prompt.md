# Function summary: sub_402050 (0x00402050)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00402050
- Name: sub_402050 (auto-generated placeholder)
- Size: 1294 bytes, 373 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402050

## How callers use this function

### From sub_401750 (0x00401750) - call at 0x00401C42

What that caller does overall (preliminary): Initializes and configures a large resource/state block ('RM' header structure) passed from main, setting up internal sub-systems, tables, processing item loops, and validation checks before invoking subsystem teardown or startup functions.

Called in a loop from `0` up to `ebx+5BBBCh` (resource count), passing the resource block pointer (`ebx`) and loop index (`edi`) to perform per-item processing.

## Functions it calls or references

### sub_417FD0 [Vec3_NormalizeOrZero] (0x00417FD0) - call at 0x004020C9

- Prototype: `float __cdecl Vec3_NormalizeOrZero(float x, float y, float z)`
- Summary: Computes the Euclidean length of a 3D vector and normalizes it, returning 1/length or 0 if the length is zero or near-zero.

### sub_418050 [CalculateSpatialOrPathMetric] (0x00418050) - call at 0x004020FB, 0x0040247D

- Prototype: `double __cdecl CalculateSpatialOrPathMetric()`
- Summary: Performs complex floating-point geometry, clamping, and polynomial evaluation calculations using values on the x87 FPU stack.

### sub_418140 [ClampFloat] (0x00418140) - call at 0x00402106

- Prototype: `float __cdecl ClampFloat(float value, float min_val, float max_val)`
- Summary: Performs a clamp operation on a floating-point value between a lower and upper bound using x87 FPU instructions.

### sub_402560 [CreateAndRegisterResourceEntity] (0x00402560) - call at 0x0040211B, 0x004022ED, 0x00402537

- Prototype: `void* __fastcall CreateAndRegisterResourceEntity(void *resourceBlock, int unused_edx, int type_index, int sub_variant_flag, int flags)`
- Summary: Allocates, initializes, and configures a new resource or RTS object entity based on type lookup tables, coordinates, and flags, registering it into spatial grid and resource tracking containers.

### __ftol2_sse (0x0041A160) - call at 0x004021F0, 0x0040224C, 0x004023B9, 0x0040240D

- Prototype: `int __cdecl __ftol2_sse(void)`
- Summary: Converts the top-of-stack x87 floating-point value to a 32-bit signed integer using SSE or fallback FPU routines, returning the integer result in EAX.

### sub_414CE0 [CheckGridAreaPassesCriteria] (0x00414CE0) - call at 0x0040228B

- Prototype: `bool __fastcall CheckGridAction(void *pDataStruct, int edx_val, int ecx_val, int arg0)`
- Summary: Tests whether a 2D rectangular grid region within a structured data array meets specific validity and value criteria, returning true if all checked elements satisfy the conditions.

### sub_414E40 [CheckGridAreaOrFindNearby] (0x00414E40) - call at 0x0040245C

- Prototype: `bool __stdcall CheckGridAreaOrFindNearby(void *gridBase, int *coord1, int *coord2, int maxRadius)`
- Summary: Validates a 2D grid cell or searches outward in concentric square rings up to a given radius for a valid/accessible grid position, returning the first matching coordinate via output pointers.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00402050  sub esp, 6Ch
0x00402053  fld                                          ; -> 0x0041F01C in .data
0x00402059  push ebx
0x0040205A  fmul                                         ; -> 0x0041D8C8 in .rdata
0x00402060  mov ebx, [esp+70h+arg_4]
0x00402064  imul ebx
0x0040206A  push ebp
0x0040206B  mov ebp, [esp+74h+arg_0]
0x0040206F  mov eax, [ebx+ebp+0ECh]
0x00402076  mov ecx, [ebx+ebp+0F0h]
0x0040207D  fstp
0x00402081  fld
0x00402085  add ebx, ebp
0x00402087  fst
0x0040208B  mov [esp+74h+var_68], eax
0x0040208F  fstp
0x00402093  mov [esp+74h+var_64], ecx
0x00402097  fld
0x0040209B  push esi
0x0040209C  fsub
0x004020A0  push edi
0x004020A1  lea edi, [esp+7Ch+var_34]
0x004020A5  lea esi, [esp+7Ch+var_54]
0x004020A9  fstp
0x004020AD  fld
0x004020B1  fstp
0x004020B5  fld
0x004020B9  fsub
0x004020BD  fstp
0x004020C1  fld
0x004020C5  fstp
0x004020C9  call sub_417FD0                              ; -> sub_417FD0 [Vec3_NormalizeOrZero]
0x004020CE  mov dl, [ebx+1]
0x004020D1  fld
0x004020D5  neg dl
0x004020D7  fchs
0x004020D9  fstp
0x004020DD  sbb edx, edx
0x004020DF  and edx, 14h
0x004020E2  add edx, 0Bh
0x004020E5  js loc_402120
0x004020E7  fld
0x004020EB  push 0
0x004020ED  sub esp, 8
0x004020F0  fstp
0x004020F4  fld
0x004020F8  fstp
0x004020FB  call sub_418050                              ; -> sub_418050 [CalculateSpatialOrPathMetric]
0x00402100  add esp, 4
0x00402103  fstp
0x00402106  call sub_418140                              ; -> sub_418140 [ClampFloat]
0x0040210B  mov eax, [esp+84h+arg_4]
0x00402112  fstp
0x00402115  push eax
0x00402116  push ebp
0x00402117  lea ecx, [esp+8Ch+var_68]
0x0040211B  call sub_402560                              ; -> sub_402560 [CreateAndRegisterResourceEntity]
0x00402120  fld
0x00402124  xor edi, edi
0x00402126  fld                                          ; -> 0x0041DB40 in .rdata
0x0040212C  fmul st(1), st
0x0040212E  fxch
0x00402130  fstp
0x00402134  fld
0x00402138  fst
0x0040213C  fmulp st(1), st
0x0040213E  fstp
0x00402142  fld
0x00402146  fld                                          ; -> 0x0041D960 in .rdata
0x0040214C  jmp loc_402156
0x00402150  fstp
0x00402152  fstp
0x00402154  fstp
0x00402156  test edi, edi
0x00402158  jz loc_402162
0x0040215A  fld                                          ; -> 0x0041DB68 in .rdata
0x00402160  jmp loc_402164
0x00402162  fld1
0x00402164  fstp
0x0040216B  fld
0x00402172  fmul                                         ; -> 0x0041DBA8 in .rdata
0x00402178  fstp
0x0040217F  fld
0x00402186  fld
0x00402188  fmul
0x0040218C  fstp
0x00402190  fld
0x00402194  fst
0x00402198  fmul st(1), st
0x0040219A  fxch
0x0040219C  fstp
0x004021A0  fld
0x004021A4  fst
0x004021A8  fld
0x004021AC  fadd st, st(1)
0x004021AE  fstp
0x004021B2  fld
0x004021B6  fst
0x004021BA  fld
0x004021BE  fadd st, st(1)
0x004021C0  fstp
0x004021C4  fld
0x004021C8  fsub
0x004021CC  fstp
0x004021D0  fld
0x004021D4  fsub st, st(5)
0x004021D6  fstp
0x004021DA  fld
0x004021DE  fmul st, st(4)
0x004021E0  fstp
0x004021E7  fld
0x004021EE  fld
0x004021F0  call __ftol2_sse                             ; -> __ftol2_sse
0x004021F5  mov ecx, eax
0x004021F7  mov [esp+7Ch+arg_0], ecx
0x004021FE  fild
0x00402205  fcompp
0x00402207  fnstsw ax
0x00402209  test ah, 41h
0x0040220C  jnz loc_402211
0x0040220E  sub ecx, 1
0x00402211  test ecx, ecx
0x00402213  jge loc_402220
0x00402215  xor esi, esi
0x00402217  mov [esp+7Ch+arg_0], esi
0x0040221E  jmp loc_40223C
0x00402220  cmp ecx, 5Fh
0x00402223  jle loc_402233
0x00402225  mov esi, 5Fh
0x0040222A  mov [esp+7Ch+arg_0], esi
0x00402231  jmp loc_40223C
0x00402233  mov esi, ecx
0x00402235  mov [esp+7Ch+arg_0], ecx
0x0040223C  fld
0x00402240  fmul st, st(4)
0x00402242  fstp
0x00402246  fld
0x0040224A  fld
0x0040224C  call __ftol2_sse                             ; -> __ftol2_sse
0x00402251  mov ecx, eax
0x00402253  mov [esp+7Ch+var_60], ecx
0x00402257  fild
0x0040225B  fcompp
0x0040225D  fnstsw ax
0x0040225F  test ah, 41h
0x00402262  jnz loc_402267
0x00402264  sub ecx, 1
0x00402267  test ecx, ecx
0x00402269  jge loc_40226F
0x0040226B  xor ecx, ecx
0x0040226D  jmp loc_402279
0x0040226F  cmp ecx, 5Fh
0x00402272  jle loc_402279
0x00402274  mov ecx, 5Fh
0x00402279  lea eax, [ebp+1A20h]
0x0040227F  push eax
0x00402280  mov eax, 3
0x00402285  mov edx, esi
0x00402287  mov [esp+80h+var_60], ecx
0x0040228B  call sub_414CE0                              ; -> sub_414CE0 [CheckGridAreaPassesCriteria]
0x00402290  test al, al
0x00402292  jnz loc_4022A4
0x00402294  add edi, 1
0x00402297  cmp edi, 2
0x0040229A  jl loc_402150
0x004022A0  fstp
0x004022A2  jmp loc_402318
0x004022A4  fstp
0x004022A6  push 0
0x004022A8  fstp
0x004022AA  push ecx
0x004022AB  fstp
0x004022AD  lea ecx, [esp+84h+var_34]
0x004022B1  fstp
0x004022B3  mov edx, 27h
0x004022B8  fstp
0x004022BA  fild
0x004022C1  fmul                                         ; -> 0x0041D8C0 in .rdata
0x004022C7  fld                                          ; -> 0x0041D9F0 in .rdata
0x004022CD  fadd st(1), st
0x004022CF  fxch
0x004022D1  fstp
0x004022D5  fild
0x004022D9  fmul                                         ; -> 0x0041D8C0 in .rdata
0x004022DF  faddp st(1), st
0x004022E1  fstp
0x004022E5  fldz
0x004022E7  fstp
0x004022EA  push 5
0x004022EC  push ebp
0x004022ED  call sub_402560                              ; -> sub_402560 [CreateAndRegisterResourceEntity]
0x004022F2  fld                                          ; -> 0x0041D960 in .rdata
0x004022F8  test eax, eax
0x004022FA  fld
0x004022FE  fld
0x00402302  fld
0x00402306  jz loc_402312
0x00402308  mov dword ptr [eax+2B8h], 30D4h
0x00402312  fxch
0x00402314  fxch
0x00402316  fxch
0x00402318  fld                                          ; -> 0x0041DBA0 in .rdata
0x0040231E  mov cl, [ebx+1]
0x00402321  fmul st(2), st
0x00402323  neg cl
0x00402325  fxch
0x00402327  fstp
0x0040232B  fld
0x0040232F  fmulp st(2), st
0x00402331  fxch
0x00402333  sbb ecx, ecx
0x00402335  fstp
0x00402339  and ecx, 14h
0x0040233C  mov edi, ecx
0x0040233E  fadd
0x00402342  xor esi, esi
0x00402344  fstp
0x00402348  fld
0x0040234C  faddp st(2), st
0x0040234E  fxch
0x00402350  fstp
0x00402354  lea edx, [esi-2]
0x00402357  mov [esp+7Ch+arg_0], edx
0x0040235E  fild
0x00402365  fmul                                         ; -> 0x0041D8C0 in .rdata
0x0040236B  fstp
0x00402372  fld
0x00402379  fld
0x0040237B  fmul
0x0040237F  fstp
0x00402383  fmul
0x00402387  fstp
0x0040238B  fld
0x0040238F  fadd
0x00402393  fstp
0x00402397  fld
0x0040239B  fadd
0x0040239F  fstp
0x004023A3  fld
0x004023A7  fmul st, st(1)
0x004023A9  fstp
0x004023B0  fld
0x004023B7  fld
0x004023B9  call __ftol2_sse                             ; -> __ftol2_sse
0x004023BE  mov ecx, eax
0x004023C0  mov [esp+7Ch+arg_0], ecx
0x004023C7  fild
0x004023CE  fcompp
0x004023D0  fnstsw ax
0x004023D2  test ah, 41h
0x004023D5  jnz loc_4023DA
0x004023D7  sub ecx, 1
0x004023DA  xor ebx, ebx
0x004023DC  cmp ecx, ebx
0x004023DE  jge loc_4023E6
0x004023E0  mov [esp+7Ch+var_68], ebx
0x004023E4  jmp loc_4023F7
0x004023E6  cmp ecx, 5Fh
0x004023E9  mov [esp+7Ch+var_68], 5Fh
0x004023F1  jg loc_4023F7
0x004023F3  mov [esp+7Ch+var_68], ecx
0x004023F7  fld
0x004023FB  fmul st, st(1)
0x004023FD  fstp
0x00402404  fld
0x0040240B  fld
0x0040240D  call __ftol2_sse                             ; -> __ftol2_sse
0x00402412  mov ecx, eax
0x00402414  mov [esp+7Ch+arg_0], ecx
0x0040241B  fild
0x00402422  fcompp
0x00402424  fnstsw ax
0x00402426  test ah, 41h
0x00402429  jnz loc_40242E
0x0040242B  sub ecx, 1
0x0040242E  cmp ecx, ebx
0x00402430  jge loc_402438
0x00402432  mov [esp+7Ch+var_60], ebx
0x00402436  jmp loc_402449
0x00402438  cmp ecx, 5Fh
0x0040243B  mov [esp+7Ch+var_60], 5Fh
0x00402443  jg loc_402449
0x00402445  mov [esp+7Ch+var_60], ecx
0x00402449  push 6
0x0040244B  lea eax, [esp+80h+var_60]
0x0040244F  push eax
0x00402450  lea ecx, [esp+84h+var_68]
0x00402454  push ecx
0x00402455  lea eax, [ebp+1A20h]
0x0040245B  push eax
0x0040245C  call sub_414E40                              ; -> sub_414E40 [CheckGridAreaOrFindNearby]
0x00402461  test al, al
0x00402463  jz loc_402546
0x00402469  fstp
0x0040246B  sub esp, 8
0x0040246E  fld
0x00402472  fstp
0x00402476  fld
0x0040247A  fstp
0x0040247D  call sub_418050                              ; -> sub_418050 [CalculateSpatialOrPathMetric]
0x00402482  fstp
0x00402489  add esp, 8
0x0040248C  fldz
0x0040248E  fld
0x00402495  fcom
0x00402497  fnstsw ax
0x00402499  test ah, 5
0x0040249C  jp loc_4024C7
0x0040249E  fld                                          ; -> 0x0041D8A8 in .rdata
0x004024A4  jmp loc_4024A8
0x004024A6  fxch
0x004024A8  fadd st(1), st
0x004024AA  fxch
0x004024AC  fstp
0x004024B3  fld
0x004024BA  fcom
0x004024BC  fnstsw ax
0x004024BE  test ah, 5
0x004024C1  jnp loc_4024A6
0x004024C3  fstp
0x004024C5  jmp loc_4024CF
0x004024C7  fstp
0x004024C9  fld                                          ; -> 0x0041D8A8 in .rdata
0x004024CF  fcom
0x004024D1  fnstsw ax
0x004024D3  test ah, 41h
0x004024D6  jnp loc_4024DE
0x004024D8  fstp
0x004024DA  jmp loc_4024FB
0x004024DC  fxch
0x004024DE  fsub st(1), st
0x004024E0  fxch
0x004024E2  fstp
0x004024E9  fld
0x004024F0  fcom
0x004024F2  fnstsw ax
0x004024F4  test ah, 1
0x004024F7  jz loc_4024DC
0x004024F9  fstp
0x004024FB  cmp edi, ebx
0x004024FD  fild
0x00402501  fld                                          ; -> 0x0041D8C8 in .rdata
0x00402507  fadd st(1), st
0x00402509  fld                                          ; -> 0x0041D8C0 in .rdata
0x0040250F  fmul st(2), st
0x00402511  fxch
0x00402513  fstp
0x00402517  fiadd
0x0040251B  fmulp st(1), st
0x0040251D  fstp
0x00402521  jl loc_40253E
0x00402523  mov edx, [esp+7Ch+arg_4]
0x0040252A  push ebx
0x0040252B  push ecx
0x0040252C  fstp
0x0040252F  push edx
0x00402530  push ebp
0x00402531  lea ecx, [esp+8Ch+var_1C]
0x00402535  mov edx, edi
0x00402537  call sub_402560                              ; -> sub_402560 [CreateAndRegisterResourceEntity]
0x0040253C  jmp loc_402540
0x0040253E  fstp
0x00402540  fld                                          ; -> 0x0041D960 in .rdata
0x00402546  add esi, 1
0x00402549  cmp esi, 5
0x0040254C  jl loc_402354
0x00402552  pop edi
0x00402553  fstp
0x00402555  pop esi
0x00402556  pop ebp
0x00402557  pop ebx
0x00402558  add esp, 6Ch
0x0040255B  retn 8
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
