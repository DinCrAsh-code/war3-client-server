# Function summary: sub_406000 (0x00406000)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00406000
- Name: sub_406000 (auto-generated placeholder)
- Size: 727 bytes, 239 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_406760 -> sub_406000

## How callers use this function

### From sub_406760 (0x00406760) - call at 0x00406A2F, 0x00406AA6

What that caller does overall (preliminary): Performs state machine updates, distance/target calculations, and behavior checks for an individual simulation entity (passed in ebx) based on its current state, flags, and spatial proximity.

Called at 0x00406A2F and 0x00406AA6 with the entity pointer in eax and a pointer to a local stack variable, used to compute distance, range, or angle relative to a target.

## Functions it calls or references

### sub_40C950 [ValidateEntityCriteriaFilter] (0x0040C950) - address taken at 0x00406029

- Prototype: `bool __fastcall ValidateEntityCriteriaFilter(const void *entity, const void *queryCriteria)`
- Summary: Evaluates whether a given entity matches a comprehensive set of filtering criteria, flags, type constraints, and spatial or status conditions specified in a query criteria structure.

### sub_4177F0 [SpatialGridQueryEntities] (0x004177F0) - call at 0x00406055

- Prototype: `int __stdcall SpatialGridQueryEntities(void *grid_data, float center_x, float center_y, float radius, void *callback_filter, void *user_data, void **out_entity_array, int max_entities)`
- Summary: Queries a spatial grid structure for entities within a bounding box or radius, filters them using distance checks and an optional callback, and populates an output array.

