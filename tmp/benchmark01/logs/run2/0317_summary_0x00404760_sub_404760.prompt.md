# Function summary: sub_404760 (0x00404760)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00404760
- Name: sub_404760 (auto-generated placeholder)
- Size: 279 bytes, 87 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_406760 -> sub_404880 -> sub_404760

## How callers use this function

### From sub_406C90 (0x00406C90) - call at 0x00406D77, 0x00406EC3

What that caller does overall (preliminary): Evaluates and processes an entity's internal collection of sub-components or spatial points, performing filtering, distance or threshold checks, and invoking batch update/routing routines (sub_404760, sub_4062E0) based on state flags and criteria.

Called twice (at 0x00406D77 and 0x00406EC3) to process a filtered batch of entity elements. In the first call, it passes the main entity pointer (`ebp`), a local array of collected element pointers (`ecx`), the count (`esi`), and constant configuration values (3, 0, and `ebp+34h`) when elements are found. In the second call, it passes the main entity pointer (`ebp`), a secondary collection (`var_60`), a count (`var_DC`), and pointer/integer arguments (`4`, `ecx+8`, `ecx+4`) to perform a subsequent batch operation.

### From sub_404880 (0x00404880) - call at 0x004048F8

What that caller does overall (preliminary): Filters or collects a list of sub-components/entities from the simulation entity in ebx based on state flags and counts, storing pointers in a local buffer, and then delegates to sub_404760 along with parameters passed on the stack (arg_0, arg_4).

Called if any matching items were found/collected (esi > 0), passing a pointer to the local entity pointer array on the stack, the entity pointer (ebx), the count (esi), and two additional arguments (arg_0 and arg_4) passed into the current function.

## Functions it calls or references

### sub_4087D0 [SerializeCommandPacket] (0x004087D0) - call at 0x00404852

- Prototype: `bool __fastcall SerializeCommandPacket(BitStream *stream, const CommandPacket *packet)`
- Summary: Serializes a complex command or action packet structure into a bitstream using bit-packing, varints, and normalized floats.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00404760  sub esp, 48h
0x00404763  push ebx
0x00404764  push ebp
0x00404765  mov ebp, [esp+50h+arg_8]
0x00404769  push esi
0x0040476A  push edi
0x0040476B  xor edi, edi
0x0040476D  test ebp, ebp
0x0040476F  jle loc_40486D
0x00404775  cmp [esp+58h+arg_10], edi
0x00404779  mov ebx, dword_42AC60                        ; -> 0x0042AC60 in .data
0x0040477F  setz cl
0x00404782  lea ecx, [ecx+ecx+2]
0x00404786  mov byte ptr [esp+58h+arg_8], cl
0x0040478A  mov eax, [esp+58h+arg_0]
0x0040478E  mov dl, [eax]
0x00404790  xor al, al
0x00404792  cmp edi, ebp
0x00404794  mov [esp+58h+var_48], 0
0x00404799  mov [esp+58h+var_47], dl
0x0040479D  mov [esp+58h+var_45], 0
0x004047A2  mov [esp+58h+var_46], al
0x004047A6  jge loc_4047CE
0x004047A8  cmp al, 0Ch
0x004047AA  jnb loc_4047CE
0x004047AC  mov edx, [esp+58h+arg_4]
0x004047B0  mov edx, [edx+edi*4]
0x004047B3  mov edx, [edx+4]
0x004047B6  movzx eax, al
0x004047B9  mov [esp+eax*4+58h+var_44], edx
0x004047BD  mov al, [esp+58h+var_46]
0x004047C1  add al, 1
0x004047C3  add edi, 1
0x004047C6  cmp edi, ebp
0x004047C8  mov [esp+58h+var_46], al
0x004047CC  jl loc_4047A8
0x004047CE  cmp dword ptr [ebx+5472Ch], 3C00h
0x004047D8  mov al, [esp+58h+arg_C]
0x004047DC  mov edx, [esp+58h+arg_10]
0x004047E0  mov [esp+58h+var_14], al
0x004047E4  mov eax, [esp+58h+arg_14]
0x004047E8  mov [esp+58h+var_10], edx
0x004047EC  mov edx, [eax]
0x004047EE  mov eax, [eax+4]
0x004047F1  mov [esp+58h+var_13], cl
0x004047F5  mov [esp+58h+var_12], 0
0x004047FC  mov [esp+58h+var_C], edx
0x00404800  mov [esp+58h+var_8], eax
0x00404804  jbe loc_40480F
0x00404806  add dword ptr [ebx+5473Ch], 1
0x0040480D  jmp loc_404865
0x0040480F  cmp byte ptr [ebx+54730h], 0
0x00404816  lea esi, [ebx+54724h]
0x0040481C  jnz loc_40484D
0x0040481E  mov edx, [esi+4]
0x00404821  mov ecx, [esi+8]
0x00404824  add edx, edx
0x00404826  add edx, edx
0x00404828  add edx, edx
0x0040482A  lea eax, [ecx+1]
0x0040482D  cmp eax, edx
0x0040482F  jbe loc_404837
0x00404831  mov byte ptr [esi+0Ch], 1
0x00404835  jmp loc_40484D
0x00404837  mov eax, [esi]
0x00404839  mov edx, ecx
0x0040483B  shr edx, 3
0x0040483E  add eax, edx
0x00404840  and ecx, 7
0x00404843  mov dl, 1
0x00404845  shl dl, cl
0x00404847  or [eax], dl
0x00404849  add dword ptr [esi+8], 1
0x0040484D  lea eax, [esp+58h+var_48]
0x00404851  push eax
0x00404852  call sub_4087D0                              ; -> sub_4087D0 [SerializeCommandPacket]
0x00404857  mov cl, byte ptr [esp+5Ch+arg_8]
0x0040485B  add esp, 4
0x0040485E  add dword ptr [ebx+54734h], 1
0x00404865  cmp edi, ebp
0x00404867  jl loc_40478A
0x0040486D  pop edi
0x0040486E  pop esi
0x0040486F  pop ebp
0x00404870  pop ebx
0x00404871  add esp, 48h
0x00404874  retn 18h
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
