# Function summary: sub_4087D0 (0x004087D0)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x004087D0
- Name: sub_4087D0 (auto-generated placeholder)
- Size: 550 bytes, 209 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_404AD0 -> sub_4046B0 -> sub_4087D0

## How callers use this function

### From sub_404760 (0x00404760) - call at 0x00404852

What that caller does overall (preliminary): Iterates over a collection of entity or component pointers passed from its caller, constructs a local packet or request structure using these pointers and additional configuration parameters, checks against a global state buffer/limit, and dispatches the packed data via sub_4087D0 in a loop.

Called inside the main processing loop to submit or dispatch the constructed local packet/request structure (passed as a pointer on the stack via `lea eax, [esp+58h+var_48]`).

### From sub_406EF0 (0x00406EF0) - call at 0x0040706E

What that caller does overall (preliminary): Performs capability, cooldown, or state checks on an entity component (using tick/time data and table lookups), and if conditions are met, initializes and dispatches a resource or command structure via global managers.

Called after sub_418AA0, passing a pointer to a locally constructed parameter/command block on the stack (starting at var_48) initialized with entity attributes and state flags, to submit or process the action request.

### From sub_4046B0 (0x004046B0) - call at 0x0040473B

What that caller does overall (preliminary): Packs entity, action, and target parameters into a local packet or structure after performing global threshold checks on data at dword_42AC60, conditionally incrementing counters and invoking subroutines for action execution or event dispatch.

Called with a pointer to the locally constructed parameter/packet buffer (at [esp+var_48]), receiving an object or status result in esi from the resource returned by sub_418AA0, as part of dispatching the action or event.

## Functions it calls or references

### sub_418AA0 [BitStream_WriteBits] (0x00418AA0) - call at 0x004087E0, 0x004087EC, 0x004087F8, 0x0040880E

- Prototype: `void __fastcall BitStream_WriteBits(BitStream *stream, int value, int count)`
- Summary: Writes a specified number of bits from an integer value into a bitstream buffer, updating bit offsets and tracking overflow status.

### sub_418B10 [WriteBitStreamVarInt] (0x00418B10) - call at 0x0040883D, 0x004089A7, 0x004089B5

- Prototype: `void __fastcall WriteBitStreamVarInt(BitStreamContext *ctx, unsigned int value)`
- Summary: Serializes an integer value as a variable-length bit-packed integer (varint-style encoding using 7-bit chunks with continuation bits) into a bitstream buffer.

### sub_418C80 [BitStream_WriteNormalizedFloat] (0x00418C80) - call at 0x004089D0, 0x004089E5