Reference kinds used above:
- *address taken*: the function's address is used as data there - e.g. passed as a callback or stored as a function pointer - rather than called directly.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00406000  sub esp, 0DCh
0x00406006  mov ecx, [eax]
0x00406008  fldz
0x0040600A  push esi
0x0040600B  fstp
0x0040600F  fld
0x00406016  push edi
0x00406017  push 30h
0x00406019  lea edx, [esp+0E8h+var_C4]
0x0040601D  push edx
0x0040601E  mov edx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406024  lea eax, [esp+0ECh+var_D4]
0x00406028  push eax
0x00406029  push offset sub_40C950                       ; -> sub_40C950 [ValidateEntityCriteriaFilter]
0x0040602E  push ecx
0x0040602F  mov [esp+0F8h+var_D0], ecx
0x00406033  fstp
0x00406036  mov ecx, [esp+0F8h+arg_0]
0x0040603D  push ecx
0x0040603E  add edx, 43D7Ch
0x00406044  push edx
0x00406045  mov [esp+100h+var_D4], 0
0x0040604D  mov [esp+100h+var_CC], 101h
0x00406055  call sub_4177F0                              ; -> sub_4177F0 [SpatialGridQueryEntities]
0x0040605A  fldz
0x0040605C  fst
0x00406060  mov esi, eax
0x00406062  fld                                          ; -> 0x0041D9A8 in .rdata
0x00406068  xor ecx, ecx
0x0040606A  cmp esi, 4
0x0040606D  fld                                          ; -> 0x0041D9A0 in .rdata
0x00406073  jl loc_406247
0x00406079  lea edi, [esi-3]
0x0040607C  mov edx, [esp+ecx*4+0E4h+var_C4]
0x00406080  mov al, [edx+18h]
0x00406083  not al
0x00406085  test al, 1
0x00406087  jz loc_4060D8
0x00406089  mov eax, [edx+1Ch]
0x0040608C  cmp byte ptr [eax+0Ch], 0
0x00406090  jz loc_4060D8
0x00406092  fld
0x00406095  fstp
0x00406099  fld
0x0040609D  fcom
0x0040609F  fnstsw ax
0x004060A1  test ah, 41h
0x004060A4  jnz loc_4060BC
0x004060A6  fstp
0x004060AA  fld
0x004060AD  fadd st, st(1)
0x004060AF  fmul
0x004060B2  fdiv
0x004060B6  fstp
0x004060BA  jmp loc_4060E0
0x004060BC  fstp
0x004060BE  fxch
0x004060C0  fst
0x004060C4  fxch
0x004060C6  fld
0x004060C9  fadd st, st(1)
0x004060CB  fmul
0x004060CE  fdiv
0x004060D2  fstp
0x004060D6  jmp loc_4060E0
0x004060D8  fxch
0x004060DA  fst
0x004060DE  fxch
0x004060E0  mov edx, [esp+ecx*4+0E4h+var_C0]
0x004060E4  fld
0x004060E8  mov al, [edx+18h]
0x004060EB  fadd
0x004060EF  not al
0x004060F1  test al, 1
0x004060F3  fstp
0x004060F7  jz loc_406148
0x004060F9  mov eax, [edx+1Ch]
0x004060FC  cmp byte ptr [eax+0Ch], 0
0x00406100  jz loc_406148
0x00406102  fld
0x00406105  fstp
0x00406109  fld
0x0040610D  fcom
0x0040610F  fnstsw ax
0x00406111  test ah, 41h
0x00406114  jnz loc_40612C
0x00406116  fstp
0x0040611A  fld
0x0040611D  fadd st, st(1)
0x0040611F  fmul
0x00406122  fdiv
0x00406126  fstp
0x0040612A  jmp loc_406150
0x0040612C  fstp
0x0040612E  fxch
0x00406130  fst
0x00406134  fxch
0x00406136  fld
0x00406139  fadd st, st(1)
0x0040613B  fmul
0x0040613E  fdiv
0x00406142  fstp
0x00406146  jmp loc_406150
0x00406148  fxch
0x0040614A  fst
0x0040614E  fxch
0x00406150  mov edx, [esp+ecx*4+0E4h+var_BC]
0x00406154  fld
0x00406158  mov al, [edx+18h]
0x0040615B  fadd
0x0040615F  not al
0x00406161  test al, 1
0x00406163  fstp
0x00406167  jz loc_4061B8
0x00406169  mov eax, [edx+1Ch]
0x0040616C  cmp byte ptr [eax+0Ch], 0
0x00406170  jz loc_4061B8
0x00406172  fld
0x00406175  fstp
0x00406179  fld
0x0040617D  fcom
0x0040617F  fnstsw ax
0x00406181  test ah, 41h
0x00406184  jnz loc_40619C
0x00406186  fstp
0x0040618A  fld
0x0040618D  fadd st, st(1)
0x0040618F  fmul
0x00406192  fdiv
0x00406196  fstp
0x0040619A  jmp loc_4061C0
0x0040619C  fstp
0x0040619E  fxch
0x004061A0  fst
0x004061A4  fxch
0x004061A6  fld
0x004061A9  fadd st, st(1)
0x004061AB  fmul
0x004061AE  fdiv
0x004061B2  fstp
0x004061B6  jmp loc_4061C0
0x004061B8  fxch
0x004061BA  fst
0x004061BE  fxch
0x004061C0  mov edx, [esp+ecx*4+0E4h+var_B8]
0x004061C4  fld
0x004061C8  mov al, [edx+18h]
0x004061CB  fadd
0x004061CF  not al
0x004061D1  test al, 1
0x004061D3  fstp
0x004061D7  jz loc_406228
0x004061D9  mov eax, [edx+1Ch]
0x004061DC  cmp byte ptr [eax+0Ch], 0
0x004061E0  jz loc_406228
0x004061E2  fld
0x004061E5  fstp
0x004061E9  fld
0x004061ED  fcom
0x004061EF  fnstsw ax
0x004061F1  test ah, 41h
0x004061F4  jnz loc_40620C
0x004061F6  fstp
0x004061FA  fld
0x004061FD  fadd st, st(1)
0x004061FF  fmul
0x00406202  fdiv
0x00406206  fstp
0x0040620A  jmp loc_406230
0x0040620C  fstp
0x0040620E  fxch
0x00406210  fst
0x00406214  fxch
0x00406216  fld
0x00406219  fadd st, st(1)
0x0040621B  fmul
0x0040621E  fdiv
0x00406222  fstp
0x00406226  jmp loc_406230
0x00406228  fxch
0x0040622A  fst
0x0040622E  fxch
0x00406230  fld
0x00406234  add ecx, 4
0x00406237  cmp ecx, edi
0x00406239  fadd
0x0040623D  fstp
0x00406241  jl loc_40607C
0x00406247  cmp ecx, esi
0x00406249  jge loc_4062C2
0x0040624B  mov edx, [esp+ecx*4+0E4h+var_C4]
0x0040624F  mov al, [edx+18h]
0x00406252  not al
0x00406254  test al, 1
0x00406256  jz loc_4062A7
0x00406258  mov eax, [edx+1Ch]
0x0040625B  cmp byte ptr [eax+0Ch], 0
0x0040625F  jz loc_4062A7
0x00406261  fld
0x00406264  fstp
0x00406268  fld
0x0040626C  fcom
0x0040626E  fnstsw ax
0x00406270  test ah, 41h
0x00406273  jnz loc_40628B
0x00406275  fstp
0x00406279  fld
0x0040627C  fadd st, st(1)
0x0040627E  fmul
0x00406281  fdiv
0x00406285  fstp
0x00406289  jmp loc_4062AF
0x0040628B  fstp
0x0040628D  fxch
0x0040628F  fst
0x00406293  fxch
0x00406295  fld
0x00406298  fadd st, st(1)
0x0040629A  fmul
0x0040629D  fdiv
0x004062A1  fstp
0x004062A5  jmp loc_4062AF
0x004062A7  fxch
0x004062A9  fst
0x004062AD  fxch
0x004062AF  fld
0x004062B3  add ecx, 1
0x004062B6  cmp ecx, esi
0x004062B8  fadd
0x004062BC  fstp
0x004062C0  jl loc_40624B
0x004062C2  fstp
0x004062C4  pop edi
0x004062C5  fstp
0x004062C7  pop esi
0x004062C8  fstp
0x004062CA  fld
0x004062CE  add esp, 0DCh
0x004062D4  retn 8
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
