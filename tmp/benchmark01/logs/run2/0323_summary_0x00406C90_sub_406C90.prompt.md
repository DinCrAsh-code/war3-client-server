# Function summary: sub_406C90 (0x00406C90)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00406C90
- Name: sub_406C90 (auto-generated placeholder)
- Size: 596 bytes, 196 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_406C90

## How callers use this function

### From sub_403460 (0x00403460) - call at 0x004034C3

What that caller does overall (preliminary): Iterates over a collection of simulation or resource entities (up to a count at [esi+5BBBCh], stepping by 4FCh bytes from offset 0x630), performing modulo-10 and flag-based checks on each element before invoking a batch of update/processing routines.

Called with `edi` pushed as an argument at the end of the update subroutine chain for the current entity.

## Functions it calls or references

### sub_404760 [SerializeCommandBatch] (0x00404760) - call at 0x00406D77, 0x00406EC3

- Prototype: `void __stdcall SerializeCommandBatch(const void *arg_0, void *arg_4, int count, char arg_C, int arg_10, const void *arg_14)`
- Summary: Iterates over a collection of items, populates a local command packet structure with pointers and metadata, and serializes it using the packet serialization routine.

### sub_40CAA0 [ValidateAdvancedEntityMatch] (0x0040CAA0) - address taken at 0x00406DAD

- Prototype: `__thiscall bool ValidateAdvancedEntityMatch(const void *entity, const void *queryCriteria)`
- Summary: Evaluates whether an entity meets criteria and specific internal state/compatibility conditions to qualify for an interaction or query result.

### sub_4062E0 [CalculateEntityCentroidOrAverage] (0x004062E0) - call at 0x00406DCE

- Prototype: `void* __stdcall CalculateEntityCentroidOrAverage(void *entity, void *outPoint)`
- Summary: Calculates the average spatial position or centroid from an entity's internal collections of points and referenced objects, storing the result in an output structure.

### sub_4177F0 [SpatialGridQueryEntities] (0x004177F0) - call at 0x00406DE4

- Prototype: `int __stdcall SpatialGridQueryEntities(void *grid_data, float center_x, float center_y, float radius, void *callback_filter, void *user_data, void **out_entity_array, int max_entities)`
- Summary: Queries a spatial grid structure for entities within a bounding box or radius, filters them using distance checks and an optional callback, and populates an output array.

