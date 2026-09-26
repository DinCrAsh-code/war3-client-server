# Function summary: sub_4062E0 (0x004062E0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004062E0
- Name: sub_4062E0 (auto-generated placeholder)
- Size: 597 bytes, 190 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_406760 -> sub_4062E0

## How callers use this function

### From sub_406C90 (0x00406C90) - call at 0x00406DCE

What that caller does overall (preliminary): Evaluates and processes an entity's internal collection of sub-components or spatial points, performing filtering, distance or threshold checks, and invoking batch update/routing routines (sub_404760, sub_4062E0) based on state flags and criteria.

Called at 0x00406DCE with the entity pointer (`ebp`), a local output buffer (`var_68`), a count (18h), and reference structures (including `sub_40CAA0` and `var_D8`), likely to perform a query, sort, or spatial lookup on the collected items.

### From sub_406760 (0x00406760) - call at 0x0040678C

What that caller does overall (preliminary): Performs state machine updates, distance/target calculations, and behavior checks for an individual simulation entity (passed in ebx) based on its current state, flags, and spatial proximity.

Called at 0x0040678C, passing the entity pointer in ebx and a stack variable pointer in ecx, likely to query or initialize entity-specific property/status data.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004062E0  sub esp, 0Ch
0x004062E3  mov eax, [esp+0Ch+arg_0]
0x004062E7  fldz
0x004062E9  push ebx
0x004062EA  fst
0x004062EE  push ebp
0x004062EF  fstp
0x004062F3  push esi
0x004062F4  push edi
0x004062F5  mov edi, [eax+3C8h]
0x004062FB  xor ebp, ebp
0x004062FD  xor ebx, ebx
0x004062FF  cmp edi, 4
0x00406302  mov [esp+1Ch+var_C], ebp
0x00406306  jl loc_4063A5
0x0040630C  lea esi, [edi-4]
0x0040630F  shr esi, 2
0x00406312  add esi, 1
0x00406315  lea edx, ds:0[esi*4]
0x0040631C  mov ebp, edx
0x0040631E  lea ecx, [eax+24Ch]
0x00406324  mov [esp+1Ch+var_C], ebp
0x00406328  mov ebx, edx
0x0040632A  lea ebx, [ebx+0]
0x00406330  mov edx, [ecx-4]
0x00406333  fld
0x00406336  add edx, 8
0x00406339  fadd
0x0040633D  add ecx, 10h
0x00406340  fstp
0x00406344  fld
0x00406347  mov edx, [ecx-10h]
0x0040634A  fadd
0x0040634E  add edx, 8
0x00406351  fstp
0x00406355  fld
0x00406357  fadd
0x0040635B  fstp
0x0040635F  fld
0x00406362  mov edx, [ecx-0Ch]
0x00406365  fadd
0x00406369  add edx, 8
0x0040636C  fstp
0x00406370  fld
0x00406372  fadd
0x00406376  fstp
0x0040637A  fld
0x0040637D  mov edx, [ecx-8]
0x00406380  fadd
0x00406384  add edx, 8
0x00406387  sub esi, 1
0x0040638A  fstp
0x0040638E  fld
0x00406390  fadd
0x00406394  fstp
0x00406398  fld
0x0040639B  fadd
0x0040639F  fstp
0x004063A3  jnz loc_406330
0x004063A5  cmp ebx, edi
0x004063A7  jge loc_4063DB
0x004063A9  sub edi, ebx
0x004063AB  add ebp, edi
0x004063AD  lea edx, [eax+ebx*4+248h]
0x004063B4  mov [esp+1Ch+var_C], ebp
0x004063B8  mov ecx, [edx]
0x004063BA  fld
0x004063BD  add ecx, 8
0x004063C0  fadd
0x004063C4  add edx, 4
0x004063C7  sub edi, 1
0x004063CA  fstp
0x004063CE  fld
0x004063D1  fadd
0x004063D5  fstp
0x004063D9  jnz loc_4063B8
0x004063DB  mov edi, [eax+3DCh]
0x004063E1  xor ebx, ebx
0x004063E3  cmp edi, 4
0x004063E6  jl loc_4064A9
0x004063EC  lea esi, [edi-4]
0x004063EF  shr esi, 2
0x004063F2  add esi, 1
0x004063F5  lea ecx, [eax+3D4h]
0x004063FB  lea ebx, ds:0[esi*4]
0x00406402  mov edx, [ecx-4]
0x00406405  mov al, [edx+18h]
0x00406408  not al
0x0040640A  test al, 1
0x0040640C  jz loc_406427
0x0040640E  fld
0x00406412  add ebp, 1
0x00406415  fadd
0x00406418  fstp
0x0040641C  fld
0x0040641F  fadd
0x00406423  fstp
0x00406427  mov edx, [ecx]
0x00406429  mov al, [edx+18h]
0x0040642C  not al
0x0040642E  test al, 1
0x00406430  jz loc_40644B
0x00406432  fld
0x00406436  add ebp, 1
0x00406439  fadd
0x0040643C  fstp
0x00406440  fld
0x00406443  fadd
0x00406447  fstp
0x0040644B  mov edx, [ecx+4]
0x0040644E  mov al, [edx+18h]
0x00406451  not al
0x00406453  test al, 1
0x00406455  jz loc_406470
0x00406457  fld
0x0040645B  add ebp, 1
0x0040645E  fadd
0x00406461  fstp
0x00406465  fld
0x00406468  fadd
0x0040646C  fstp
0x00406470  mov edx, [ecx+8]
0x00406473  mov al, [edx+18h]
0x00406476  not al
0x00406478  test al, 1
0x0040647A  jz loc_406495
0x0040647C  fld
0x00406480  add ebp, 1
0x00406483  fadd
0x00406486  fstp
0x0040648A  fld
0x0040648D  fadd
0x00406491  fstp
0x00406495  add ecx, 10h
0x00406498  sub esi, 1
0x0040649B  jnz loc_406402
0x004064A1  mov eax, [esp+1Ch+arg_0]
0x004064A5  mov [esp+1Ch+var_C], ebp
0x004064A9  cmp ebx, edi
0x004064AB  jge loc_4064E9
0x004064AD  sub edi, ebx
0x004064AF  lea edx, [eax+ebx*4+3D0h]
0x004064B6  mov esi, edi
0x004064B8  mov ecx, [edx]
0x004064BA  mov bl, [ecx+18h]
0x004064BD  not bl
0x004064BF  test bl, 1
0x004064C2  jz loc_4064DD
0x004064C4  fld
0x004064C8  add ebp, 1
0x004064CB  fadd
0x004064CE  fstp
0x004064D2  fld
0x004064D5  fadd
0x004064D9  fstp
0x004064DD  add edx, 4
0x004064E0  sub esi, 1
0x004064E3  jnz loc_4064B8
0x004064E5  mov [esp+1Ch+var_C], ebp
0x004064E9  test ebp, ebp
0x004064EB  jnz loc_406508
0x004064ED  mov edx, [eax+34h]
0x004064F0  mov ecx, [esp+1Ch+arg_4]
0x004064F4  mov eax, [eax+38h]
0x004064F7  mov [ecx+4], eax
0x004064FA  mov [ecx], edx
0x004064FC  mov eax, ecx
0x004064FE  pop edi
0x004064FF  pop esi
0x00406500  pop ebp
0x00406501  pop ebx
0x00406502  add esp, 0Ch
0x00406505  retn 8
0x00406508  fild
0x0040650C  mov eax, [esp+1Ch+arg_4]
0x00406510  fld1
0x00406512  pop edi
0x00406513  fdivrp st(1), st
0x00406515  pop esi
0x00406516  pop ebp
0x00406517  pop ebx
0x00406518  fstp
0x0040651C  fld
0x00406520  fld
0x00406524  fmul st, st(1)
0x00406526  fstp
0x00406528  fmul
0x0040652C  fstp
0x0040652F  add esp, 0Ch
0x00406532  retn 8
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
