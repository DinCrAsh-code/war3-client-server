# Function summary: sub_414E40 (0x00414E40)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00414E40
- Name: sub_414E40 (auto-generated placeholder)
- Size: 347 bytes, 120 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_416640 -> sub_414E40

## How callers use this function

### From sub_40A3F0 (0x0040A3F0) - call at 0x0040A4BC

What that caller does overall (preliminary): Interprets and executes a specific graphics, layout, or positioning bytecode instruction by extracting parameters from a descriptor structure, validating bounds against a lookup table, performing math/coordinate conversions, querying resource/state managers, and storing the resulting value into an output descriptor container.

Called with a pointer to a sub-structure within global state (at esi+1A20h), a pointer to coordinates, and size 8, to query or validate a region or transform, returning a boolean result in al that conditionally gates subsequent math operations.

### From sub_402050 (0x00402050) - call at 0x0040245C

What that caller does overall (preliminary): Performs geometric, spatial, or resource item computations for an individual resource entry in a loop, querying and interacting with subsystem tables and coordinate/index bounds using floating-point math.

Called inside an outer nested loop over 5 iterations to check or update spatial resource status, passing pointer structures (including var_68 and var_60) and offset 0x1A20 of the resource block.

### From sub_416640 (0x00416640) - call at 0x004168CB

What that caller does overall (preliminary): Performs batch or iterative processing on resource/state data structures using floating-point calculations, coordinate transformations, random number updates, and lookup/validation checks.

Called inside an iterative loop over resource parameters, passing `esi`, clamped integer coordinate arguments, and a constant (0Ah) to validate or query grid/table data, using the boolean return value (`al`) to conditionally update values in a structure (`ebx`).

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00414E40  sub esp, 34h
0x00414E43  mov ecx, [esp+34h+arg_4]
0x00414E47  mov eax, [esp+34h+arg_8]
0x00414E4B  mov ecx, [ecx]
0x00414E4D  test ecx, ecx
0x00414E4F  mov edx, [eax]
0x00414E51  push esi
0x00414E52  mov [esp+38h+var_30], edx
0x00414E56  mov [esp+38h+var_2C], ecx
0x00414E5A  jl loc_414E6A
0x00414E5C  test edx, edx
0x00414E5E  jl loc_414E6A
0x00414E60  cmp ecx, 60h
0x00414E63  jge loc_414E6A
0x00414E65  cmp edx, 60h
0x00414E68  jl loc_414E6E
0x00414E6A  mov al, 23h
0x00414E6C  jmp loc_414E81
0x00414E6E  mov esi, [esp+38h+arg_0]
0x00414E72  lea eax, [edx+edx*2]
0x00414E75  shl eax, 5
0x00414E78  add eax, ecx
0x00414E7A  mov al, [eax+esi+0B704h]
0x00414E81  not al
0x00414E83  test al, 1
0x00414E85  jz loc_414E90
0x00414E87  mov al, 1
0x00414E89  pop esi
0x00414E8A  add esp, 34h
0x00414E8D  retn 10h
0x00414E90  push ebx
0x00414E91  push ebp
0x00414E92  mov ebp, 1
0x00414E97  cmp [esp+40h+arg_C], ebp
0x00414E9B  push edi
0x00414E9C  jl loc_414F6F
0x00414EA2  or esi, 0FFFFFFFFh
0x00414EA5  mov eax, edx
0x00414EA7  sub eax, ecx
0x00414EA9  mov [esp+44h+var_24], esi
0x00414EAD  lea ebx, [ecx-1]
0x00414EB0  mov [esp+44h+var_28], eax
0x00414EB4  cmp esi, ebp
0x00414EB6  mov [esp+44h+var_34], esi
0x00414EBA  jg loc_414F58
0x00414EC0  lea edi, [eax+ebx]
0x00414EC3  mov esi, ebx
0x00414EC5  add eax, ebx
0x00414EC7  mov [esp+44h+var_1C], eax
0x00414ECB  lea eax, [edx+ebp]
0x00414ECE  mov [esp+44h+var_14], eax
0x00414ED2  lea eax, [ecx+ebp]
0x00414ED5  mov [esp+44h+var_20], esi
0x00414ED9  mov [esp+44h+var_18], esi
0x00414EDD  mov [esp+44h+var_10], ebx
0x00414EE1  mov [esp+44h+var_C], edi
0x00414EE5  mov [esp+44h+var_8], eax
0x00414EE9  mov [esp+44h+var_4], edi
0x00414EED  xor ecx, ecx
0x00414EEF  nop
0x00414EF0  mov edx, [esp+ecx*8+44h+var_20]
0x00414EF4  test edx, edx
0x00414EF6  mov eax, [esp+ecx*8+44h+var_1C]
0x00414EFA  jl loc_414F0A
0x00414EFC  test eax, eax
0x00414EFE  jl loc_414F0A
0x00414F00  cmp edx, 60h
0x00414F03  jge loc_414F0A
0x00414F05  cmp eax, 60h
0x00414F08  jl loc_414F0E
0x00414F0A  mov al, 23h
0x00414F0C  jmp loc_414F21
0x00414F0E  lea eax, [eax+eax*2]
0x00414F11  shl eax, 5
0x00414F14  add eax, edx
0x00414F16  mov edx, [esp+44h+arg_0]
0x00414F1A  mov al, [eax+edx+0B704h]
0x00414F21  not al
0x00414F23  test al, 1
0x00414F25  jnz loc_414F7B
0x00414F27  add ecx, 1
0x00414F2A  cmp ecx, 4
0x00414F2D  jl loc_414EF0
0x00414F2F  mov eax, [esp+44h+var_34]
0x00414F33  mov edx, [esp+44h+var_30]
0x00414F37  mov ecx, [esp+44h+var_2C]
0x00414F3B  add eax, 1
0x00414F3E  add esi, 1
0x00414F41  add edi, 1
0x00414F44  mov [esp+44h+var_34], eax
0x00414F48  cmp eax, ebp
0x00414F4A  mov eax, [esp+44h+var_28]
0x00414F4E  jle loc_414EC5
0x00414F54  mov esi, [esp+44h+var_24]
0x00414F58  add ebp, 1
0x00414F5B  sub esi, 1
0x00414F5E  sub ebx, 1
0x00414F61  cmp ebp, [esp+44h+arg_C]
0x00414F65  mov [esp+44h+var_24], esi
0x00414F69  jle loc_414EB4
0x00414F6F  pop edi
0x00414F70  pop ebp
0x00414F71  pop ebx
0x00414F72  xor al, al
0x00414F74  pop esi
0x00414F75  add esp, 34h
0x00414F78  retn 10h
0x00414F7B  mov eax, [esp+ecx*8+44h+var_20]
0x00414F7F  mov edx, [esp+44h+arg_4]
0x00414F83  pop edi
0x00414F84  mov [edx], eax
0x00414F86  mov eax, [esp+ecx*8+40h+var_1C]
0x00414F8A  mov ecx, [esp+40h+arg_8]
0x00414F8E  pop ebp
0x00414F8F  pop ebx
0x00414F90  mov [ecx], eax
0x00414F92  mov al, 1
0x00414F94  pop esi
0x00414F95  add esp, 34h
0x00414F98  retn 10h
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
