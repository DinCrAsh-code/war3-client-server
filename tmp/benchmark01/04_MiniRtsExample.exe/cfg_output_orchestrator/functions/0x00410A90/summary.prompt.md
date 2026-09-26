# Function summary: sub_410A90 (0x00410A90)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00410A90
- Name: sub_410A90 (auto-generated placeholder)
- Size: 142 bytes, 44 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_410A90

## How callers use this function

### From sub_401750 (0x00401750) - call at 0x00401A86

What that caller does overall (preliminary): Initializes and configures a large resource/state block ('RM' header structure) passed from main, setting up internal sub-systems, tables, processing item loops, and validation checks before invoking subsystem teardown or startup functions.

Called with a lookup/identifier value (`Src`) to create or fetch a resource entry, returning a pointer in `eax` that is subsequently inserted into a tracking table.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00410A90  sub esp, 0Ch
0x00410A93  fld                                          ; -> 0x0041DA44 in .rdata
0x00410A99  push esi
0x00410A9A  mov esi, eax
0x00410A9C  fst
0x00410A9F  xor eax, eax
0x00410AA1  fstp
0x00410AA4  mov [ecx+4], eax
0x00410AA7  mov [ecx+18h], ax
0x00410AAB  mov [ecx+20h], al
0x00410AAE  mov eax, 2AAAAAABh
0x00410AB3  imul
0x00410AB5  sar edx, 4
0x00410AB8  mov eax, edx
0x00410ABA  shr eax, 1Fh
0x00410ABD  add eax, edx
0x00410ABF  mov [esp+10h+var_4], eax
0x00410AC3  lea eax, [eax+eax*2]
0x00410AC6  shl eax, 5
0x00410AC9  mov [ecx+1Ch], si
0x00410ACD  sub esi, eax
0x00410ACF  mov [esp+10h+var_C], esi
0x00410AD3  mov byte ptr [ecx+1Ah], 3
0x00410AD7  fild
0x00410ADB  mov byte ptr [ecx+1Bh], 5
0x00410ADF  fld                                          ; -> 0x0041D8C8 in .rdata
0x00410AE5  mov dword ptr [ecx], offset off_41D418       ; -> 0x0041D418 in .rdata
0x00410AEB  mov word ptr [ecx+1Eh], 32h
0x00410AF1  fadd st(1), st
0x00410AF3  pop esi
0x00410AF4  fld                                          ; -> 0x0041D8C0 in .rdata
0x00410AFA  fmul st(2), st
0x00410AFC  fxch
0x00410AFE  fstp
0x00410B01  mov edx, [esp+0Ch+var_C]
0x00410B04  mov [ecx+8], edx
0x00410B07  fiadd
0x00410B0B  fmulp st(1), st
0x00410B0D  fstp
0x00410B11  mov eax, [esp+0Ch+var_8]
0x00410B15  mov [ecx+0Ch], eax
0x00410B18  mov eax, ecx
0x00410B1A  add esp, 0Ch
0x00410B1D  retn
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