Reference kinds used above:
- *address taken*: the function's address is used as data there - e.g. passed as a callback or stored as a function pointer - rather than called directly.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00406C90  sub esp, 0E0h
0x00406C96  mov eax, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406C9B  fld
0x00406CA1  push ebx
0x00406CA2  fstp
0x00406CA6  push ebp
0x00406CA7  fld
0x00406CAB  mov ebp, [esp+0E8h+arg_0]
0x00406CB2  fld
0x00406CB4  push esi
0x00406CB5  fsub
0x00406CB8  push edi
0x00406CB9  fcomp                                        ; -> 0x0041D968 in .rdata
0x00406CBF  fnstsw ax
0x00406CC1  test ah, 5
0x00406CC4  jnp loc_406ED5
0x00406CCA  cmp dword ptr [ebp+0Ch], 3
0x00406CCE  jz loc_406ED5
0x00406CD4  xor esi, esi
0x00406CD6  fstp
0x00406CD9  cmp [ebp+3C8h], esi
0x00406CDF  mov [esp+0F0h+var_DC], esi
0x00406CE3  jle loc_406ED7
0x00406CE9  mov ebx, [ebp+3C8h]
0x00406CEF  fld                                          ; -> 0x0041D948 in .rdata
0x00406CF5  lea edi, [ebp+248h]
0x00406CFB  mov ecx, [edi]
0x00406CFD  mov edx, [ecx+1Ch]
0x00406D00  movzx edx, word ptr [edx]
0x00406D03  test dl, 10h
0x00406D06  jnz loc_406D5A
0x00406D08  fld
0x00406D0B  fld
0x00406D0E  fmul st, st(2)
0x00406D10  fcompp
0x00406D12  fnstsw ax
0x00406D14  test ah, 41h
0x00406D17  jnz loc_406D30
0x00406D19  cmp dword ptr [ecx+1F4h], 0
0x00406D20  jz loc_406D30
0x00406D22  cmp esi, 10h
0x00406D25  jge loc_406D30
0x00406D27  mov [esp+esi*4+0F0h+var_C8], ecx
0x00406D2B  add esi, 1
0x00406D2E  jmp loc_406D5A
0x00406D30  test edx, 4000h
0x00406D36  jz loc_406D5A
0x00406D38  cmp dword ptr [ecx+1F4h], 0
0x00406D3F  jz loc_406D5A
0x00406D41  cmp [esp+0F0h+var_DC], 18h
0x00406D46  jge loc_406D5A
0x00406D48  mov eax, [esp+0F0h+var_DC]
0x00406D4C  mov [esp+eax*4+0F0h+var_60], ecx
0x00406D53  add eax, 1
0x00406D56  mov [esp+0F0h+var_DC], eax
0x00406D5A  add edi, 4
0x00406D5D  sub ebx, 1
0x00406D60  jnz loc_406CFB
0x00406D62  test esi, esi
0x00406D64  fstp
0x00406D66  jle loc_406D7C
0x00406D68  lea eax, [ebp+34h]
0x00406D6B  push eax
0x00406D6C  push 0
0x00406D6E  push 3
0x00406D70  push esi
0x00406D71  lea ecx, [esp+100h+var_C8]
0x00406D75  push ecx
0x00406D76  push ebp
0x00406D77  call sub_404760                              ; -> sub_404760 [SerializeCommandBatch]
0x00406D7C  cmp [esp+0F0h+var_DC], 2
0x00406D81  jl loc_406ED7
0x00406D87  mov edx, [esp+0F0h+var_60]
0x00406D8E  fldz
0x00406D90  mov eax, [ebp+0]
0x00406D93  fstp
0x00406D97  fld                                          ; -> 0x0041DB9C in .rdata
0x00406D9D  push 18h
0x00406D9F  lea ecx, [esp+0F4h+var_C8]
0x00406DA3  push ecx
0x00406DA4  mov [esp+0F8h+var_D8], edx
0x00406DA8  lea edx, [esp+0F8h+var_D8]
0x00406DAC  push edx
0x00406DAD  push offset sub_40CAA0                       ; -> sub_40CAA0 [ValidateAdvancedEntityMatch]
0x00406DB2  sub esp, 8
0x00406DB5  mov [esp+108h+var_D4], eax
0x00406DB9  fstp
0x00406DBD  lea eax, [esp+108h+var_68]
0x00406DC4  push eax
0x00406DC5  push ebp
0x00406DC6  mov [esp+110h+var_D0], 121h
0x00406DCE  call sub_4062E0                              ; -> sub_4062E0 [CalculateEntityCentroidOrAverage]
0x00406DD3  mov ecx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406DD9  add esp, 4
0x00406DDC  push eax
0x00406DDD  add ecx, 43D7Ch
0x00406DE3  push ecx
0x00406DE4  call sub_4177F0                              ; -> sub_4177F0 [SpatialGridQueryEntities]
0x00406DE9  mov esi, eax
0x00406DEB  xor ecx, ecx
0x00406DED  xor edx, edx
0x00406DEF  cmp esi, 4
0x00406DF2  jl loc_406E6A
0x00406DF4  lea edi, [esi-3]
0x00406DF7  test ecx, ecx
0x00406DF9  jz loc_406E0E
0x00406DFB  mov eax, [esp+edx*4+0F0h+var_C8]
0x00406DFF  fld
0x00406E02  fld
0x00406E05  fcompp
0x00406E07  fnstsw ax
0x00406E09  test ah, 41h
0x00406E0C  jnz loc_406E12
0x00406E0E  mov ecx, [esp+edx*4+0F0h+var_C8]
0x00406E12  test ecx, ecx
0x00406E14  jz loc_406E29
0x00406E16  mov eax, [esp+edx*4+0F0h+var_C4]
0x00406E1A  fld
0x00406E1D  fld
0x00406E20  fcompp
0x00406E22  fnstsw ax
0x00406E24  test ah, 41h
0x00406E27  jnz loc_406E2D
0x00406E29  mov ecx, [esp+edx*4+0F0h+var_C4]
0x00406E2D  test ecx, ecx
0x00406E2F  jz loc_406E44
0x00406E31  mov eax, [esp+edx*4+0F0h+var_C0]
0x00406E35  fld
0x00406E38  fld
0x00406E3B  fcompp
0x00406E3D  fnstsw ax
0x00406E3F  test ah, 41h
0x00406E42  jnz loc_406E48
0x00406E44  mov ecx, [esp+edx*4+0F0h+var_C0]
0x00406E48  test ecx, ecx
0x00406E4A  jz loc_406E5F
0x00406E4C  mov eax, [esp+edx*4+0F0h+var_BC]
0x00406E50  fld
0x00406E53  fld
0x00406E56  fcompp
0x00406E58  fnstsw ax
0x00406E5A  test ah, 41h
0x00406E5D  jnz loc_406E63
0x00406E5F  mov ecx, [esp+edx*4+0F0h+var_BC]
0x00406E63  add edx, 4
0x00406E66  cmp edx, edi
0x00406E68  jl loc_406DF7
0x00406E6A  cmp edx, esi
0x00406E6C  jge loc_406E92
0x00406E6E  mov edi, edi
0x00406E70  test ecx, ecx
0x00406E72  jz loc_406E87
0x00406E74  mov eax, [esp+edx*4+0F0h+var_C8]
0x00406E78  fld
0x00406E7B  fld
0x00406E7E  fcompp
0x00406E80  fnstsw ax
0x00406E82  test ah, 41h
0x00406E85  jnz loc_406E8B
0x00406E87  mov ecx, [esp+edx*4+0F0h+var_C8]
0x00406E8B  add edx, 1
0x00406E8E  cmp edx, esi
0x00406E90  jl loc_406E70
0x00406E92  test ecx, ecx
0x00406E94  jz loc_406ED7
0x00406E96  fld
0x00406E99  fld
0x00406E9C  fmul                                         ; -> 0x0041DA28 in .rdata
0x00406EA2  fcompp
0x00406EA4  fnstsw ax
0x00406EA6  test ah, 41h
0x00406EA9  jnz loc_406ED7
0x00406EAB  mov eax, [ecx+4]
0x00406EAE  lea edx, [ecx+8]
0x00406EB1  mov ecx, [esp+0F0h+var_DC]
0x00406EB5  push edx
0x00406EB6  push eax
0x00406EB7  push 4
0x00406EB9  push ecx
0x00406EBA  lea edx, [esp+100h+var_60]
0x00406EC1  push edx
0x00406EC2  push ebp
0x00406EC3  call sub_404760                              ; -> sub_404760 [SerializeCommandBatch]
0x00406EC8  pop edi
0x00406EC9  pop esi
0x00406ECA  pop ebp
0x00406ECB  pop ebx
0x00406ECC  add esp, 0E0h
0x00406ED2  retn 4
0x00406ED5  fstp
0x00406ED7  pop edi
0x00406ED8  pop esi
0x00406ED9  pop ebp
0x00406EDA  pop ebx
0x00406EDB  add esp, 0E0h
0x00406EE1  retn 4
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
