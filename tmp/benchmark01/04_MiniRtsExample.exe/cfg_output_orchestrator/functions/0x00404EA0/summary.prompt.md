# Function summary: sub_404EA0 (0x00404EA0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00404EA0
- Name: sub_404EA0 (auto-generated placeholder)
- Size: 383 bytes, 129 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_405040 -> sub_404EA0

## How callers use this function

### From sub_405040 (0x00405040) - call at 0x004050CE

What that caller does overall (preliminary): Evaluates or processes an entity/object state during simulation updates, iterating through configuration data structures or lookup tables based on object properties and calling sub_404EA0 in a loop to test or apply conditions.

Invoked in a loop over entity configuration items (up to an index limit determined by local flags and count structures) with the entity pointer in `eax` and a calculated parameter pushed on the stack. The return value in `al` dictates whether the loop continues to the next iteration or terminates early.

## Functions it calls or references

### sub_404DF0 [CheckEntityConditionsOrCount] (0x00404DF0) - call at 0x00404ECF, 0x00404F0A

- Prototype: `int __stdcall CheckEntityConditionsOrCount(void *entity, int targetId)`
- Summary: Queries entity state and loops through a list of sub-elements to calculate an accumulated count or conditional metric, combined with a timing/distance check based on a global data structure.

### sub_405300 [Entity::TryPerformActionOrCommand] (0x00405300) - call at 0x00404EF4, 0x00404F48

- Prototype: `bool __fastcall Entity::TryPerformActionOrCommand(void *pEntity, void *edx_unused, int actionIndex)`
- Summary: Validates timing cooldowns, resource/structure criteria, geometric placement, and target availability for an entity action, and if successful, dispatches a command packet and updates the action cooldown timestamp.

### sub_405520 [CheckEntityPlacementOrActionCriteria] (0x00405520) - call at 0x00404F35

- Prototype: `bool __stdcall CheckEntityPlacementOrActionCriteria(void *pEntity, int arg_4, Vector2 *outPos)`
- Summary: Evaluates complex geometric, proximity, and grid placement/action criteria for an entity, checking surrounding entities and map grid regions before validating target coordinates.

### sub_417EB0 [GetConfigTableEntry] (0x00417EB0) - call at 0x00404F80

- Prototype: `const void* __cdecl GetConfigTableEntry(int index)`
- Summary: Retrieves a pointer to a configuration structure or entry from a static table in .rdata based on an input index.

### sub_404260 [ValidateCapacityOrLimit] (0x00404260) - call at 0x00404F8C

- Prototype: `bool __fastcall ValidateCapacityOrLimit(void *entity, int arg_0)`
- Summary: Validates whether a calculated value based on an entity's fields and an argument is within a capped limit (0x64), returning a boolean gating further execution.

### sub_405110 [CheckAndExecuteUnitAbility] (0x00405110) - call at 0x00404F97, 0x00404FC9

- Prototype: `bool __stdcall CheckAndExecuteUnitAbility(void *entity, int abilityIndex)`
- Summary: Validates resource cooldowns, unit requirements, and target constraints for a specific ability/action on an entity, and dispatches a command packet if all checks pass.

### sub_405230 [CheckAndDispatchAction] (0x00405230) - call at 0x00404FFD

