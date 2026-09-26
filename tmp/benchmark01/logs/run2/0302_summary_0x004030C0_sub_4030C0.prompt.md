# Function summary: sub_4030C0 (0x004030C0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004030C0
- Name: sub_4030C0 (auto-generated placeholder)
- Size: 596 bytes, 202 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_4079D0 -> sub_4070A0 -> sub_4030C0

## How callers use this function

### From sub_4070A0 (0x004070A0) - call at 0x0040715B

What that caller does overall (preliminary): Evaluates unit/entity state, timers, and specific component/sub-item conditions (such as checking slot arrays and thresholds) to trigger actions or interact with other game entities via helper routines.

Called with a pointer derived from esi ([esi+8]) to retrieve a related entity, component, or target pointer, whose validity and type properties are then checked before initiating further action.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004030C0  sub esp, 0Ch
0x004030C3  fld
0x004030C7  push ebx
0x004030C8  mov ebx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x004030CE  fmul st, st
0x004030D0  push ebp
0x004030D1  mov ebp, [esp+14h+arg_0]
0x004030D5  push esi
0x004030D6  fstp
0x004030DA  mov esi, [ebx+573A8h]
0x004030E0  xor eax, eax
0x004030E2  cmp esi, 4
0x004030E5  mov [esp+18h+var_8], eax
0x004030E9  jl loc_403294
0x004030EF  mov edx, [ebx+573A4h]
0x004030F5  push edi
0x004030F6  lea edi, [esi-4]
0x004030F9  shr edi, 2
0x004030FC  add edx, 8
0x004030FF  add edi, 1
0x00403102  lea eax, ds:0[edi*4]
0x00403109  mov [esp+1Ch+var_4], eax
0x0040310D  lea ecx, [ecx+0]
0x00403110  mov ecx, [edx-8]
0x00403113  mov al, [ecx+18h]
0x00403116  not al
0x00403118  test al, 1
0x0040311A  jz loc_40316D
0x0040311C  cmp dword ptr [ecx+20h], 0
0x00403120  jnz loc_40316D
0x00403122  test byte ptr [ecx+18h], 2
0x00403126  jnz loc_40316D
0x00403128  fld
0x0040312B  fsub
0x0040312E  fstp
0x00403132  fld
0x00403135  fsub
0x00403138  fstp
0x0040313C  fld
0x00403140  fld
0x00403144  fmul st, st
0x00403146  fld
0x00403148  fmulp st(2), st
0x0040314A  faddp st(1), st
0x0040314C  fstp
0x00403150  fld
0x00403154  fld
0x00403158  fcomp
0x0040315A  fnstsw ax
0x0040315C  test ah, 41h
0x0040315F  jnz loc_40316B
0x00403161  fstp
0x00403165  mov [esp+1Ch+var_8], ecx
0x00403169  jmp loc_40316D
0x0040316B  fstp
0x0040316D  mov ecx, [edx-4]
0x00403170  mov al, [ecx+18h]
0x00403173  not al
0x00403175  test al, 1
0x00403177  jz loc_4031CA
0x00403179  cmp dword ptr [ecx+20h], 0
0x0040317D  jnz loc_4031CA
0x0040317F  test byte ptr [ecx+18h], 2
0x00403183  jnz loc_4031CA
0x00403185  fld
0x00403188  fsub
0x0040318B  fstp
0x0040318F  fld
0x00403192  fsub
0x00403195  fstp
0x00403199  fld
0x0040319D  fld
0x004031A1  fmul st, st
0x004031A3  fld
0x004031A5  fmulp st(2), st
0x004031A7  faddp st(1), st
0x004031A9  fstp
0x004031AD  fld
0x004031B1  fld
0x004031B5  fcomp
0x004031B7  fnstsw ax
0x004031B9  test ah, 41h
0x004031BC  jnz loc_4031C8
0x004031BE  fstp
0x004031C2  mov [esp+1Ch+var_8], ecx
0x004031C6  jmp loc_4031CA
0x004031C8  fstp
0x004031CA  mov ecx, [edx]
0x004031CC  mov al, [ecx+18h]
0x004031CF  not al
0x004031D1  test al, 1
0x004031D3  jz loc_403226
0x004031D5  cmp dword ptr [ecx+20h], 0
0x004031D9  jnz loc_403226
0x004031DB  test byte ptr [ecx+18h], 2
0x004031DF  jnz loc_403226
0x004031E1  fld
0x004031E4  fsub
0x004031E7  fstp
0x004031EB  fld
0x004031EE  fsub
0x004031F1  fstp
0x004031F5  fld
0x004031F9  fld
0x004031FD  fmul st, st
0x004031FF  fld
0x00403201  fmulp st(2), st
0x00403203  faddp st(1), st
0x00403205  fstp
0x00403209  fld
0x0040320D  fld
0x00403211  fcomp
0x00403213  fnstsw ax
0x00403215  test ah, 41h
0x00403218  jnz loc_403224
0x0040321A  fstp
0x0040321E  mov [esp+1Ch+var_8], ecx
0x00403222  jmp loc_403226
0x00403224  fstp
0x00403226  mov ecx, [edx+4]
0x00403229  mov al, [ecx+18h]
0x0040322C  not al
0x0040322E  test al, 1
0x00403230  jz loc_403283
0x00403232  cmp dword ptr [ecx+20h], 0
0x00403236  jnz loc_403283
0x00403238  test byte ptr [ecx+18h], 2
0x0040323C  jnz loc_403283
0x0040323E  fld
0x00403241  fsub
0x00403244  fstp
0x00403248  fld
0x0040324B  fsub
0x0040324E  fstp
0x00403252  fld
0x00403256  fld
0x0040325A  fmul st, st
0x0040325C  fld
0x0040325E  fmulp st(2), st
0x00403260  faddp st(1), st
0x00403262  fstp
0x00403266  fld
0x0040326A  fld
0x0040326E  fcomp
0x00403270  fnstsw ax
0x00403272  test ah, 41h
0x00403275  jnz loc_403281
0x00403277  fstp
0x0040327B  mov [esp+1Ch+var_8], ecx
0x0040327F  jmp loc_403283
0x00403281  fstp
0x00403283  add edx, 10h
0x00403286  sub edi, 1
0x00403289  jnz loc_403110
0x0040328F  mov eax, [esp+1Ch+var_4]
0x00403293  pop edi
0x00403294  cmp eax, esi
0x00403296  jge loc_403307
0x00403298  mov ecx, [ebx+573A4h]
0x0040329E  lea edx, [ecx+eax*4]
0x004032A1  sub esi, eax
0x004032A3  mov ecx, [edx]
0x004032A5  mov al, [ecx+18h]
0x004032A8  not al
0x004032AA  test al, 1
0x004032AC  jz loc_4032FF
0x004032AE  cmp dword ptr [ecx+20h], 0
0x004032B2  jnz loc_4032FF
0x004032B4  test byte ptr [ecx+18h], 2
0x004032B8  jnz loc_4032FF
0x004032BA  fld
0x004032BD  fsub
0x004032C0  fstp
0x004032C4  fld
0x004032C7  fsub
0x004032CA  fstp
0x004032CE  fld
0x004032D2  fld
0x004032D6  fmul st, st
0x004032D8  fld
0x004032DA  fmulp st(2), st
0x004032DC  faddp st(1), st
0x004032DE  fstp
0x004032E2  fld
0x004032E6  fld
0x004032EA  fcomp
0x004032EC  fnstsw ax
0x004032EE  test ah, 41h
0x004032F1  jnz loc_4032FD
0x004032F3  fstp
0x004032F7  mov [esp+18h+var_8], ecx
0x004032FB  jmp loc_4032FF
0x004032FD  fstp
0x004032FF  add edx, 4
0x00403302  sub esi, 1
0x00403305  jnz loc_4032A3
0x00403307  mov eax, [esp+18h+var_8]
0x0040330B  pop esi
0x0040330C  pop ebp
0x0040330D  pop ebx
0x0040330E  add esp, 0Ch
0x00403311  retn 8
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
