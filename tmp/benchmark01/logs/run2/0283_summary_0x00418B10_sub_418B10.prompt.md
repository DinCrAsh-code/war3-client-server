# Function summary: sub_418B10 (0x00418B10)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. This function calls no other function, so its behavior is entirely in the disassembly below. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00418B10
- Name: sub_418B10 (auto-generated placeholder)
- Size: 356 bytes, 138 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404AD0 -> sub_4046B0 -> sub_4087D0 -> sub_418B10

## How callers use this function

### From sub_4087D0 (0x004087D0) - call at 0x0040883D, 0x004089A7, 0x004089B5

What that caller does overall (preliminary): Serializes and packs structured command/packet data into a bitstream buffer using a state-tracking context (passed in esi) and a parameter/configuration source structure (passed in ebp).

Called to write integer or bit-length fields (such as calculated bounds or word values from the parameter structure) into the serialization stream, receiving the context pointer in eax/esi and value in ecx.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00418B10  push ebx
0x00418B11  push ebp
0x00418B12  push esi
0x00418B13  push edi
0x00418B14  mov edi, ecx
0x00418B16  mov ebx, 1
0x00418B1B  jmp loc_418B20
0x00418B20  mov esi, edi
0x00418B22  and esi, 7Fh
0x00418B25  shr edi, 7
0x00418B28  cmp byte ptr [eax+0Ch], 0
0x00418B2C  jnz loc_418C61
0x00418B32  mov edx, [eax+4]
0x00418B35  mov ecx, [eax+8]
0x00418B38  add edx, edx
0x00418B3A  add edx, edx
0x00418B3C  add edx, edx
0x00418B3E  lea ebp, [ecx+8]
0x00418B41  cmp ebp, edx
0x00418B43  jbe loc_418B4D
0x00418B45  mov [eax+0Ch], bl
0x00418B48  jmp loc_418C61
0x00418B4D  mov edx, edi
0x00418B4F  neg edx
0x00418B51  sbb edx, edx
0x00418B53  and edx, 80h
0x00418B59  or edx, esi
0x00418B5B  test bl, dl
0x00418B5D  jz loc_418B76
0x00418B5F  mov ebx, [eax]
0x00418B61  mov esi, ecx
0x00418B63  shr esi, 3
0x00418B66  add esi, ebx
0x00418B68  and ecx, 7
0x00418B6B  mov bl, 1
0x00418B6D  shl bl, cl
0x00418B6F  or [esi], bl
0x00418B71  mov ebx, 1
0x00418B76  add [eax+8], ebx
0x00418B79  test dl, 2
0x00418B7C  mov ecx, [eax+8]
0x00418B7F  jz loc_418B98
0x00418B81  mov ebx, [eax]
0x00418B83  mov esi, ecx
0x00418B85  shr esi, 3
0x00418B88  add esi, ebx
0x00418B8A  and ecx, 7
0x00418B8D  mov bl, 1
0x00418B8F  shl bl, cl
0x00418B91  or [esi], bl
0x00418B93  mov ebx, 1
0x00418B98  add [eax+8], ebx
0x00418B9B  test dl, 4
0x00418B9E  mov ecx, [eax+8]
0x00418BA1  jz loc_418BBA
0x00418BA3  mov ebx, [eax]
0x00418BA5  mov esi, ecx
0x00418BA7  shr esi, 3
0x00418BAA  add esi, ebx
0x00418BAC  and ecx, 7
0x00418BAF  mov bl, 1
0x00418BB1  shl bl, cl
0x00418BB3  or [esi], bl
0x00418BB5  mov ebx, 1
0x00418BBA  add [eax+8], ebx
0x00418BBD  test dl, 8
0x00418BC0  mov ecx, [eax+8]
0x00418BC3  jz loc_418BDC
0x00418BC5  mov ebx, [eax]
0x00418BC7  mov esi, ecx
0x00418BC9  shr esi, 3
0x00418BCC  add esi, ebx
0x00418BCE  and ecx, 7
0x00418BD1  mov bl, 1
0x00418BD3  shl bl, cl
0x00418BD5  or [esi], bl
0x00418BD7  mov ebx, 1
0x00418BDC  add [eax+8], ebx
0x00418BDF  test dl, 10h
0x00418BE2  mov ecx, [eax+8]
0x00418BE5  jz loc_418BFE
0x00418BE7  mov ebx, [eax]
0x00418BE9  mov esi, ecx
0x00418BEB  shr esi, 3
0x00418BEE  add esi, ebx
0x00418BF0  and ecx, 7
0x00418BF3  mov bl, 1
0x00418BF5  shl bl, cl
0x00418BF7  or [esi], bl
0x00418BF9  mov ebx, 1
0x00418BFE  add [eax+8], ebx
0x00418C01  test dl, 20h
0x00418C04  mov ecx, [eax+8]
0x00418C07  jz loc_418C20
0x00418C09  mov ebx, [eax]
0x00418C0B  mov esi, ecx
0x00418C0D  shr esi, 3
0x00418C10  add esi, ebx
0x00418C12  and ecx, 7
0x00418C15  mov bl, 1
0x00418C17  shl bl, cl
0x00418C19  or [esi], bl
0x00418C1B  mov ebx, 1
0x00418C20  add [eax+8], ebx
0x00418C23  test dl, 40h
0x00418C26  mov ecx, [eax+8]
0x00418C29  jz loc_418C42
0x00418C2B  mov ebx, [eax]
0x00418C2D  mov esi, ecx
0x00418C2F  shr esi, 3
0x00418C32  add esi, ebx
0x00418C34  and ecx, 7
0x00418C37  mov bl, 1
0x00418C39  shl bl, cl
0x00418C3B  or [esi], bl
0x00418C3D  mov ebx, 1
0x00418C42  add [eax+8], ebx
0x00418C45  test dl, dl
0x00418C47  mov ecx, [eax+8]
0x00418C4A  jns loc_418C5E
0x00418C4C  mov esi, [eax]
0x00418C4E  mov edx, ecx
0x00418C50  shr edx, 3
0x00418C53  add esi, edx
0x00418C55  and ecx, 7
0x00418C58  mov dl, bl
0x00418C5A  shl dl, cl
0x00418C5C  or [esi], dl
0x00418C5E  add [eax+8], ebx
0x00418C61  test edi, edi
0x00418C63  jz loc_418C6F
0x00418C65  cmp byte ptr [eax+0Ch], 0
0x00418C69  jz loc_418B20
0x00418C6F  pop edi
0x00418C70  pop esi
0x00418C71  pop ebp
0x00418C72  pop ebx
0x00418C73  retn
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
