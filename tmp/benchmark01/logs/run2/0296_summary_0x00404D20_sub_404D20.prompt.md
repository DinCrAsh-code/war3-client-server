# Function summary: sub_404D20 (0x00404D20)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00404D20
- Name: sub_404D20 (auto-generated placeholder)
- Size: 206 bytes, 71 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404D20

## How callers use this function

### From sub_403460 (0x00403460) - call at 0x004034AB

What that caller does overall (preliminary): Iterates over a collection of simulation or resource entities (up to a count at [esi+5BBBCh], stepping by 4FCh bytes from offset 0x630), performing modulo-10 and flag-based checks on each element before invoking a batch of update/processing routines.

Called with `eax` set to `edi` (the current entity structure) as part of the sequence of update routines for the filtered entity.

## Functions it calls or references

### sub_405300 [Entity::TryPerformActionOrCommand] (0x00405300) - call at 0x00404DE1

- Prototype: `bool __fastcall Entity::TryPerformActionOrCommand(void *pEntity, void *edx_unused, int actionIndex)`
- Summary: Validates timing cooldowns, resource/structure criteria, geometric placement, and target availability for an entity action, and if successful, dispatches a command packet and updates the action cooldown timestamp.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00404D20  sub esp, 0Ch
0x00404D23  push ebx
0x00404D24  push ebp
0x00404D25  push esi
0x00404D26  push edi
0x00404D27  mov edi, eax
0x00404D29  mov ecx, [edi]
0x00404D2B  imul ecx
0x00404D31  add ecx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00404D37  mov ebp, [ecx+10h]
0x00404D3A  cmp ebp, 64h
0x00404D3D  mov [esp+1Ch+var_4], ebp
0x00404D41  jge loc_404DE6
0x00404D47  mov bl, [ecx+1]
0x00404D4A  neg bl
0x00404D4C  sbb ebx, ebx
0x00404D4E  and ebx, 14h
0x00404D51  add ebx, 0Ch
0x00404D54  cmp ebx, 30h
0x00404D57  mov [esp+1Ch+var_8], ebx
0x00404D5B  ja loc_404D6D
0x00404D5D  mov eax, ebx
0x00404D5F  shl eax, 4
0x00404D62  sub eax, ebx
0x00404D64  lea eax, ds:41B178h[eax*4]                   ; -> 0x0041B178 in .rdata
0x00404D6B  jmp loc_404D72
0x00404D6D  mov eax, offset byte_41B178                  ; -> 0x0041B178 in .rdata
0x00404D72  movsx edx, byte ptr [eax+2Eh]
0x00404D76  mov eax, [edi+244h]
0x00404D7C  movzx esi, word ptr [ecx+ebx*2+76h]
0x00404D81  neg edx
0x00404D83  test eax, eax
0x00404D85  jle loc_404DB6
0x00404D87  lea ebp, [edi+144h]
0x00404D8D  lea ecx, [ecx+0]
0x00404D90  mov ebx, [ebp+0]
0x00404D93  cmp byte ptr [ebx+70h], 0Ah
0x00404D97  jnz loc_404DA6
0x00404D99  movzx ebx, word ptr [ebx+72h]
0x00404D9D  cmp ebx, [esp+1Ch+var_8]
0x00404DA1  jnz loc_404DA6
0x00404DA3  add esi, 1
0x00404DA6  add ebp, 4
0x00404DA9  sub eax, 1
0x00404DAC  jnz loc_404D90
0x00404DAE  mov ebx, [esp+1Ch+var_8]
0x00404DB2  mov ebp, [esp+1Ch+var_4]
0x00404DB6  mov eax, esi
0x00404DB8  imul eax, edx
0x00404DBB  sub eax, [ecx+0Ch]
0x00404DBE  add eax, ebp
0x00404DC0  mov ecx, eax
0x00404DC2  mov eax, [edi+4E0h]
0x00404DC8  cdq
0x00404DC9  and edx, 3
0x00404DCC  add eax, edx
0x00404DCE  sar eax, 2
0x00404DD1  add eax, 4
0x00404DD4  cmp ecx, eax
0x00404DD6  jg loc_404DE6
0x00404DD8  cmp esi, 2
0x00404DDB  jge loc_404DE6
0x00404DDD  push 0
0x00404DDF  mov esi, ebx
0x00404DE1  call sub_405300                              ; -> sub_405300 [Entity::TryPerformActionOrCommand]
0x00404DE6  pop edi
0x00404DE7  pop esi
0x00404DE8  pop ebp
0x00404DE9  pop ebx
0x00404DEA  add esp, 0Ch
0x00404DED  retn
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
