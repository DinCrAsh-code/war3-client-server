# Function summary: sub_404940 (0x00404940)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00404940
- Name: sub_404940 (auto-generated placeholder)
- Size: 398 bytes, 101 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404940

## How callers use this function

### From sub_403460 (0x00403460) - call at 0x0040349F

What that caller does overall (preliminary): Iterates over a collection of simulation or resource entities (up to a count at [esi+5BBBCh], stepping by 4FCh bytes from offset 0x630), performing modulo-10 and flag-based checks on each element before invoking a batch of update/processing routines.

Called inside the loop for the current entity (with `eax` pointing to `edi`, the entity structure at `esi+630h + ebp*4FCh`) when interval and flag conditions are met.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00404940  push ebp
0x00404941  mov ebp, dword_42AC60                        ; -> 0x0042AC60 in .data
0x00404947  push edi
0x00404948  xor edi, edi
0x0040494A  mov [eax+244h], edi
0x00404950  mov [eax+3C8h], edi
0x00404956  mov [eax+3CCh], edi
0x0040495C  mov [eax+3DCh], edi
0x00404962  mov [eax+4E0h], edi
0x00404968  mov [eax+4F4h], edi
0x0040496E  mov [eax+4F8h], edi
0x00404974  cmp [ebp+57390h], edi
0x0040497A  jle loc_404AB1
0x00404980  push ebx
0x00404981  push esi
0x00404982  mov ebx, 1
0x00404987  jmp loc_404990
0x00404990  mov ecx, [ebp+5738Ch]
0x00404996  mov ecx, [ecx+edi*4]
0x00404999  movzx edx, byte ptr [ecx+1Bh]
0x0040499D  cmp edx, [eax]
0x0040499F  jnz loc_404A9F
0x004049A5  mov esi, [ecx+1Ch]
0x004049A8  movzx edx, word ptr [esi]
0x004049AB  mov bl, dl
0x004049AD  shr bl, 1
0x004049AF  test bl, 1
0x004049B2  jz loc_4049DA
0x004049B4  mov edx, [eax+3DCh]
0x004049BA  cmp edx, 3
0x004049BD  mov ebx, 1
0x004049C2  jge loc_404A9F
0x004049C8  mov [eax+edx*4+3D0h], ecx
0x004049CF  add [eax+3DCh], ebx
0x004049D5  jmp loc_404A9F
0x004049DA  mov bl, [ecx+18h]
0x004049DD  not bl
0x004049DF  test bl, 1
0x004049E2  mov ebx, 1
0x004049E7  jz loc_404A9F
0x004049ED  test [esi], bl
0x004049EF  jz loc_404A39
0x004049F1  mov edx, [eax+4E0h]
0x004049F7  cmp edx, 40h
0x004049FA  jge loc_404A09
0x004049FC  mov [eax+edx*4+3E0h], ecx
0x00404A03  add [eax+4E0h], ebx
0x00404A09  mov edx, [ecx+1Ch]
0x00404A0C  test byte ptr [edx], 40h
0x00404A0F  jz loc_404A9F
0x00404A15  test byte ptr [ecx+18h], 10h
0x00404A19  jnz loc_404A9F
0x00404A1F  mov edx, [eax+4F4h]
0x00404A25  cmp edx, 4
0x00404A28  jge loc_404A9F
0x00404A2A  mov [eax+edx*4+4E4h], ecx
0x00404A31  add [eax+4F4h], ebx
0x00404A37  jmp loc_404A9F
0x00404A39  shr dl, 2
0x00404A3C  test bl, dl
0x00404A3E  jz loc_404A6E
0x00404A40  mov edx, [eax+244h]
0x00404A46  cmp edx, 40h
0x00404A49  jge loc_404A58
0x00404A4B  mov [eax+edx*4+144h], ecx
0x00404A52  add [eax+244h], ebx
0x00404A58  cmp byte ptr [ecx+70h], 0
0x00404A5C  jnz loc_404A9F
0x00404A5E  test word ptr [ecx+18h], 200h
0x00404A64  jnz loc_404A9F
0x00404A66  add [eax+4F8h], ebx
0x00404A6C  jmp loc_404A9F
0x00404A6E  test byte ptr [ecx+18h], 2
0x00404A72  jnz loc_404A9F
0x00404A74  mov edx, [eax+3C8h]
0x00404A7A  cmp edx, 60h
0x00404A7D  jge loc_404A8C
0x00404A7F  mov [eax+edx*4+248h], ecx
0x00404A86  add [eax+3C8h], ebx
0x00404A8C  mov ecx, [ecx+1Ch]
0x00404A8F  movsx ecx, byte ptr [ecx+2Eh]
0x00404A93  cmp ecx, ebx
0x00404A95  jg loc_404A99
0x00404A97  mov ecx, ebx
0x00404A99  add [eax+3CCh], ecx
0x00404A9F  add edi, ebx
0x00404AA1  cmp edi, [ebp+57390h]
0x00404AA7  jl loc_404990
0x00404AAD  pop esi
0x00404AAE  xor edi, edi
0x00404AB0  pop ebx
0x00404AB1  cmp [eax+4F4h], edi
0x00404AB7  pop edi
0x00404AB8  pop ebp
0x00404AB9  jle locret_404ACD
0x00404ABB  mov ecx, [eax+4E4h]
0x00404AC1  mov edx, [ecx+8]
0x00404AC4  mov [eax+34h], edx
0x00404AC7  mov ecx, [ecx+0Ch]
0x00404ACA  mov [eax+38h], ecx
0x00404ACD  retn
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
