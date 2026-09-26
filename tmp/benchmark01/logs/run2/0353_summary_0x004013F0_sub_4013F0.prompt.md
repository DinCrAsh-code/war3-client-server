# Function summary: sub_4013F0 (0x004013F0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004013F0
- Name: sub_4013F0 (auto-generated placeholder)
- Size: 147 bytes, 45 instructions
- One call chain from the analysis root down to it: _main -> sub_401490 -> sub_4013F0

## How callers use this function

### From sub_401490 (0x00401490) - call at 0x004015ED

What that caller does overall (preliminary): Performs cleanup, resource teardown, and deallocation for a large resource block (passed as argument) by validating custom 'RM' header blocks, updating global tracking data, freeing memory via standard free or custom pool lists, and invoking component-specific destructors/cleanup helpers.

Called with the `esi` register initialized to `edi + 0x54740` to perform teardown or cleanup operations on a sub-component within the resource structure.

## Imported APIs referenced

- MSVCR80!free

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004013F0  mov eax, [esi+2010h]
0x004013F6  test eax, eax
0x004013F8  push edi
0x004013F9  mov edi, ds:free                             ; -> import MSVCR80!free
0x004013FF  jz loc_40143C
0x00401401  add eax, 0FFFFFFF8h
0x00401404  cmp word ptr [eax], 4D52h
0x00401409  jnz loc_40143C
0x0040140B  mov ecx, [eax+4]
0x0040140E  sub dword_42281C, ecx                        ; -> 0x0042281C in .data
0x00401414  mov cl, [eax+2]
0x00401417  cmp cl, 0FFh
0x0040141A  mov word ptr [eax], 0
0x0040141F  jnz loc_401429
0x00401421  push eax
0x00401422  call edi                                     ; -> import MSVCR80!free
0x00401424  add esp, 4
0x00401427  jmp loc_40143C
0x00401429  movzx ecx, cl
0x0040142C  mov edx, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x00401433  mov [eax], edx
0x00401435  mov dword_422820[ecx*4], eax                 ; -> 0x00422820 in .data
0x0040143C  mov eax, [esi+2004h]
0x00401442  test eax, eax
0x00401444  jz loc_401481
0x00401446  add eax, 0FFFFFFF8h
0x00401449  cmp word ptr [eax], 4D52h
0x0040144E  jnz loc_401481
0x00401450  mov ecx, [eax+4]
0x00401453  sub dword_42281C, ecx                        ; -> 0x0042281C in .data
0x00401459  mov cl, [eax+2]
0x0040145C  cmp cl, 0FFh
0x0040145F  mov word ptr [eax], 0
0x00401464  jnz loc_40146E
0x00401466  push eax
0x00401467  call edi                                     ; -> import MSVCR80!free
0x00401469  add esp, 4
0x0040146C  pop edi
0x0040146D  retn
0x0040146E  movzx ecx, cl
0x00401471  mov edx, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x00401478  mov [eax], edx
0x0040147A  mov dword_422820[ecx*4], eax                 ; -> 0x00422820 in .data
0x00401481  pop edi
0x00401482  retn
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
