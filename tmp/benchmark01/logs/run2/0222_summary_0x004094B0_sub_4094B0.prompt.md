# Function summary: sub_4094B0 (0x004094B0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004094B0
- Name: sub_4094B0 (auto-generated placeholder)
- Size: 431 bytes, 148 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_40A0D0 -> sub_409940 -> sub_4094B0

## How callers use this function

### From sub_409940 (0x00409940) - call at 0x00409B9B

What that caller does overall (preliminary): Interprets and executes bytecode/script stream instructions from a resource structure (ESI), dispatching based on opcode bytes using a large switch/dispatch table and invoking specialized helper routines or recursive sub-executions.

Called to perform a binary or arithmetic operation on two fetched variable/operand buffers (passed via ECX and EAX) with an operation code in EDX, returning a combined result.

## Functions it calls or references

### _CIfmod (0x0041A21E) - call at 0x00409631

- Summary: Import thunk: jumps straight to the imported API MSVCR80!_CIfmod.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004094B0  sub esp, 18h
0x004094B3  push ebx
0x004094B4  mov bl, 2
0x004094B6  cmp [eax], bl
0x004094B8  jz loc_40954D
0x004094BE  cmp [ecx], bl
0x004094C0  jz loc_409552
0x004094C6  mov eax, [eax+4]
0x004094C9  mov ecx, [ecx+4]
0x004094CC  add edx, 0FFFFFFE0h
0x004094CF  cmp edx, 3
0x004094D2  ja def_4094D4
0x004094D4  jmp ds:jpt_4094D4[edx*4]                     ; -> 0x00409660 in .text
0x004094DB  mov byte ptr [esp+1Ch+var_C], 1
0x004094E0  lea edx, [ecx+eax]
0x004094E3  mov eax, [esp+1Ch+var_C]
0x004094E7  pop ebx
0x004094E8  add esp, 18h
0x004094EB  retn
0x004094EC  sub eax, ecx
0x004094EE  mov byte ptr [esp+1Ch+var_C], 1
0x004094F3  mov edx, eax
0x004094F5  mov eax, [esp+1Ch+var_C]
0x004094F9  pop ebx
0x004094FA  add esp, 18h
0x004094FD  retn
0x004094FE  imul ecx, eax
0x00409501  mov byte ptr [esp+1Ch+var_C], 1
0x00409506  mov eax, [esp+1Ch+var_C]
0x0040950A  mov edx, ecx
0x0040950C  pop ebx
0x0040950D  add esp, 18h
0x00409510  retn
0x00409511  test ecx, ecx
0x00409513  jz loc_409528
0x00409515  cdq
0x00409516  idiv
0x00409518  mov byte ptr [esp+1Ch+var_C], 1
0x0040951D  mov edx, eax
0x0040951F  mov eax, [esp+1Ch+var_C]
0x00409523  pop ebx
0x00409524  add esp, 18h
0x00409527  retn
0x00409528  mov byte ptr [esp+1Ch+var_C], 1
0x0040952D  mov eax, [esp+1Ch+var_C]
0x00409531  xor edx, edx
0x00409533  pop ebx
0x00409534  add esp, 18h
0x00409537  retn
0x00409538  test ecx, ecx
0x0040953A  jz loc_409528
0x0040953C  cdq
0x0040953D  idiv
0x0040953F  mov byte ptr [esp+1Ch+var_C], 1
0x00409544  mov eax, [esp+1Ch+var_C]
0x00409548  pop ebx
0x00409549  add esp, 18h
0x0040954C  retn
0x0040954D  fld
0x00409550  jmp loc_409555
0x00409552  fild
0x00409555  cmp [ecx], bl
0x00409557  fstp
0x0040955B  jnz loc_409562
0x0040955D  fld
0x00409560  jmp loc_409565
0x00409562  fild
0x00409565  lea eax, [edx-20h]
0x00409568  fstp
0x0040956C  cmp eax, 3
0x0040956F  ja def_409575
0x00409575  jmp ds:jpt_409575[eax*4]                     ; -> 0x00409670 in .text
0x0040957C  fld
0x00409580  mov byte ptr [esp+1Ch+var_C], bl
0x00409584  fadd
0x00409588  mov eax, [esp+1Ch+var_C]
0x0040958C  fstp
0x00409590  mov edx, [esp+1Ch+var_8]
0x00409594  pop ebx
0x00409595  add esp, 18h
0x00409598  retn
0x00409599  fld
0x0040959D  mov byte ptr [esp+1Ch+var_C], bl
0x004095A1  fsub
0x004095A5  mov eax, [esp+1Ch+var_C]
0x004095A9  fstp
0x004095AD  mov edx, [esp+1Ch+var_8]
0x004095B1  pop ebx
0x004095B2  add esp, 18h
0x004095B5  retn
0x004095B6  fld
0x004095BA  mov byte ptr [esp+1Ch+var_C], bl
0x004095BE  fmul
0x004095C2  mov eax, [esp+1Ch+var_C]
0x004095C6  fstp
0x004095CA  mov edx, [esp+1Ch+var_8]
0x004095CE  pop ebx
0x004095CF  add esp, 18h
0x004095D2  retn
0x004095D3  fldz
0x004095D5  fld
0x004095D9  fucom
0x004095DB  fnstsw ax
0x004095DD  fstp
0x004095DF  test ah, 44h
0x004095E2  jnp loc_4095FD
0x004095E4  fdivr
0x004095E8  mov byte ptr [esp+1Ch+var_C], bl
0x004095EC  mov eax, [esp+1Ch+var_C]
0x004095F0  fstp
0x004095F4  mov edx, [esp+1Ch+var_8]
0x004095F8  pop ebx
0x004095F9  add esp, 18h
0x004095FC  retn
0x004095FD  fstp
0x004095FF  mov byte ptr [esp+1Ch+var_C], bl
0x00409603  fldz
0x00409605  mov eax, [esp+1Ch+var_C]
0x00409609  fstp
0x0040960D  mov edx, [esp+1Ch+var_8]
0x00409611  pop ebx
0x00409612  add esp, 18h
0x00409615  retn
0x00409616  fldz
0x00409618  fld
0x0040961A  fld
0x0040961E  fucom
0x00409620  fnstsw ax
0x00409622  fstp
0x00409624  test ah, 44h
0x00409627  jnp loc_409640
0x00409629  fstp
0x0040962B  fld
0x0040962F  fxch
0x00409631  call _CIfmod                                 ; -> _CIfmod
0x00409636  fstp
0x0040963A  fld
0x0040963E  jmp loc_409642
0x00409640  fstp
0x00409642  fstp
0x00409646  mov byte ptr [esp+1Ch+var_C], bl
0x0040964A  fld
0x0040964E  mov eax, [esp+1Ch+var_C]
0x00409652  fstp
0x00409656  mov edx, [esp+1Ch+var_8]
0x0040965A  pop ebx
0x0040965B  add esp, 18h
0x0040965E  retn
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
