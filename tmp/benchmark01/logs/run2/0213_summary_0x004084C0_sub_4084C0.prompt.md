# Function summary: sub_4084C0 (0x004084C0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004084C0
- Name: sub_4084C0 (auto-generated placeholder)
- Size: 69 bytes, 24 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402980 -> sub_4084C0

## How callers use this function

### From sub_402980 (0x00402980) - call at 0x0040299F

What that caller does overall (preliminary): Serves as an event handler or callback routine registered in a resource structure's callback table, performing an optional check on an object pointer before conditionally invoking a downstream handling function.

Invoked as a conditional handler or processing action inside the callback function. It is called only when the object pointer extracted from the resource structure (at offset +4) is non-null and has a specific type/status byte at offset +1Bh equal to 4. It receives an adjusted value from the second argument (arg_4 plus an offset of 0x571D4) in ecx, while its other arguments are implicitly set up via registers like edi (derived from offset +8 of the first argument).

## Functions it calls or references

### sub_4029F0 [LookupResourceOrHandle] (0x004029F0) - call at 0x004084E7

- Prototype: `void* __fastcall LookupResourceOrHandle(unsigned int handle)`
- Summary: Resolves a 32-bit handle (comprising an index and a validation tag) into a managed object pointer, verifying bounds, a tag match, and an internal status byte.

### sub_4080A0 [ProcessEntityInteractionContext] (0x004080A0) - tail jump at 0x00408500

- Prototype: `void __fastcall ProcessEntityInteractionContext(void *record, void *context)`
- Summary: Iterates through a list of entity references in a context structure, validates each entity against compatibility and state rules, and triggers an entity interaction or command using helper functions.

Reference kinds used above:
- *tail jump*: a `jmp` straight to the function's start, i.e. a tail call.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004084C0  mov eax, [eax+238h]
0x004084C6  test eax, eax
0x004084C8  push esi
0x004084C9  jl loc_4084F9
0x004084CB  cmp eax, [ecx+1B0h]
0x004084D1  jge loc_4084F9
0x004084D3  test edi, edi
0x004084D5  jz loc_4084F9
0x004084D7  lea edx, [eax+eax*8]
0x004084DA  cmp byte ptr [ecx+edx*4+0Bh], 0
0x004084DF  lea esi, [ecx+edx*4]
0x004084E2  jz loc_4084FB
0x004084E4  mov ecx, [esi+20h]
0x004084E7  call sub_4029F0                              ; -> sub_4029F0 [LookupResourceOrHandle]
0x004084EC  test eax, eax
0x004084EE  jz loc_4084FB
0x004084F0  fld                                          ; -> 0x0041D2BC in .rdata
0x004084F6  fstp
0x004084F9  pop esi
0x004084FA  retn
0x004084FB  mov eax, esi
0x004084FD  mov ecx, edi
0x004084FF  pop esi
0x00408500  jmp sub_4080A0                               ; -> sub_4080A0 [ProcessEntityInteractionContext]
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
