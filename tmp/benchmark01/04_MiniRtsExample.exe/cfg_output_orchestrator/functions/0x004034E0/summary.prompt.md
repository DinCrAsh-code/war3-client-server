# Function summary: sub_4034E0 (0x004034E0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004034E0
- Name: sub_4034E0 (auto-generated placeholder)
- Size: 55 bytes, 18 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_4034E0

## How callers use this function

### From sub_403D30 (0x00403D30) - call at 0x00403D95

What that caller does overall (preliminary): Executes a single step or tick of a simulation/update phase using the resource structure provided in ebp (passed from sub_403E50), periodically updating sub-components, checking step-based intervals (dividing counters by 5 and 10), and handling state or resource blocks.

Called unconditionally in sequence after sub_403460, receiving the resource pointer in edi (ebp).

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004034E0  push esi
0x004034E1  xor esi, esi
0x004034E3  cmp [edi+57390h], esi
0x004034E9  jle loc_403515
0x004034EB  jmp loc_4034F0
0x004034F0  mov eax, [edi+5738Ch]
0x004034F6  fld                                          ; -> 0x0041D2EC in .rdata
0x004034FC  mov ecx, [eax+esi*4]
0x004034FF  mov edx, [ecx]
0x00403501  mov eax, [edx+4]
0x00403504  push ecx
0x00403505  fstp
0x00403508  call eax
0x0040350A  add esi, 1
0x0040350D  cmp esi, [edi+57390h]
0x00403513  jl loc_4034F0
0x00403515  pop esi
0x00403516  retn
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
