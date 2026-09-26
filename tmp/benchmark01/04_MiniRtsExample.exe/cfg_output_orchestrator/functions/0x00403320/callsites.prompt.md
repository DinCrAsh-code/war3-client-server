# Callsite analysis: sub_403320 (0x00403320)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed in two passes. This is the top-down pass: before any of this function's callees are analyzed, describe - from this function's side - what it uses each of them for. Each description is handed to whoever analyzes that callee as its *callsite summary*, so the callee can be understood in context (e.g. "called once per list node to print the node's value" rather than just "prints an integer"). Carry the relevant bigger picture down with it: what this function is doing overall and why, as far as its own callers (below) suggest.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00403320
- Name: sub_403320 (auto-generated placeholder)
- Size: 120 bytes, 45 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_4079D0 -> sub_4071C0 -> sub_40C950 -> sub_403320

## Context from this function's callers

### From sub_40C950 (0x0040C950) - call at 0x0040CA7E

What that caller does overall (preliminary): Acts as a sophisticated validation and filtering predicate for game entities/components, checking entity status flags, type attributes, bitmasks, and spatial or numerical criteria against a query criteria structure before deciding if the entity matches.

Called conditionally when bit 8 (`0x100`) of the criteria flags (`ebx`) is set, passing the entity pointer in `edx` (`edi`) and a scalar/parameter in `eax` (`ebp+4`), using the boolean return value in `al` to determine whether to accept or reject the entity.

## Callees to describe

- `0x00417D70` sub_417D70 - call at 0x0040338B

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00403320  test byte ptr [edx+18h], 40h
0x00403324  push ebx
0x00403325  mov ebx, eax
0x00403327  mov eax, dword_42AC60                        ; -> 0x0042AC60 in .data
0x0040332C  jz loc_403354
0x0040332E  movzx ecx, byte ptr [edx+1Bh]
0x00403332  cmp ecx, 5
0x00403335  ja loc_403350
0x00403337  imul ebx
0x0040333D  mov edx, 1
0x00403342  shl edx, cl
0x00403344  test [ebx+eax+0E8h], edx
0x0040334B  pop ebx
0x0040334C  setnz al
0x0040334F  retn
0x00403350  xor al, al
0x00403352  pop ebx
0x00403353  retn
0x00403354  cmp ebx, 4
0x00403357  push edi
0x00403358  jge loc_403393
0x0040335A  movzx ecx, byte ptr [edx+1Bh]
0x0040335E  cmp ecx, 5
0x00403361  ja loc_403382
0x00403363  push esi
0x00403364  mov esi, ebx
0x00403366  imul esi
0x0040336C  mov edi, 1
0x00403371  shl edi, cl
0x00403373  test [esi+eax+0E8h], edi
0x0040337A  pop esi
0x0040337B  setnz cl
0x0040337E  test cl, cl
0x00403380  jnz loc_403393
0x00403382  add eax, 4467Ch
0x00403387  lea edi, [edx+8]
0x0040338A  push eax
0x0040338B  call sub_417D70                              ; -> sub_417D70
0x00403390  pop edi
0x00403391  pop ebx
0x00403392  retn
0x00403393  pop edi
0x00403394  mov al, 1
0x00403396  pop ebx
0x00403397  retn
```

## Output

Reply with exactly one JSON object - no prose before or after it, no markdown code fences - of this shape:

{
  "function_role": "1-2 sentences: what this function appears to do overall (preliminary - its callees are not analyzed yet)",
  "callsites": [
    {"callee": "<address exactly as listed above, e.g. 0x00401596>", "callsite_summary": "..."}
  ]
}

Include one `callsites` entry for every callee listed under "Callees to describe". Each `callsite_summary` should cover, as far as this function's code shows: why it uses the callee at that point (the role it plays in this function's logic); the arguments it passes (registers / stack slots, constants, and what they represent); how the return value is used, if at all; the conditions or loops around the call(s); and for an address used as data or a vtable entry, what the address is stored into or passed to and who will likely call it. Don't speculate about the callee's internals beyond what its use here implies.