- Prototype: `char __fastcall CheckAndDispatchAction(void *entity, void *edx_unused, int actionIndex)`
- Summary: Validates command cooldown timers, resource requirements, and target state for a specific action index, returning false if conditions fail or dispatching a command packet via DispatchCommandPacket and updating the cooldown timestamp on success.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00404EA0  sub esp, 0Ch
0x00404EA3  push ebp
0x00404EA4  push esi
0x00404EA5  push edi
0x00404EA6  mov edi, eax
0x00404EA8  mov esi, [edi]
0x00404EAA  movzx eax, byte ptr [ebx]
0x00404EAD  imul esi
0x00404EB3  add esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00404EB9  cmp eax, 5
0x00404EBC  ja def_404EC2
0x00404EC2  jmp ds:jpt_404EC2[eax*4]                     ; -> 0x00405020 in .text
0x00404EC9  movzx ebp, byte ptr [ebx+1]
0x00404ECD  push ebp
0x00404ECE  push edi
0x00404ECF  call sub_404DF0                              ; -> sub_404DF0 [CheckEntityConditionsOrCount]
0x00404ED4  movzx ecx, word ptr [esi+ebp*2+76h]
0x00404ED9  movzx edx, word ptr [esi+ebp*2+14h]
0x00404EDE  add eax, ecx
0x00404EE0  movzx ecx, byte ptr [ebx+2]
0x00404EE4  add eax, edx
0x00404EE6  cmp eax, ecx
0x00404EE8  jge def_404EC2
0x00404EEE  movzx esi, byte ptr [ebx+1]
0x00404EF2  push 0
0x00404EF4  call sub_405300                              ; -> sub_405300 [Entity::TryPerformActionOrCommand]
0x00404EF9  xor al, al
0x00404EFB  pop edi
0x00404EFC  pop esi
0x00404EFD  pop ebp
0x00404EFE  add esp, 0Ch
0x00404F01  retn 4
0x00404F04  movzx ebp, byte ptr [ebx+1]
0x00404F08  push ebp
0x00404F09  push edi
0x00404F0A  call sub_404DF0                              ; -> sub_404DF0 [CheckEntityConditionsOrCount]
0x00404F0F  movzx edx, word ptr [esi+ebp*2+76h]
0x00404F14  movzx ecx, word ptr [esi+ebp*2+14h]
0x00404F19  add eax, edx
0x00404F1B  movzx edx, byte ptr [ebx+2]
0x00404F1F  add eax, ecx
0x00404F21  cmp eax, edx
0x00404F23  jge def_404EC2
0x00404F29  lea eax, [esp+18h+var_8]
0x00404F2D  push eax
0x00404F2E  movzx eax, byte ptr [ebx+1]
0x00404F32  push 1
0x00404F34  push edi
0x00404F35  call sub_405520                              ; -> sub_405520 [CheckEntityPlacementOrActionCriteria]
0x00404F3A  test al, al
0x00404F3C  jz def_404EC2
0x00404F42  movzx esi, byte ptr [ebx+1]
0x00404F46  push 1
0x00404F48  call sub_405300                              ; -> sub_405300 [Entity::TryPerformActionOrCommand]
0x00404F4D  xor al, al
0x00404F4F  pop edi
0x00404F50  pop esi
0x00404F51  pop ebp
0x00404F52  add esp, 0Ch
0x00404F55  retn 4
0x00404F58  movzx ebp, byte ptr [ebx+1]
0x00404F5C  movzx eax, byte ptr [ebx+2]
0x00404F60  movzx ecx, word ptr [esi+ebp*2+76h]
0x00404F65  movzx edx, word ptr [esi+ebp*2+14h]
0x00404F6A  add eax, [esp+18h+arg_0]
0x00404F6E  add ecx, edx
0x00404F70  cmp ecx, eax
0x00404F72  jge def_404EC2
0x00404F78  cmp dword ptr [esi+10h], 64h
0x00404F7C  jl loc_404F95
0x00404F7E  mov eax, ebp
0x00404F80  call sub_417EB0                              ; -> sub_417EB0 [GetConfigTableEntry]
0x00404F85  movsx ecx, byte ptr [eax+2Eh]
0x00404F89  push ecx
0x00404F8A  mov ecx, esi
0x00404F8C  call sub_404260                              ; -> sub_404260 [ValidateCapacityOrLimit]
0x00404F91  test al, al
0x00404F93  jz def_404EC2
0x00404F95  push ebp
0x00404F96  push edi
0x00404F97  call sub_405110                              ; -> sub_405110 [CheckAndExecuteUnitAbility]
0x00404F9C  xor al, al
0x00404F9E  pop edi
0x00404F9F  pop esi
0x00404FA0  pop ebp
0x00404FA1  add esp, 0Ch
0x00404FA4  retn 4
0x00404FA7  movzx edx, byte ptr [ebx+2]
0x00404FAB  cmp [esi+0F4h], edx
0x00404FB1  jge def_404EC2
0x00404FB3  movzx eax, byte ptr [ebx+1]
0x00404FB7  movzx ecx, word ptr [esi+eax*2+76h]
0x00404FBC  movzx edx, word ptr [esi+eax*2+14h]
0x00404FC1  add ecx, edx
0x00404FC3  test ecx, ecx
0x00404FC5  jg def_404EC2
0x00404FC7  push eax
0x00404FC8  push edi
0x00404FC9  call sub_405110                              ; -> sub_405110 [CheckAndExecuteUnitAbility]
0x00404FCE  xor al, al
0x00404FD0  pop edi
0x00404FD1  pop esi
0x00404FD2  pop ebp
0x00404FD3  add esp, 0Ch
0x00404FD6  retn 4
0x00404FD9  movzx eax, byte ptr [ebx+1]
0x00404FDD  movzx ecx, byte ptr [eax+esi+0E0h]
0x00404FE5  movzx edx, byte ptr [eax+esi+0D8h]
0x00404FED  add eax, esi
0x00404FEF  movzx eax, byte ptr [ebx+2]
0x00404FF3  add ecx, edx
0x00404FF5  cmp ecx, eax
0x00404FF7  jge def_404EC2
0x00404FF9  movzx esi, byte ptr [ebx+1]
0x00404FFD  call sub_405230                              ; -> sub_405230 [CheckAndDispatchAction]
0x00405002  xor al, al
0x00405004  pop edi
0x00405005  pop esi
0x00405006  pop ebp
0x00405007  add esp, 0Ch
0x0040500A  retn 4
0x0040500D  movzx ecx, byte ptr [ebx+3]
0x00405011  mov [edi+3Ch], ecx
0x00405014  pop edi
0x00405015  pop esi
0x00405016  mov al, 1
0x00405018  pop ebp
0x00405019  add esp, 0Ch
0x0040501C  retn 4
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
