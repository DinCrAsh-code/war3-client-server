# Function summary: sub_406EF0 (0x00406EF0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00406EF0
- Name: sub_406EF0 (auto-generated placeholder)
- Size: 427 bytes, 117 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_4079D0 -> sub_406EF0

## How callers use this function

### From sub_4079D0 (0x004079D0) - call at 0x00407AF8

What that caller does overall (preliminary): Iterates over an entity's internal collection/sub-components (up to a count at [edi+3DCh]), checking timers, state flags, and conditions to perform unit/ability/spell targeting or casting logic by invoking processing and action routines.

Called during sub-component validation checks when an internal flag test fails (specifically when bit 0 of byte [esi+18h] is set), passing the entity pointer and a component-related pointer to perform cleanup or state resetting.

## Functions it calls or references

### sub_418AA0 [BitStream_WriteBits] (0x00418AA0) - call at 0x00407062

- Prototype: `void __fastcall BitStream_WriteBits(BitStream *stream, int value, int count)`
- Summary: Writes a specified number of bits from an integer value into a bitstream buffer, updating bit offsets and tracking overflow status.

### sub_4087D0 [SerializeCommandPacket] (0x004087D0) - call at 0x0040706E

- Prototype: `bool __fastcall SerializeCommandPacket(BitStream *stream, const CommandPacket *packet)`
- Summary: Serializes a complex command or action packet structure into a bitstream using bit-packing, varints, and normalized floats.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00406EF0  sub esp, 48h
0x00406EF3  cmp dword ptr [eax+258h], 0
0x00406EFA  push ebp
0x00406EFB  push esi
0x00406EFC  push edi
0x00406EFD  jle loc_407092
0x00406F03  mov ecx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00406F09  mov edx, [ecx+5BBC0h]
0x00406F0F  mov ecx, [esp+54h+arg_0]
0x00406F13  mov esi, [esp+54h+arg_4]
0x00406F17  sub edx, [ecx+esi*4+12Ch]
0x00406F1E  cmp edx, 1Eh
0x00406F21  jl loc_407092
0x00406F27  movzx edx, byte ptr [eax+261h]
0x00406F2E  movzx esi, byte ptr [eax+262h]
0x00406F35  movzx ecx, byte ptr [eax+260h]
0x00406F3C  lea ebp, [eax+260h]
0x00406F42  add esi, edx
0x00406F44  mov dl, [eax+5Ch]
0x00406F47  add esi, ecx
0x00406F49  xor edi, edi
0x00406F4B  mov ecx, offset asc_41BD00                   ; -> 0x0041BD00 in .rdata
0x00406F50  cmp [ecx], dl
0x00406F52  jz loc_406F64
0x00406F54  add ecx, 20h
0x00406F57  add edi, 1
0x00406F5A  cmp ecx, offset byte_41BDC0                  ; -> 0x0041BDC0 in .rdata
0x00406F60  jl loc_406F50
0x00406F62  xor edi, edi
0x00406F64  test esi, esi
0x00406F66  jge loc_406F6C
0x00406F68  xor esi, esi
0x00406F6A  jmp loc_406F76
0x00406F6C  cmp esi, 9
0x00406F6F  jle loc_406F76
0x00406F71  mov esi, 9
0x00406F76  lea edx, [edi+edi*4]
0x00406F79  movzx ecx, ds:byte_41CE18[esi+edx*2]         ; -> 0x0041CE18 in .rdata
0x00406F81  cmp ecx, 2
0x00406F84  ja loc_406FAD
0x00406F86  cmp dword ptr [eax+24Ch], 0
0x00406F8D  jz loc_406FAD
0x00406F8F  movzx edx, byte ptr [ecx+eax+260h]
0x00406F97  cmp edx, 3
0x00406F9A  jge loc_406FAD
0x00406F9C  lea edx, [edx+edx+1]
0x00406FA0  cmp [eax+250h], edx
0x00406FA6  setnl dl
0x00406FA9  test dl, dl
0x00406FAB  jnz loc_406FF8
0x00406FAD  mov edi, 0FFFFFDA0h
0x00406FB2  xor esi, esi
0x00406FB4  mov ecx, ebp
0x00406FB6  sub edi, eax
0x00406FB8  lea edx, [edi+ecx]
0x00406FBB  cmp edx, 2
0x00406FBE  ja loc_406FE2
0x00406FC0  cmp dword ptr [eax+24Ch], 0
0x00406FC7  jz loc_406FE2
0x00406FC9  movzx edx, byte ptr [ecx]
0x00406FCC  cmp edx, 3
0x00406FCF  jge loc_406FE2
0x00406FD1  lea edx, [edx+edx+1]
0x00406FD5  cmp [eax+250h], edx
0x00406FDB  setnl dl
0x00406FDE  test dl, dl
0x00406FE0  jnz loc_406FF6
0x00406FE2  add esi, 1
0x00406FE5  add ecx, 1
0x00406FE8  cmp esi, 3
0x00406FEB  jl loc_406FB8
0x00406FED  pop edi
0x00406FEE  pop esi
0x00406FEF  pop ebp
0x00406FF0  add esp, 48h
0x00406FF3  retn 8
0x00406FF6  mov ecx, esi
0x00406FF8  mov edx, [esp+54h+arg_0]
0x00406FFC  mov dl, [edx]
0x00406FFE  mov edi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00407004  cmp dword ptr [edi+5472Ch], 3C00h
0x0040700E  mov [esp+54h+var_47], dl
0x00407012  mov edx, [eax+4]
0x00407015  mov [esp+54h+var_44], edx
0x00407019  mov edx, [eax+0Ch]
0x0040701C  mov [esp+54h+var_12], cx
0x00407021  mov ecx, [eax+8]
0x00407024  mov [esp+54h+var_48], 1
0x00407029  mov [esp+54h+var_46], 1
0x0040702E  mov [esp+54h+var_45], 0
0x00407033  mov [esp+54h+var_14], 13h
0x00407038  mov [esp+54h+var_13], 0
0x0040703D  mov [esp+54h+var_10], 0
0x00407045  mov [esp+54h+var_C], ecx
0x00407049  mov [esp+54h+var_8], edx
0x0040704D  jbe loc_407058
0x0040704F  add dword ptr [edi+5473Ch], 1
0x00407056  jmp loc_40707D
0x00407058  push 1
0x0040705A  lea eax, [edi+54724h]
0x00407060  push 1
0x00407062  call sub_418AA0                              ; -> sub_418AA0 [BitStream_WriteBits]
0x00407067  lea ecx, [esp+54h+var_48]
0x0040706B  push ecx
0x0040706C  mov esi, eax
0x0040706E  call sub_4087D0                              ; -> sub_4087D0 [SerializeCommandPacket]
0x00407073  add esp, 4
0x00407076  add dword ptr [edi+54734h], 1
0x0040707D  mov edx, [edi+5BBC0h]
0x00407083  mov eax, [esp+54h+arg_0]
0x00407087  mov ecx, [esp+54h+arg_4]
0x0040708B  mov [eax+ecx*4+12Ch], edx
0x00407092  pop edi
0x00407093  pop esi
0x00407094  pop ebp
0x00407095  add esp, 48h
0x00407098  retn 8
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
