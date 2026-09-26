# Function summary: sub_4070A0 (0x004070A0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004070A0
- Name: sub_4070A0 (auto-generated placeholder)
- Size: 285 bytes, 106 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_4079D0 -> sub_4070A0

## How callers use this function

### From sub_4079D0 (0x004079D0) - call at 0x00407AFE

What that caller does overall (preliminary): Iterates over an entity's internal collection/sub-components (up to a count at [edi+3DCh]), checking timers, state flags, and conditions to perform unit/ability/spell targeting or casting logic by invoking processing and action routines.

Called immediately after sub_406EF0 during the alternative branch for an invalid or inactive sub-component state, passing the main entity pointer (edi).

## Functions it calls or references

### sub_4046B0 [DispatchCommandPacket] (0x004046B0) - call at 0x00407108, 0x004071B3

- Prototype: `void __fastcall DispatchCommandPacket(void *entity, void *edx_unused, void *sourceData, int arg0, char arg4, char arg8, int argC, short arg10)`
- Summary: Constructs and serializes an outgoing network command packet using a local bitstream buffer, checking global packet-rate limits before transmission.

### sub_4030C0 [FindNearestValidEntityInRange] (0x004030C0) - call at 0x0040715B

- Prototype: `void* __stdcall FindNearestValidEntityInRange(float x, float y, float max_distance)`
- Summary: Iterates through a global list of entities/objects, filtering out inactive, dead, or invalid items and finding one that satisfies specific state flags and lies within a given distance threshold.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004070A0  fld                                          ; -> 0x0041D8C8 in .rdata
0x004070A6  push ebp
0x004070A7  fld                                          ; -> 0x0041D948 in .rdata
0x004070AD  mov ebp, [esp+4+arg_0]
0x004070B1  fld                                          ; -> 0x0041D8D8 in .rdata
0x004070B7  push edi
0x004070B8  lea edi, [esi+264h]
0x004070BE  xor ecx, ecx
0x004070C0  mov edx, edi
0x004070C2  mov eax, [edx]
0x004070C4  test eax, eax
0x004070C6  jz loc_40712D
0x004070C8  movzx eax, byte ptr [eax+1Ch]
0x004070CC  sub eax, 0
0x004070CF  jz loc_40711C
0x004070D1  sub eax, 1
0x004070D4  jz loc_407112
0x004070D6  sub eax, 0Ah
0x004070D9  jnz loc_40712D
0x004070DB  fld
0x004070DE  fld
0x004070E1  fmul st, st(4)
0x004070E3  fcompp
0x004070E5  fnstsw ax
0x004070E7  test ah, 41h
0x004070EA  jnz loc_40712D
0x004070EC  cmp dword ptr [esi+1F4h], 0
0x004070F3  jz loc_40712D
0x004070F5  push ecx
0x004070F6  fstp
0x004070F8  push 0
0x004070FA  fstp
0x004070FC  push 0
0x004070FE  fstp
0x00407100  push 0Fh
0x00407102  lea eax, [esi+8]
0x00407105  push esi
0x00407106  mov edx, ebp
0x00407108  call sub_4046B0                              ; -> sub_4046B0 [DispatchCommandPacket]
0x0040710D  pop edi
0x0040710E  pop ebp
0x0040710F  retn 4
0x00407112  fld
0x00407115  fld
0x00407118  fmul st, st(3)
0x0040711A  jmp loc_407124
0x0040711C  fld
0x0040711F  fld
0x00407122  fmul st, st(2)
0x00407124  fcompp
0x00407126  fnstsw ax
0x00407128  test ah, 41h
0x0040712B  jz loc_4070F5
0x0040712D  add ecx, 1
0x00407130  add edx, 4
0x00407133  cmp ecx, 6
0x00407136  jl loc_4070C2
0x00407138  cmp dword ptr [esi+1F4h], 0
0x0040713F  fstp
0x00407141  fstp
0x00407143  fstp
0x00407145  jnz loc_4071B8
0x00407147  cmp byte ptr [esi+70h], 0Eh
0x0040714B  jz loc_4071B8
0x0040714D  fld                                          ; -> 0x0041DBEC in .rdata
0x00407153  push ecx
0x00407154  lea eax, [esi+8]
0x00407157  fstp
0x0040715A  push eax
0x0040715B  call sub_4030C0                              ; -> sub_4030C0 [FindNearestValidEntityInRange]
0x00407160  mov edx, eax
0x00407162  test edx, edx
0x00407164  jz loc_4071B8
0x00407166  xor eax, eax
0x00407168  mov ecx, edi
0x0040716A  lea ebx, [ebx+0]
0x00407170  cmp dword ptr [ecx], 0
0x00407173  jz loc_407182
0x00407175  add eax, 1
0x00407178  add ecx, 4
0x0040717B  cmp eax, 6
0x0040717E  jl loc_407170
0x00407180  jmp loc_407186
0x00407182  test eax, eax
0x00407184  jge loc_4071A3
0x00407186  movzx eax, byte ptr [edx+1Ch]
0x0040718A  xor ecx, ecx
0x0040718C  cmp eax, 0Eh
0x0040718F  setnl cl
0x00407192  sub ecx, 1
0x00407195  and ecx, eax
0x00407197  imul ecx
0x0040719A  cmp ds:byte_41C9E0[ecx], 2                   ; -> 0x0041C9E0 in .rdata
0x004071A1  jnz loc_4071B8
0x004071A3  push 0
0x004071A5  lea eax, [edx+8]
0x004071A8  mov edx, [edx+4]
0x004071AB  push edx
0x004071AC  push 8
0x004071AE  push 0Eh
0x004071B0  push esi
0x004071B1  mov edx, ebp
0x004071B3  call sub_4046B0                              ; -> sub_4046B0 [DispatchCommandPacket]
0x004071B8  pop edi
0x004071B9  pop ebp
0x004071BA  retn 4
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
