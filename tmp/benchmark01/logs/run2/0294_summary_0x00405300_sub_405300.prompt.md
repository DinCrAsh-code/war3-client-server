# Function summary: sub_405300 (0x00405300)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00405300
- Name: sub_405300 (auto-generated placeholder)
- Size: 201 bytes, 69 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404AD0 -> sub_405300

## How callers use this function

### From sub_404EA0 (0x00404EA0) - call at 0x00404EF4, 0x00404F48

What that caller does overall (preliminary): Interprets and executes a specific command or bytecode instruction from a configuration stream (pointed to by ebx) acting on an entity/object state (pointed to by edi), validating various bounds and state conditions via a switch statement before dispatching sub-operations.

Called to update or trigger a state change on the entity/system, passing an index (from ebx+1) and a boolean/flag constant (0 or 1) as arguments.

### From sub_404D20 (0x00404D20) - call at 0x00404DE1

What that caller does overall (preliminary): Performs status, threshold, and inventory checks on a specific game entity structure passed in eax, potentially triggering a subsystem event or action via sub_405300 if certain criteria are met.

Invoked as part of a conditional trigger when the entity's computed metric falls below a maximum threshold and count criteria are met (esi < 2). It is called with a constant 0 pushed on the stack, while esi is set to the computed classification value (ebx) previously saved during entity evaluation.

### From sub_404AD0 (0x00404AD0) - call at 0x00404D10

What that caller does overall (preliminary): Iterates over a collection of sub-objects/elements stored within an entity (passed in eax), performing distance-to-target calculations or state checks to conditionally trigger sub-routines, management actions, or status updates on the entity.

Called at 0x00404D10 as part of a final conditional branch after sub_404DF0, passing an integer constant '1' as an argument.

## Functions it calls or references

### sub_405520 [CheckEntityPlacementOrActionCriteria] (0x00405520) - call at 0x0040537F

- Prototype: `bool __stdcall CheckEntityPlacementOrActionCriteria(void *pEntity, int arg_4, Vector2 *outPos)`
- Summary: Evaluates complex geometric, proximity, and grid placement/action criteria for an entity, checking surrounding entities and map grid regions before validating target coordinates.

### sub_4053D0 [Entity::FindTargetOrValidSubEntity] (0x004053D0) - call at 0x0040538F

- Prototype: `void* __fastcall Entity::FindTargetOrValidSubEntity(void *this, void *edx, float threshold)`
- Summary: Iterates through an entity's sub-components or list of associated elements, performs distance and state validation checks, and returns the first matching element that satisfies the criteria.

### sub_4046B0 [DispatchCommandPacket] (0x004046B0) - call at 0x004053A6

- Prototype: `void __fastcall DispatchCommandPacket(void *entity, void *edx_unused, void *sourceData, int arg0, char arg4, char arg8, int argC, short arg10)`
- Summary: Constructs and serializes an outgoing network command packet using a local bitstream buffer, checking global packet-rate limits before transmission.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00405300  sub esp, 8
0x00405303  push ebx
0x00405304  push ebp
0x00405305  mov ebp, dword_42AC60                        ; -> 0x0042AC60 in .data
0x0040530B  mov eax, [ebp+5BBC0h]
0x00405311  sub eax, [edi+esi*4+48h]
0x00405315  cmp eax, 1Eh
0x00405318  jl loc_4053BF
0x0040531E  mov ecx, [edi]
0x00405320  imul ecx
0x00405326  add ecx, ebp
0x00405328  cmp esi, 30h
0x0040532B  ja loc_40533D
0x0040532D  mov edx, esi
0x0040532F  shl edx, 4
0x00405332  sub edx, esi
0x00405334  lea ebx, ds:41B178h[edx*4]                   ; -> 0x0041B178 in .rdata
0x0040533B  jmp loc_405342
0x0040533D  mov ebx, offset byte_41B178                  ; -> 0x0041B178 in .rdata
0x00405342  xor edx, edx
0x00405344  movzx eax, byte ptr [ebx+edx+38h]
0x00405349  cmp eax, 0FFh
0x0040534E  jz loc_405358
0x00405350  cmp word ptr [ecx+eax*2+14h], 0
0x00405356  jz loc_4053BF
0x00405358  add edx, 1
0x0040535B  cmp edx, 2
0x0040535E  jl loc_405344
0x00405360  movzx eax, word ptr [ebx+28h]
0x00405364  cmp [ecx+4], eax
0x00405367  jl loc_4053BF
0x00405369  movzx edx, word ptr [ebx+2Ah]
0x0040536D  cmp [ecx+8], edx
0x00405370  jl loc_4053BF
0x00405372  mov ecx, [esp+10h+arg_0]
0x00405376  lea eax, [esp+10h+var_8]
0x0040537A  push eax
0x0040537B  push ecx
0x0040537C  push edi
0x0040537D  mov eax, esi
0x0040537F  call sub_405520                              ; -> sub_405520 [CheckEntityPlacementOrActionCriteria]
0x00405384  test al, al
0x00405386  jz loc_4053BF
0x00405388  lea edx, [esp+10h+var_8]
0x0040538C  push edx
0x0040538D  mov ecx, edi
0x0040538F  call sub_4053D0                              ; -> sub_4053D0 [Entity::FindTargetOrValidSubEntity]
0x00405394  test eax, eax
0x00405396  jz loc_4053BF
0x00405398  push esi
0x00405399  push 0
0x0040539B  push 4
0x0040539D  push 0Ah
0x0040539F  push eax
0x004053A0  lea eax, [esp+24h+var_8]
0x004053A4  mov edx, edi
0x004053A6  call sub_4046B0                              ; -> sub_4046B0 [DispatchCommandPacket]
0x004053AB  mov eax, [ebp+5BBC0h]
0x004053B1  mov [edi+esi*4+48h], eax
0x004053B5  mov al, 1
0x004053B7  pop ebp
0x004053B8  pop ebx
0x004053B9  add esp, 8
0x004053BC  retn 4
0x004053BF  pop ebp
0x004053C0  xor al, al
0x004053C2  pop ebx
0x004053C3  add esp, 8
0x004053C6  retn 4
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
