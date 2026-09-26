# Function summary: sub_403520 (0x00403520)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00403520
- Name: sub_403520 (auto-generated placeholder)
- Size: 386 bytes, 122 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403520

## How callers use this function

### From sub_403D30 (0x00403D30) - call at 0x00403D9A

What that caller does overall (preliminary): Executes a single step or tick of a simulation/update phase using the resource structure provided in ebp (passed from sub_403E50), periodically updating sub-components, checking step-based intervals (dividing counters by 5 and 10), and handling state or resource blocks.

Called unconditionally as part of the update sequence following sub_4034E0.

## Functions it calls or references

### sub_410D00 [UpdateElementState] (0x00410D00) - call at 0x00403543

- Prototype: `bool __stdcall UpdateElementState(void *pElement)`
- Summary: Evaluates, validates, and updates a state object or entity (passed via `esi`) against geometry/spatial constraints, invoking virtual callback methods and computing vector distance calculations.

### sub_4185D0 [AllocateCustomBlock] (0x004185D0) - call at 0x004035DD, 0x00403614

- Prototype: `void* __fastcall AllocateCustomBlock(int size)`
- Summary: Allocates a custom memory block of the requested size, utilizing a fixed-size memory pool allocator for small/medium allocations or falling back to malloc for large ones, and returns an inner payload pointer with a tracked header.

### memcpy (0x0041A22A) - call at 0x0040362D

- Summary: Import thunk: jumps straight to the imported API MSVCR80!memcpy.

## Imported APIs referenced

- MSVCR80!free

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00403520  sub esp, 10h
0x00403523  push ebx
0x00403524  xor ebx, ebx
0x00403526  cmp [edi+5739Ch], ebx
0x0040352C  push ebp
0x0040352D  push esi
0x0040352E  jle loc_403553
0x00403530  mov eax, [edi+57398h]
0x00403536  fld                                          ; -> 0x0041D2EC in .rdata
0x0040353C  mov esi, [eax+ebx*4]
0x0040353F  push ecx
0x00403540  fstp
0x00403543  call sub_410D00                              ; -> sub_410D00 [UpdateElementState]
0x00403548  add ebx, 1
0x0040354B  cmp ebx, [edi+5739Ch]
0x00403551  jl loc_403530
0x00403553  xor ebx, ebx
0x00403555  xor esi, esi
0x00403557  cmp [edi+5739Ch], esi
0x0040355D  mov [esp+1Ch+var_8], ebx
0x00403561  jle loc_403591
0x00403563  mov eax, [edi+57398h]
0x00403569  mov ecx, [eax+esi*4]
0x0040356C  cmp byte ptr [ecx+29h], 0
0x00403570  jz loc_40357C
0x00403572  mov edx, [ecx]
0x00403574  mov eax, [edx]
0x00403576  push 1
0x00403578  call eax
0x0040357A  jmp loc_403582
0x0040357C  mov [eax+ebx*4], ecx
0x0040357F  add ebx, 1
0x00403582  add esi, 1
0x00403585  cmp esi, [edi+5739Ch]
0x0040358B  jl loc_403563
0x0040358D  mov [esp+1Ch+var_8], ebx
0x00403591  mov eax, [edi+573A0h]
0x00403597  cmp ebx, eax
0x00403599  jle loc_403695
0x0040359F  test eax, eax
0x004035A1  jz loc_4035AC
0x004035A3  lea ecx, [eax+eax]
0x004035A6  mov [esp+1Ch+var_C], ecx
0x004035AA  jmp loc_4035B4
0x004035AC  mov [esp+1Ch+var_C], 10h
0x004035B4  cmp [esp+1Ch+var_C], ebx
0x004035B8  jge loc_4035C9
0x004035BA  mov edx, [esp+1Ch+var_C]
0x004035BE  lea eax, [edx+edx]
0x004035C1  cmp eax, ebx
0x004035C3  mov [esp+1Ch+var_C], eax
0x004035C7  jl loc_4035BA
0x004035C9  mov ebx, [esp+1Ch+var_C]
0x004035CD  mov ebp, [edi+57398h]
0x004035D3  add ebx, ebx
0x004035D5  add ebx, ebx
0x004035D7  test ebp, ebp
0x004035D9  jnz loc_4035E7
0x004035DB  mov eax, ebx
0x004035DD  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004035E2  jmp loc_403674
0x004035E7  mov al, [ebp-6]
0x004035EA  cmp al, 0FFh
0x004035EC  lea esi, [ebp-8]
0x004035EF  jz loc_403612
0x004035F1  movzx eax, al
0x004035F4  movzx ecx, ds:word_41B150[eax*2]             ; -> 0x0041B150 in .rdata
0x004035FC  cmp ebx, ecx
0x004035FE  ja loc_403612
0x00403600  mov edx, ebx
0x00403602  sub edx, [esi+4]
0x00403605  mov [esi+4], ebx
0x00403608  add dword_42281C, edx                        ; -> 0x0042281C in .data
0x0040360E  mov eax, ebp
0x00403610  jmp loc_403674
0x00403612  mov eax, ebx
0x00403614  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00403619  test eax, eax
0x0040361B  mov [esp+1Ch+var_4], eax
0x0040361F  jz loc_403674
0x00403621  mov ecx, [esi+4]
0x00403624  cmp ecx, ebx
0x00403626  jnb loc_40362A
0x00403628  mov ebx, ecx
0x0040362A  push ebx
0x0040362B  push ebp
0x0040362C  push eax
0x0040362D  call memcpy                                  ; -> memcpy
0x00403632  add esp, 0Ch
0x00403635  cmp word ptr [esi], 4D52h
0x0040363A  jnz loc_403670
0x0040363C  mov eax, [esi+4]
0x0040363F  sub dword_42281C, eax                        ; -> 0x0042281C in .data
0x00403645  mov al, [esi+2]
0x00403648  cmp al, 0FFh
0x0040364A  mov word ptr [esi], 0
0x0040364F  jnz loc_40365D
0x00403651  push esi
0x00403652  call ds:free                                 ; -> import MSVCR80!free
0x00403658  add esp, 4
0x0040365B  jmp loc_403670
0x0040365D  movzx eax, al
0x00403660  mov ecx, dword_422820[eax*4]                 ; -> 0x00422820 in .data
0x00403667  mov [esi], ecx
0x00403669  mov dword_422820[eax*4], esi                 ; -> 0x00422820 in .data
0x00403670  mov eax, [esp+1Ch+var_4]
0x00403674  mov edx, [esp+1Ch+var_C]
0x00403678  mov [edi+57398h], eax
0x0040367E  mov eax, [esp+1Ch+var_8]
0x00403682  mov [edi+573A0h], edx
0x00403688  mov [edi+5739Ch], eax
0x0040368E  pop esi
0x0040368F  pop ebp
0x00403690  pop ebx
0x00403691  add esp, 10h
0x00403694  retn
0x00403695  pop esi
0x00403696  pop ebp
0x00403697  mov [edi+5739Ch], ebx
0x0040369D  pop ebx
0x0040369E  add esp, 10h
0x004036A1  retn
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
