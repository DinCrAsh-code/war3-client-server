# Function summary: sub_403BF0 (0x00403BF0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00403BF0
- Name: sub_403BF0 (auto-generated placeholder)
- Size: 97 bytes, 34 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_40A0D0 -> sub_409940 -> sub_40ABE0 -> sub_403BF0

## How callers use this function

### From sub_40ABE0 (0x0040ABE0) - call at 0x0040ABFE

What that caller does overall (preliminary): A virtual opcode handler (slot 18 in vtable 0x0041D228) invoked from the bytecode interpreter to process a typed value/operand (checking if it is type 2, converting floating-point via __ftol2_sse or extracting payload, bounding to <= 5, and indexing/dispatching to a helper sub_403BF0).

Called with the processed integer value (clamped/bounded to be <= 5) in EBX to perform the core operation or lookup associated with this opcode handler.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00403BF0  mov eax, ebx
0x00403BF2  imul eax
0x00403BF8  push esi
0x00403BF9  mov esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00403BFF  mov cl, [eax+esi+3]
0x00403C03  add eax, esi
0x00403C05  test cl, 4
0x00403C08  jnz loc_403C4F
0x00403C0A  or cl, 4
0x00403C0D  push edi
0x00403C0E  xor edi, edi
0x00403C10  mov [eax+3], cl
0x00403C13  cmp [esi+57390h], edi
0x00403C19  jle loc_403C4E
0x00403C1B  jmp loc_403C20
0x00403C20  mov eax, [esi+5738Ch]
0x00403C26  mov ecx, [eax+edi*4]
0x00403C29  movzx edx, byte ptr [ecx+1Bh]
0x00403C2D  cmp edx, ebx
0x00403C2F  jnz loc_403C43
0x00403C31  mov al, [ecx+18h]
0x00403C34  not al
0x00403C36  test al, 1
0x00403C38  jz loc_403C43
0x00403C3A  mov edx, [ecx]
0x00403C3C  mov eax, [edx+0Ch]
0x00403C3F  push 0
0x00403C41  call eax
0x00403C43  add edi, 1
0x00403C46  cmp edi, [esi+57390h]
0x00403C4C  jl loc_403C20
0x00403C4E  pop edi
0x00403C4F  pop esi
0x00403C50  retn
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
