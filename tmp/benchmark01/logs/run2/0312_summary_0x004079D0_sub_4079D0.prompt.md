# Function summary: sub_4079D0 (0x004079D0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004079D0
- Name: sub_4079D0 (auto-generated placeholder)
- Size: 322 bytes, 98 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_4079D0

## How callers use this function

### From sub_403460 (0x00403460) - call at 0x004034B6

What that caller does overall (preliminary): Iterates over a collection of simulation or resource entities (up to a count at [esi+5BBBCh], stepping by 4FCh bytes from offset 0x630), performing modulo-10 and flag-based checks on each element before invoking a batch of update/processing routines.

Called as part of the sequence of sub-routine updates for the current entity when interval and flag checks pass.

## Functions it calls or references

### __ftol2_sse (0x0041A160) - call at 0x00407A4E

- Prototype: `int __cdecl __ftol2_sse(void)`
- Summary: Converts the top-of-stack x87 floating-point value to a 32-bit signed integer using SSE or fallback FPU routines, returning the integer result in EAX.

### sub_4046B0 [DispatchCommandPacket] (0x004046B0) - call at 0x00407ADC

- Prototype: `void __fastcall DispatchCommandPacket(void *entity, void *edx_unused, void *sourceData, int arg0, char arg4, char arg8, int argC, short arg10)`
- Summary: Constructs and serializes an outgoing network command packet using a local bitstream buffer, checking global packet-rate limits before transmission.

### sub_406EF0 [Entity_CheckAndSerializeCommand] (0x00406EF0) - call at 0x00407AF8

- Prototype: `__thiscall void Entity_CheckAndSerializeCommand(void *entity, int arg1, int arg2)`
- Summary: Validates entity state/timers and serializes a network command packet into a bitstream if rate limits and conditions are satisfied.

### sub_4070A0 [CUnit::EvaluateCombatActionOrAcquireTarget] (0x004070A0) - call at 0x00407AFE

- Prototype: `void __fastcall CUnit::EvaluateCombatActionOrAcquireTarget(void *entity, void *edx_unused, int arg0)`
- Summary: Evaluates unit combat states, checks proximity/range to targets or entities in an internal array, and dispatches command packets if criteria are met.

### sub_4071C0 [EvaluateEntityAbilityCriteriaAndCast] (0x004071C0) - call at 0x00407B0B

- Prototype: `bool __stdcall EvaluateEntityAbilityCriteriaAndCast(void *entity, void *component)`
- Summary: Evaluates spatial queries and targeting criteria for an entity's sub-components, checking conditions such as counts of nearby filtered entities or raycast line-of-sight to trigger and dispatch command packets.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004079D0  sub esp, 10h
0x004079D3  push ebx
0x004079D4  push ebp
0x004079D5  mov ebp, [edi]
0x004079D7  imul ebp
0x004079DD  add ebp, dword_42AC60                        ; -> 0x0042AC60 in .data
0x004079E3  xor eax, eax
0x004079E5  cmp [edi+3DCh], eax
0x004079EB  push esi
0x004079EC  mov [esp+1Ch+var_10], ebp
0x004079F0  mov [esp+1Ch+var_8], eax
0x004079F4  jle loc_407ABB
0x004079FA  lea ebx, [edi+138h]
0x00407A00  mov [esp+1Ch+var_C], ebx
0x00407A04  mov esi, [ebx+298h]
0x00407A0A  mov cl, [esi+18h]
0x00407A0D  not cl
0x00407A0F  test cl, 1
0x00407A12  jnz loc_407AF4
0x00407A18  mov edx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00407A1E  mov eax, [edx+5BBC0h]
0x00407A24  sub eax, [ebx]
0x00407A26  cmp eax, 1Eh
0x00407A29  jl loc_407A9D
0x00407A2B  fild
0x00407A31  mov ecx, [esi+1Ch]
0x00407A34  movzx edx, word ptr [ecx+28h]
0x00407A38  mov [esp+1Ch+var_4], edx
0x00407A3C  fmul                                         ; -> 0x0041D8E0 in .rdata
0x00407A42  fadd                                         ; -> 0x0041D8D8 in .rdata
0x00407A48  fild
0x00407A4C  fmulp st(1), st
0x00407A4E  call __ftol2_sse                             ; -> __ftol2_sse
0x00407A53  cmp [ebp+4], eax
0x00407A56  jl loc_407A9D
0x00407A58  cmp dword ptr [ebp+8], 0
0x00407A5C  jl loc_407A9D
0x00407A5E  mov ebp, [edi+4E0h]
0x00407A64  xor eax, eax
0x00407A66  test ebp, ebp
0x00407A68  jle loc_407A99
0x00407A6A  lea edx, [edi+3E0h]
0x00407A70  mov ecx, [edx]
0x00407A72  mov ebx, [ecx+1Ch]
0x00407A75  test word ptr [ebx], 2000h
0x00407A7A  jz loc_407A8B
0x00407A7C  test byte ptr [ecx+18h], 10h
0x00407A80  jnz loc_407A8B
0x00407A82  cmp dword ptr [ecx+29Ch], 0
0x00407A89  jle loc_407AC2
0x00407A8B  add eax, 1
0x00407A8E  add edx, 4
0x00407A91  cmp eax, ebp
0x00407A93  jl loc_407A70
0x00407A95  mov ebx, [esp+1Ch+var_C]
0x00407A99  mov ebp, [esp+1Ch+var_10]
0x00407A9D  mov eax, [esp+1Ch+var_8]
0x00407AA1  add eax, 1
0x00407AA4  add ebx, 4
0x00407AA7  cmp eax, [edi+3DCh]
0x00407AAD  mov [esp+1Ch+var_8], eax
0x00407AB1  mov [esp+1Ch+var_C], ebx
0x00407AB5  jl loc_407A04
0x00407ABB  pop esi
0x00407ABC  pop ebp
0x00407ABD  pop ebx
0x00407ABE  add esp, 10h
0x00407AC1  retn
0x00407AC2  mov esi, [esi+4]
0x00407AC5  xor edx, edx
0x00407AC7  test esi, esi
0x00407AC9  setz dl
0x00407ACC  push 0
0x00407ACE  push esi
0x00407ACF  lea eax, [ecx+8]
0x00407AD2  lea edx, [edx+edx+2]
0x00407AD6  push edx
0x00407AD7  push 10h
0x00407AD9  push ecx
0x00407ADA  mov edx, edi
0x00407ADC  call sub_4046B0                              ; -> sub_4046B0 [DispatchCommandPacket]
0x00407AE1  mov eax, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00407AE6  mov ecx, [eax+5BBC0h]
0x00407AEC  mov ebx, [esp+1Ch+var_C]
0x00407AF0  mov [ebx], ecx
0x00407AF2  jmp loc_407A99
0x00407AF4  push eax
0x00407AF5  push edi
0x00407AF6  mov eax, esi
0x00407AF8  call sub_406EF0                              ; -> sub_406EF0 [Entity_CheckAndSerializeCommand]
0x00407AFD  push edi
0x00407AFE  call sub_4070A0                              ; -> sub_4070A0 [CUnit::EvaluateCombatActionOrAcquireTarget]
0x00407B03  cmp byte ptr [esi+70h], 0Dh
0x00407B07  jz loc_407A9D
0x00407B09  push esi
0x00407B0A  push edi
0x00407B0B  call sub_4071C0                              ; -> sub_4071C0 [EvaluateEntityAbilityCriteriaAndCast]
0x00407B10  jmp loc_407A9D
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
