# Function summary: sub_413870 (0x00413870)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00413870
- Name: sub_413870 (auto-generated placeholder)
- Size: 107 bytes, 33 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402050 -> sub_402560 -> sub_410720 -> sub_40B100 -> sub_411580 -> sub_413870

## How callers use this function

### From sub_411580 (0x00411580) - call at 0x004115C6

What that caller does overall (preliminary): Acts as a factory dispatcher, taking a type/index identifier in `esi`, validating it against a range and lookup table (`ds:byte_41BDC0`), allocating a memory block of a specific size via `sub_4185D0`, and then invoking a corresponding constructor/initialization function for a specific sub-component type.

Called as the constructor/initializer for the first factory case (type index mapped via switch), receiving the newly allocated memory block in `edx`, `esi` (or `edi` via push), and setting up the object.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00413870  mov [eax+8], edx
0x00413873  fldz
0x00413875  xor edx, edx
0x00413877  xor ecx, ecx
0x00413879  cmp [esp+arg_0], 18h
0x0041387E  push esi
0x0041387F  setnl dl
0x00413882  mov dword ptr [eax], offset off_41D464       ; -> 0x0041D464 in .rdata
0x00413888  mov [eax+4], ecx
0x0041388B  sub edx, 1
0x0041388E  and edx, [esp+4+arg_0]
0x00413892  imul edx
0x00413895  lea esi, byte_41BDC0[edx]                    ; -> 0x0041BDC0 in .rdata
0x0041389B  mov dl, byte ptr [esp+4+arg_0]
0x0041389F  mov [eax+10h], dl
0x004138A2  mov dl, [esp+4+arg_4]
0x004138A6  mov [eax+11h], dl
0x004138A9  mov [eax+0Ch], esi
0x004138AC  movzx edx, byte ptr [esi+1]
0x004138B0  fst
0x004138B3  shr dl, 2
0x004138B6  fstp
0x004138B9  and dl, 1
0x004138BC  mov [eax+12h], dl
0x004138BF  mov [eax+13h], cl
0x004138C2  mov dword ptr [eax], offset off_41D770       ; -> 0x0041D770 in .rdata
0x004138C8  mov [eax+1Ch], cl
0x004138CB  mov [eax+1Dh], cl
0x004138CE  mov [eax+1Eh], cl
0x004138D1  mov [eax+20h], ecx
0x004138D4  mov [eax+24h], ecx
0x004138D7  pop esi
0x004138D8  retn 8
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
