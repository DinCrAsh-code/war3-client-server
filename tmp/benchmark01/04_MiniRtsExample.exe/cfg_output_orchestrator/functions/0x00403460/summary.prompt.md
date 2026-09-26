# Function summary: sub_403460 (0x00403460)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00403460
- Name: sub_403460 (auto-generated placeholder)
- Size: 128 bytes, 41 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460

## How callers use this function

### From sub_403D30 (0x00403D30) - call at 0x00403D8E

What that caller does overall (preliminary): Executes a single step or tick of a simulation/update phase using the resource structure provided in ebp (passed from sub_403E50), periodically updating sub-components, checking step-based intervals (dividing counters by 5 and 10), and handling state or resource blocks.

Called unconditionally in sequence after sub_4033A0, receiving the resource pointer in esi (ebp).

## Functions it calls or references

### sub_404940 [UpdateEntitySelectionAndLists] (0x00404940) - call at 0x0040349F

- Prototype: `void __cdecl UpdateEntitySelectionAndLists(Entity *entityStruct)`
- Summary: Iterates through a global list of game units/entities, filtering and categorizing them into various sub-lists and tracking counts within the specified entity structure.

### sub_404AD0 [Entity::ProcessCombatOrAbilityUpdate] (0x00404AD0) - call at 0x004034A4

- Prototype: `void __fastcall Entity::ProcessCombatOrAbilityUpdate(void *pEntity)`
- Summary: Evaluates combat proximity and ability conditions for an entity, dispatching targeted command packets or triggering unit abilities and actions based on distance metrics and state criteria.

### sub_404D20 [Entity::ProcessAIOrProductionAction] (0x00404D20) - call at 0x004034AB

- Prototype: `bool __fastcall Entity::ProcessAIOrProductionAction(void *pEntity, void *edx_unused)`
- Summary: Evaluates entity-specific production or action progress and criteria, and potentially triggers a command via Entity::TryPerformActionOrCommand.

### sub_405040 [Entity::ProcessTimedBehaviorsOrActions] (0x00405040) - call at 0x004034B1

- Prototype: `bool __fastcall Entity::ProcessTimedBehaviorsOrActions(void *pEntity)`
- Summary: Iterates through a configuration-driven sequence of state actions or behavior frames for an entity, using Entity::ProcessStateTransitionOrAction, and updates timing counters or loop indices upon success or special termination conditions.

### sub_4079D0 [CUnit::EvaluateAbilitiesAndDispatchCommands] (0x004079D0) - call at 0x004034B6

- Prototype: `void __fastcall CUnit::EvaluateAbilitiesAndDispatchCommands(void *entity, void *edx_unused)`
- Summary: Iterates over a collection of sub-components or queued capabilities within an entity, validating timer thresholds, target/combat conditions, and rate limits to serialize and dispatch command packets.

### sub_406760 [CUnit::UpdateAIStateOrBehavior] (0x00406760) - call at 0x004034BD

- Prototype: `void __fastcall CUnit::UpdateAIStateOrBehavior(void *entity)`
- Summary: Evaluates and updates an entity's high-level state machine or behavior mode (stored at offset +0x0C) by checking environmental conditions, proximity values, target validity, and executing corresponding command batches.

### sub_406C90 [ProcessEntityCommandBatchOrSpatialQuery] (0x00406C90) - call at 0x004034C3

- Prototype: `void __stdcall ProcessEntityCommandBatchOrSpatialQuery(void *entity)`
- Summary: Inspects an entity's sub-collection of pointers, serializes command batches via SerializeCommandBatch based on internal item flags and counts, and performs a spatial grid query using SpatialGridQueryEntities to process nearby targets.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00403460  push ebx
0x00403461  push ebp
0x00403462  xor ebp, ebp
0x00403464  cmp [esi+5BBBCh], ebp
0x0040346A  push edi
0x0040346B  jle loc_4034DC
0x0040346D  xor ebx, ebx
0x0040346F  lea edi, [esi+630h]
0x00403475  mov eax, [esi+5BBC0h]
0x0040347B  add eax, ebx
0x0040347D  cdq
0x0040347E  mov ecx, 0Ah
0x00403483  idiv
0x00403485  test edx, edx
0x00403487  jnz loc_4034C8
0x00403489  mov edx, [edi]
0x0040348B  mov eax, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00403490  imul edx
0x00403496  test byte ptr [edx+eax+3], 4
0x0040349B  jnz loc_4034C8
0x0040349D  mov eax, edi
0x0040349F  call sub_404940                              ; -> sub_404940 [UpdateEntitySelectionAndLists]
0x004034A4  call sub_404AD0                              ; -> sub_404AD0 [Entity::ProcessCombatOrAbilityUpdate]
0x004034A9  mov eax, edi
0x004034AB  call sub_404D20                              ; -> sub_404D20 [Entity::ProcessAIOrProductionAction]
0x004034B0  push edi
0x004034B1  call sub_405040                              ; -> sub_405040 [Entity::ProcessTimedBehaviorsOrActions]
0x004034B6  call sub_4079D0                              ; -> sub_4079D0 [CUnit::EvaluateAbilitiesAndDispatchCommands]
0x004034BB  mov eax, edi
0x004034BD  call sub_406760                              ; -> sub_406760 [CUnit::UpdateAIStateOrBehavior]
0x004034C2  push edi
0x004034C3  call sub_406C90                              ; -> sub_406C90 [ProcessEntityCommandBatchOrSpatialQuery]
0x004034C8  add ebp, 1
0x004034CB  add ebx, 3
0x004034CE  add edi, 4FCh
0x004034D4  cmp ebp, [esi+5BBBCh]
0x004034DA  jl loc_403475
0x004034DC  pop edi
0x004034DD  pop ebp
0x004034DE  pop ebx
0x004034DF  retn
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
