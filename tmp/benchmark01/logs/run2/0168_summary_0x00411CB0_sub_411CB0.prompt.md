# Function summary: sub_411CB0 (0x00411CB0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00411CB0
- Name: sub_411CB0 (auto-generated placeholder)
- Size: 84 bytes, 24 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402050 -> sub_402560 -> sub_410720 -> sub_40B100 -> sub_411580 -> sub_411CB0

## How callers use this function

### From sub_411580 (0x00411580) - call at 0x0041163A

What that caller does overall (preliminary): Acts as a factory dispatcher, taking a type/index identifier in `esi`, validating it against a range and lookup table (`ds:byte_41BDC0`), allocating a memory block of a specific size via `sub_4185D0`, and then invoking a corresponding constructor/initialization function for a specific sub-component type.

Called as the constructor/initializer for factory case 5, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00411CB0  mov [eax+8], edx
0x00411CB3  fldz
0x00411CB5  xor edx, edx
0x00411CB7  cmp ecx, 18h
0x00411CBA  setnl dl
0x00411CBD  mov [eax+10h], cl
0x00411CC0  mov dword ptr [eax], offset off_41D464       ; -> 0x0041D464 in .rdata
0x00411CC6  mov dword ptr [eax+4], 0
0x00411CCD  sub edx, 1
0x00411CD0  and edx, ecx
0x00411CD2  mov cl, [esp+arg_0]
0x00411CD6  imul edx
0x00411CD9  lea edx, byte_41BDC0[edx]                    ; -> 0x0041BDC0 in .rdata
0x00411CDF  mov [eax+0Ch], edx
0x00411CE2  mov [eax+11h], cl
0x00411CE5  mov dl, [edx+1]
0x00411CE8  shr dl, 2
0x00411CEB  and dl, 1
0x00411CEE  fst
0x00411CF1  fstp
0x00411CF4  mov [eax+12h], dl
0x00411CF7  mov byte ptr [eax+13h], 0
0x00411CFB  mov dword ptr [eax], offset off_41D500       ; -> 0x0041D500 in .rdata
0x00411D01  retn 4
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
