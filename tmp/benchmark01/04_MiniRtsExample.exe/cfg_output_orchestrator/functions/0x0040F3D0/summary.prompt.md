# Function summary: sub_40F3D0 (0x0040F3D0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x0040F3D0
- Name: sub_40F3D0 (auto-generated placeholder)
- Size: 229 bytes, 77 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_408C10 -> sub_408D70 -> sub_40F3D0

## How callers use this function

### From sub_408D70 (0x00408D70) - call at 0x00408E9C

What that caller does overall (preliminary): Iterates over a collection of handles or entries associated with a resource structure, validates each entry against global tables and state flags, dispatches actions based on a command/type code, and updates success/failure counters in a manager object.

Called inside the main loop for operation type 1, passing `edi` (word from [ebp+36h]) after verifying a flag on an internal sub-structure, to perform the requested command-specific action and increment a success counter.

## Functions it calls or references

### sub_411580 [RtsComponentFactory_Create] (0x00411580) - call at 0x0040F465

- Prototype: `void* __fastcall RtsComponentFactory_Create(void *arg_ebx, int arg_edx, int type_index, int arg_edi)`
- Summary: Factory function that allocates and initializes various RTS component or sub-component objects based on a type index lookup table.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x0040F3D0  push ebx
0x0040F3D1  push edi
0x0040F3D2  mov edi, eax
0x0040F3D4  cmp edi, 2
0x0040F3D7  mov ebx, ecx
0x0040F3D9  ja loc_40F40D
0x0040F3DB  cmp dword ptr [ebx+258h], 0
0x0040F3E2  jle loc_40F40D
0x0040F3E4  cmp dword ptr [ebx+24Ch], 0
0x0040F3EB  jz loc_40F40D
0x0040F3ED  mov al, [edi+ebx+260h]
0x0040F3F4  movzx ecx, al
0x0040F3F7  cmp ecx, 3
0x0040F3FA  jge loc_40F40D
0x0040F3FC  lea ecx, [ecx+ecx+1]
0x0040F400  cmp [ebx+250h], ecx
0x0040F406  setnl cl
0x0040F409  test cl, cl
0x0040F40B  jnz loc_40F412
0x0040F40D  pop edi
0x0040F40E  xor al, al
0x0040F410  pop ebx
0x0040F411  retn
0x0040F412  add al, 1
0x0040F414  mov [edi+ebx+260h], al
0x0040F41B  mov edx, [ebx+24Ch]
0x0040F421  add dword ptr [ebx+258h], 0FFFFFFFFh
0x0040F428  push ebp
0x0040F429  push esi
0x0040F42A  movzx esi, byte ptr [edx+edi+2]
0x0040F42F  mov dl, [ebx+5Eh]
0x0040F432  xor eax, eax
0x0040F434  test dl, dl
0x0040F436  jbe loc_40F458
0x0040F438  lea ecx, [ebx+210h]
0x0040F43E  mov edi, edi
0x0040F440  mov ebp, [ecx]
0x0040F442  movzx ebp, byte ptr [ebp+10h]
0x0040F446  cmp ebp, esi
0x0040F448  jz loc_40F493
0x0040F44A  movzx ebp, byte ptr [ebx+5Eh]
0x0040F44E  add eax, 1
0x0040F451  add ecx, 4
0x0040F454  cmp eax, ebp
0x0040F456  jl loc_40F440
0x0040F458  cmp dl, 5
0x0040F45B  movzx edi, byte ptr [edi+ebx+260h]
0x0040F463  jnb loc_40F486
0x0040F465  call sub_411580                              ; -> sub_411580 [RtsComponentFactory_Create]
0x0040F46A  test eax, eax
0x0040F46C  jz loc_40F486
0x0040F46E  movzx edx, byte ptr [ebx+5Eh]
0x0040F472  mov [ebx+edx*4+210h], eax
0x0040F479  add byte ptr [ebx+5Eh], 1
0x0040F47D  mov edx, [eax]
0x0040F47F  mov ecx, eax
0x0040F481  mov eax, [edx+4]
0x0040F484  call eax
0x0040F486  or word ptr [ebx+18h], 400h
0x0040F48C  pop esi
0x0040F48D  pop ebp
0x0040F48E  pop edi
0x0040F48F  mov al, 1
0x0040F491  pop ebx
0x0040F492  retn
0x0040F493  mov eax, [ebx+eax*4+210h]
0x0040F49A  test eax, eax
0x0040F49C  jz loc_40F458
0x0040F49E  mov cl, [edi+ebx+260h]
0x0040F4A5  pop esi
0x0040F4A6  pop ebp
0x0040F4A7  mov [eax+11h], cl
0x0040F4AA  or word ptr [ebx+18h], 400h
0x0040F4B0  pop edi
0x0040F4B1  mov al, 1
0x0040F4B3  pop ebx
0x0040F4B4  retn
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
