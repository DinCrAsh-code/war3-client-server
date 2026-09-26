# Function summary: sub_414CE0 (0x00414CE0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00414CE0
- Name: sub_414CE0 (auto-generated placeholder)
- Size: 217 bytes, 90 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402050 -> sub_414CE0

## How callers use this function

### From sub_405520 (0x00405520) - call at 0x004058C1

What that caller does overall (preliminary): Performs complex spatial collision, proximity, or intersection checks for an entity against environment elements or other entities, iterating over collections of candidate structures and returning a boolean success status along with target location coordinates.

Called inside nested loops over spatial or index grids (at 0x004058C1) to perform a sub-check or intersection test, passing calculated grid coordinates and a pointer from a local data structure. Its boolean return value (in al) controls whether further evaluation occurs.

### From sub_401750 (0x00401750) - call at 0x00401D31

What that caller does overall (preliminary): Initializes and configures a large resource/state block ('RM' header structure) passed from main, setting up internal sub-systems, tables, processing item loops, and validation checks before invoking subsystem teardown or startup functions.

Called with floating-point parameters, an index (`ecx`), and a resource offset (`ebx+1A20h`) to query or test specific spatial/numeric properties; its boolean result (`al`) gates subsequent item configuration.

### From sub_402050 (0x00402050) - call at 0x0040228B

What that caller does overall (preliminary): Performs geometric, spatial, or resource item computations for an individual resource entry in a loop, querying and interacting with subsystem tables and coordinate/index bounds using floating-point math.

Called inside a loop (clamping index up to 2 iterations) passing a computed index and a pointer to offset 0x1A20 of the resource block to query or validate grid/spatial status.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00414CE0  push ecx
0x00414CE1  push ebx
0x00414CE2  push ebp
0x00414CE3  mov ebp, [esp+0Ch+arg_0]
0x00414CE7  push esi
0x00414CE8  mov esi, edx
0x00414CEA  mov ebx, eax
0x00414CEC  cdq
0x00414CED  sub eax, edx
0x00414CEF  sar eax, 1
0x00414CF1  push edi
0x00414CF2  mov edx, esi
0x00414CF4  mov edi, ecx
0x00414CF6  sub edx, eax
0x00414CF8  sub ecx, eax
0x00414CFA  test esi, esi
0x00414CFC  jl loc_414D0C
0x00414CFE  test edi, edi
0x00414D00  jl loc_414D0C
0x00414D02  cmp esi, 60h
0x00414D05  jge loc_414D0C
0x00414D07  cmp edi, 60h
0x00414D0A  jl loc_414D16
0x00414D0C  mov [esp+14h+arg_0], 0
0x00414D14  jmp loc_414D2A
0x00414D16  lea eax, [edi+edi*2]
0x00414D19  shl eax, 5
0x00414D1C  add eax, esi
0x00414D1E  movsx eax, byte ptr [eax+ebp+9304h]
0x00414D26  mov [esp+14h+arg_0], eax
0x00414D2A  lea eax, [ecx+ebx]
0x00414D2D  cmp ecx, eax
0x00414D2F  mov esi, ecx
0x00414D31  mov [esp+14h+var_4], eax
0x00414D35  jge loc_414DA5
0x00414D37  lea ecx, [ecx+ecx*2]
0x00414D3A  add ebx, edx
0x00414D3C  shl ecx, 5
0x00414D3F  lea edi, [ecx+ebp+9304h]
0x00414D46  cmp edx, ebx
0x00414D48  mov eax, edx
0x00414D4A  jge loc_414D99
0x00414D4C  lea esp, [esp+0]
0x00414D50  test eax, eax
0x00414D52  jl loc_414D62
0x00414D54  test esi, esi
0x00414D56  jl loc_414D62
0x00414D58  cmp eax, 60h
0x00414D5B  jge loc_414D62
0x00414D5D  cmp esi, 60h
0x00414D60  jl loc_414D66
0x00414D62  mov cl, 23h
0x00414D64  jmp loc_414D6D
0x00414D66  mov cl, [edi+eax+2400h]
0x00414D6D  test cl, 5Bh
0x00414D70  jnz loc_414DAF
0x00414D72  test eax, eax
0x00414D74  jl loc_414D84
0x00414D76  test esi, esi
0x00414D78  jl loc_414D84
0x00414D7A  cmp eax, 60h
0x00414D7D  jge loc_414D84
0x00414D7F  cmp esi, 60h
0x00414D82  jl loc_414D88
0x00414D84  xor ecx, ecx
0x00414D86  jmp loc_414D8C
0x00414D88  movsx ecx, byte ptr [edi+eax]
0x00414D8C  cmp ecx, [esp+14h+arg_0]
0x00414D90  jnz loc_414DAF
0x00414D92  add eax, 1
0x00414D95  cmp eax, ebx
0x00414D97  jl loc_414D50
0x00414D99  add esi, 1
0x00414D9C  add edi, 60h
0x00414D9F  cmp esi, [esp+14h+var_4]
0x00414DA3  jl loc_414D46
0x00414DA5  pop edi
0x00414DA6  pop esi
0x00414DA7  pop ebp
0x00414DA8  mov al, 1
0x00414DAA  pop ebx
0x00414DAB  pop ecx
0x00414DAC  retn 4
0x00414DAF  pop edi
0x00414DB0  pop esi
0x00414DB1  pop ebp
0x00414DB2  xor al, al
0x00414DB4  pop ebx
0x00414DB5  pop ecx
0x00414DB6  retn 4
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