- Prototype: `void __stdcall BitStream_WriteNormalizedFloat(BitStream *stream, float val, float min_val, float max_val, int num_bits)`
- Summary: Serializes and writes a floating-point value clamped to a specified range into a bitstream as a quantized fixed-point integer.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x004087D0  push ebx
0x004087D1  push ebp
0x004087D2  mov ebp, [esp+8+arg_0]
0x004087D6  movzx eax, byte ptr [ebp+0]
0x004087DA  push edi
0x004087DB  push 2
0x004087DD  push eax
0x004087DE  mov eax, esi
0x004087E0  call sub_418AA0                              ; -> sub_418AA0 [BitStream_WriteBits]
0x004087E5  movzx ecx, byte ptr [ebp+1]
0x004087E9  push 3
0x004087EB  push ecx
0x004087EC  call sub_418AA0                              ; -> sub_418AA0 [BitStream_WriteBits]
0x004087F1  movzx edx, byte ptr [ebp+2]
0x004087F5  push 4
0x004087F7  push edx
0x004087F8  call sub_418AA0                              ; -> sub_418AA0 [BitStream_WriteBits]
0x004087FD  xor eax, eax
0x004087FF  cmp [ebp+3], al
0x00408802  mov ebx, 1
0x00408807  setnz al
0x0040880A  push ebx
0x0040880B  push eax
0x0040880C  mov eax, esi
0x0040880E  call sub_418AA0                              ; -> sub_418AA0 [BitStream_WriteBits]
0x00408813  xor edi, edi
0x00408815  cmp byte ptr [ebp+2], 0
0x00408819  jbe loc_40884C
0x0040881B  jmp loc_408820
0x00408820  test edi, edi
0x00408822  jnz loc_408829
0x00408824  mov ecx, [ebp+4]
0x00408827  jmp loc_40883B
0x00408829  mov eax, [ebp+edi*4+4]
0x0040882D  sub eax, [ebp+edi*4+0]
0x00408831  mov ecx, eax
0x00408833  sar ecx, 1Fh
0x00408836  lea edx, [eax+eax]
0x00408839  xor ecx, edx
0x0040883B  mov eax, esi
0x0040883D  call sub_418B10                              ; -> sub_418B10 [WriteBitStreamVarInt]
0x00408842  movzx eax, byte ptr [ebp+2]
0x00408846  add edi, ebx
0x00408848  cmp edi, eax
0x0040884A  jl loc_408820
0x0040884C  cmp byte ptr [esi+0Ch], 0
0x00408850  movzx eax, byte ptr [ebp+34h]
0x00408854  jnz loc_4088FE
0x0040885A  mov edx, [esi+4]
0x0040885D  mov ecx, [esi+8]
0x00408860  add edx, edx
0x00408862  add edx, edx
0x00408864  add edx, edx
0x00408866  lea edi, [ecx+5]
0x00408869  cmp edi, edx
0x0040886B  jbe loc_408875
0x0040886D  mov [esi+0Ch], bl
0x00408870  jmp loc_4088FE
0x00408875  test bl, al
0x00408877  jz loc_40888B
0x00408879  mov edi, [esi]
0x0040887B  mov edx, ecx
0x0040887D  shr edx, 3
0x00408880  add edi, edx
0x00408882  and ecx, 7
0x00408885  mov dl, bl
0x00408887  shl dl, cl
0x00408889  or [edi], dl
0x0040888B  add [esi+8], ebx
0x0040888E  test al, 2
0x00408890  mov ecx, [esi+8]
0x00408893  jz loc_4088A7
0x00408895  mov edi, [esi]
0x00408897  mov edx, ecx
0x00408899  shr edx, 3
0x0040889C  add edi, edx
0x0040889E  and ecx, 7
0x004088A1  mov dl, bl
0x004088A3  shl dl, cl
0x004088A5  or [edi], dl
0x004088A7  add [esi+8], ebx
0x004088AA  test al, 4
0x004088AC  mov ecx, [esi+8]
0x004088AF  jz loc_4088C3
0x004088B1  mov edi, [esi]
0x004088B3  mov edx, ecx
0x004088B5  shr edx, 3
0x004088B8  add edi, edx
0x004088BA  and ecx, 7
0x004088BD  mov dl, bl
0x004088BF  shl dl, cl
0x004088C1  or [edi], dl
0x004088C3  add [esi+8], ebx
0x004088C6  test al, 8
0x004088C8  mov ecx, [esi+8]
0x004088CB  jz loc_4088DF
0x004088CD  mov edi, [esi]
0x004088CF  mov edx, ecx
0x004088D1  shr edx, 3
0x004088D4  add edi, edx
0x004088D6  and ecx, 7
0x004088D9  mov dl, bl
0x004088DB  shl dl, cl
0x004088DD  or [edi], dl
0x004088DF  add [esi+8], ebx
0x004088E2  test al, 10h
0x004088E4  mov ecx, [esi+8]
0x004088E7  jz loc_4088FB
0x004088E9  mov edx, [esi]
0x004088EB  mov eax, ecx
0x004088ED  shr eax, 3
0x004088F0  add eax, edx
0x004088F2  and ecx, 7
0x004088F5  mov dl, bl
0x004088F7  shl dl, cl
0x004088F9  or [eax], dl
0x004088FB  add [esi+8], ebx
0x004088FE  cmp byte ptr [esi+0Ch], 0
0x00408902  movzx edx, byte ptr [ebp+35h]
0x00408906  jnz loc_4089A1
0x0040890C  mov eax, [esi+4]
0x0040890F  mov ecx, [esi+8]
0x00408912  add eax, eax
0x00408914  add eax, eax
0x00408916  add eax, eax
0x00408918  lea edi, [ecx+4]
0x0040891B  cmp edi, eax
0x0040891D  jbe loc_408924
0x0040891F  mov [esi+0Ch], bl
0x00408922  jmp loc_4089A1
0x00408924  test bl, dl
0x00408926  jz loc_40893D
0x00408928  mov edi, [esi]
0x0040892A  mov eax, ecx
0x0040892C  and ecx, 7
0x0040892F  shl bl, cl
0x00408931  shr eax, 3
0x00408934  add eax, edi
0x00408936  or [eax], bl
0x00408938  mov ebx, 1
0x0040893D  add [esi+8], ebx
0x00408940  test dl, 2
0x00408943  mov ecx, [esi+8]
0x00408946  jz loc_40895F
0x00408948  mov edi, [esi]
0x0040894A  mov eax, ecx
0x0040894C  and ecx, 7
0x0040894F  mov bl, 1
0x00408951  shl bl, cl
0x00408953  shr eax, 3
0x00408956  add eax, edi
0x00408958  or [eax], bl
0x0040895A  mov ebx, 1
0x0040895F  add [esi+8], ebx
0x00408962  test dl, 4
0x00408965  mov ecx, [esi+8]
0x00408968  jz loc_408981
0x0040896A  mov edi, [esi]
0x0040896C  mov eax, ecx
0x0040896E  and ecx, 7
0x00408971  mov bl, 1
0x00408973  shl bl, cl
0x00408975  shr eax, 3
0x00408978  add eax, edi
0x0040897A  or [eax], bl
0x0040897C  mov ebx, 1
0x00408981  add [esi+8], ebx
0x00408984  test dl, 8
0x00408987  mov ecx, [esi+8]
0x0040898A  jz loc_40899E
0x0040898C  mov eax, [esi]
0x0040898E  mov edx, ecx
0x00408990  shr edx, 3
0x00408993  add eax, edx
0x00408995  and ecx, 7
0x00408998  mov dl, bl
0x0040899A  shl dl, cl
0x0040899C  or [eax], dl
0x0040899E  add [esi+8], ebx
0x004089A1  movzx ecx, word ptr [ebp+36h]
0x004089A5  mov eax, esi
0x004089A7  call sub_418B10                              ; -> sub_418B10 [WriteBitStreamVarInt]
0x004089AC  test byte ptr [ebp+35h], 0Ah
0x004089B0  jz loc_4089BA
0x004089B2  mov ecx, [ebp+38h]
0x004089B5  call sub_418B10                              ; -> sub_418B10 [WriteBitStreamVarInt]
0x004089BA  test byte ptr [ebp+35h], 4
0x004089BE  jz loc_4089EA
0x004089C0  fldz
0x004089C2  sub esp, 8
0x004089C5  fstp
0x004089C9  fld
0x004089CC  fstp
0x004089CF  push esi
0x004089D0  call sub_418C80                              ; -> sub_418C80 [BitStream_WriteNormalizedFloat]
0x004089D5  fldz
0x004089D7  sub esp, 8
0x004089DA  fstp
0x004089DE  fld
0x004089E1  fstp
0x004089E4  push esi
0x004089E5  call sub_418C80                              ; -> sub_418C80 [BitStream_WriteNormalizedFloat]
0x004089EA  xor eax, eax
0x004089EC  cmp [esi+0Ch], al
0x004089EF  pop edi
0x004089F0  pop ebp
0x004089F1  setz al
0x004089F4  pop ebx
0x004089F5  retn
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
