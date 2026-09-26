# Function summary: sub_4027A0 (0x004027A0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004027A0
- Name: sub_4027A0 (auto-generated placeholder)
- Size: 171 bytes, 57 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_40A0D0 -> sub_409940 -> sub_40AC70 -> sub_4027A0

## How callers use this function

### From sub_40AC70 (0x0040AC70) - call at 0x0040ACCD

What that caller does overall (preliminary): Implements a virtual opcode handler (vtable slot 20) for the bytecode interpreter, processing script data structures by reading operand values, performing type checks or conversions, and invoking helper routines to compute a result stored into an output status structure.

Called with parameters prepared from processed script operand values (including floating-point/integer conversions loaded onto the x87 stack and an integer code pushed on the stack) to perform a core operation, returning a pointer whose field is used to update the output status structure.

## Functions it calls or references

### sub_4185D0 [AllocateCustomBlock] (0x004185D0) - call at 0x004027AD

- Prototype: `void* __fastcall AllocateCustomBlock(int size)`
- Summary: Allocates a custom memory block of the requested size, utilizing a fixed-size memory pool allocator for small/medium allocations or falling back to malloc for large ones, and returns an inner payload pointer with a tracked header.

### sub_410BD0 [ScriptVariable_Initialize] (0x00410BD0) - call at 0x004027BA

- Prototype: `void* __fastcall ScriptVariable_Initialize(void *varObj, int unused, char typeCode)`
- Summary: Initializes a script variable or runtime property object with default values, a vtable pointer, and a type-specific property computed from a passed-in identifier or type flag.

### sub_404100 [GrowContainerCapacity] (0x00404100) - call at 0x00402833

- Prototype: `void __stdcall GrowContainerCapacity(Container *container)`
- Summary: Grows the capacity and reallocates the underlying data buffer of a container structure when it reaches capacity.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004027A0  push esi
0x004027A1  mov esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x004027A7  push edi
0x004027A8  mov eax, 24h
0x004027AD  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004027B2  test eax, eax
0x004027B4  jz loc_4027C3
0x004027B6  mov ecx, [esp+8+arg_0]
0x004027BA  call sub_410BD0                              ; -> sub_410BD0 [ScriptVariable_Initialize]
0x004027BF  mov edi, eax
0x004027C1  jmp loc_4027C5
0x004027C3  xor edi, edi
0x004027C5  mov ecx, [esi+51E88h]
0x004027CB  cmp ecx, [esi+51E84h]
0x004027D1  jge loc_402801
0x004027D3  mov eax, [esi+51E80h]
0x004027D9  movzx edx, word ptr [eax+ecx*8+6]
0x004027DE  lea eax, [eax+ecx*8]
0x004027E1  mov [esi+51E88h], edx
0x004027E7  mov [eax], edi
0x004027E9  add dword ptr [esi+51E8Ch], 1
0x004027F0  movzx eax, word ptr [eax+4]
0x004027F4  shl eax, 10h
0x004027F7  add ecx, 1
0x004027FA  or eax, ecx
0x004027FC  mov [edi+4], eax
0x004027FF  jnz loc_402816
0x00402801  test edi, edi
0x00402803  jz loc_40280F
0x00402805  mov eax, [edi]
0x00402807  mov edx, [eax]
0x00402809  push 1
0x0040280B  mov ecx, edi
0x0040280D  call edx
0x0040280F  pop edi
0x00402810  xor eax, eax
0x00402812  pop esi
0x00402813  retn 4
0x00402816  mov eax, [ebx]
0x00402818  mov [edi+8], eax
0x0040281B  mov ecx, [ebx+4]
0x0040281E  add esi, 573A4h
0x00402824  mov [edi+0Ch], ecx
0x00402827  mov eax, [esi+4]
0x0040282A  cmp eax, [esi+8]
0x0040282D  jnz loc_402838
0x0040282F  lea ecx, [eax+1]
0x00402832  push esi
0x00402833  call sub_404100                              ; -> sub_404100 [GrowContainerCapacity]
0x00402838  mov edx, [esi+4]
0x0040283B  mov eax, [esi]
0x0040283D  mov [eax+edx*4], edi
0x00402840  add dword ptr [esi+4], 1
0x00402844  mov eax, edi
0x00402846  pop edi
0x00402847  pop esi
0x00402848  retn 4
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
