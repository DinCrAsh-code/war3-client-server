# Function summary: sub_401490 (0x00401490)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00401490
- Name: sub_401490 (auto-generated placeholder)
- Size: 484 bytes, 172 instructions
- One call chain from the analysis root down to it: _main -> sub_401490

## How callers use this function

### From _main (0x00401050) - call at 0x00401240

What that caller does overall (preliminary): _main serves as the program's main entry point, handling initialization, command-line argument parsing, memory allocation for a resource block (approx. 0x5BBE4 bytes with a custom 'RM' header), execution/processing phases, and cleanup upon exit.

Called during cleanup if the resource pointer (esi) is non-zero, passing esi to perform finalization or resource teardown operations.

## Functions it calls or references

### sub_4013F0 [FreeResourceSubBlock] (0x004013F0) - call at 0x004015ED

- Prototype: `void __fastcall FreeResourceSubBlock(void *self)`
- Summary: Cleans up and deallocates or recycles two custom 'RM' header-tracked memory blocks referenced from offsets within a sub-component structure.

### sub_401690 [ResourceObject_Destructor] (0x00401690) - address taken at 0x004015F2

- Prototype: `void __thiscall ResourceObject_Destructor(void *this)`
- Summary: Destroys or unlinks an individual RM-header resource object, updating global tracking counters and either freeing its memory or returning it to a custom free-list pool.

### ??_M@YGXPAXIHP6EX0@Z@Z [vector_destructor] (0x0041995F) - call at 0x00401602

- Prototype: `void __stdcall vector_destructor(void *ptr, int element_size, int count, void (__cdecl *dtor)(void *))`
- Summary: Destroys an array of objects by invoking a provided element destructor function on each element in forward order, with SEH protection that delegates to __ArrayUnwind if an exception occurs mid-destruction.

### @__security_check_cookie@4 [__security_check_cookie] (0x004198F2) - call at 0x0041A339

- Prototype: `void __fastcall __security_check_cookie(uintptr_t stack_cookie)`
- Summary: Validates that the stack canary value passed in ecx matches the global security cookie, jumping to the GS failure handler if a mismatch is detected.

### __CxxFrameHandler3 (0x0041A224) - tail jump at 0x0041A343

- Summary: Import thunk: jumps straight to the imported API MSVCR80!__CxxFrameHandler3.

Reference kinds used above:
- *tail jump*: a `jmp` straight to the function's start, i.e. a tail call.
- *address taken*: the function's address is used as data there - e.g. passed as a callback or stored as a function pointer - rather than called directly.

## Imported APIs referenced

- MSVCR80!free

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00401490  push 0FFFFFFFFh
0x00401492  push offset SEH_401490                       ; -> 0x0041A32D in .text
0x00401497  mov eax, large fs:0
0x0040149D  push eax
0x0040149E  push ebx
0x0040149F  push esi
0x004014A0  push edi
0x004014A1  mov eax, ___security_cookie                  ; -> 0x0041F000 in .data
0x004014A6  xor eax, esp
0x004014A8  push eax
0x004014A9  lea eax, [esp+1Ch+var_C]
0x004014AD  mov large fs:0, eax
0x004014B3  mov edi, [esp+1Ch+arg_0]
0x004014B7  mov [esp+1Ch+var_4], 0
0x004014BF  mov eax, [edi+573B0h]
0x004014C5  test eax, eax
0x004014C7  mov ebx, ds:free                             ; -> import MSVCR80!free
0x004014CD  jz loc_401512
0x004014CF  add eax, 0FFFFFFF8h
0x004014D2  cmp word ptr [eax], 4D52h
0x004014D7  jnz loc_401512
0x004014D9  mov esi, dword_42281C                        ; -> 0x0042281C in .data
0x004014DF  sub esi, [eax+4]
0x004014E2  mov cl, [eax+2]
0x004014E5  cmp cl, 0FFh
0x004014E8  mov word ptr [eax], 0
0x004014ED  mov dword_42281C, esi                        ; -> 0x0042281C in .data
0x004014F3  jnz loc_4014FD
0x004014F5  push eax
0x004014F6  call ebx                                     ; -> import MSVCR80!free
0x004014F8  add esp, 4
0x004014FB  jmp loc_401518
0x004014FD  movzx ecx, cl
0x00401500  mov edx, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x00401507  mov [eax], edx
0x00401509  mov dword_422820[ecx*4], eax                 ; -> 0x00422820 in .data
0x00401510  jmp loc_401518
0x00401512  mov esi, dword_42281C                        ; -> 0x0042281C in .data
0x00401518  mov eax, [edi+573A4h]
0x0040151E  test eax, eax
0x00401520  jz loc_40155D
0x00401522  add eax, 0FFFFFFF8h
0x00401525  cmp word ptr [eax], 4D52h
0x0040152A  jnz loc_40155D
0x0040152C  sub esi, [eax+4]
0x0040152F  mov cl, [eax+2]
0x00401532  cmp cl, 0FFh
0x00401535  mov word ptr [eax], 0
0x0040153A  mov dword_42281C, esi                        ; -> 0x0042281C in .data
0x00401540  jnz loc_40154A
0x00401542  push eax
0x00401543  call ebx                                     ; -> import MSVCR80!free
0x00401545  add esp, 4
0x00401548  jmp loc_40155D
0x0040154A  movzx ecx, cl
0x0040154D  mov edx, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x00401554  mov [eax], edx
0x00401556  mov dword_422820[ecx*4], eax                 ; -> 0x00422820 in .data
0x0040155D  mov eax, [edi+57398h]
0x00401563  test eax, eax
0x00401565  jz loc_4015A2
0x00401567  add eax, 0FFFFFFF8h
0x0040156A  cmp word ptr [eax], 4D52h
0x0040156F  jnz loc_4015A2
0x00401571  sub esi, [eax+4]
0x00401574  mov cl, [eax+2]
0x00401577  cmp cl, 0FFh
0x0040157A  mov word ptr [eax], 0
0x0040157F  mov dword_42281C, esi                        ; -> 0x0042281C in .data
0x00401585  jnz loc_40158F
0x00401587  push eax
0x00401588  call ebx                                     ; -> import MSVCR80!free
0x0040158A  add esp, 4
0x0040158D  jmp loc_4015A2
0x0040158F  movzx ecx, cl
0x00401592  mov edx, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x00401599  mov [eax], edx
0x0040159B  mov dword_422820[ecx*4], eax                 ; -> 0x00422820 in .data
0x004015A2  mov eax, [edi+5738Ch]
0x004015A8  test eax, eax
0x004015AA  jz loc_4015E7
0x004015AC  add eax, 0FFFFFFF8h
0x004015AF  cmp word ptr [eax], 4D52h
0x004015B4  jnz loc_4015E7
0x004015B6  sub esi, [eax+4]
0x004015B9  mov cl, [eax+2]
0x004015BC  cmp cl, 0FFh
0x004015BF  mov word ptr [eax], 0
0x004015C4  mov dword_42281C, esi                        ; -> 0x0042281C in .data
0x004015CA  jnz loc_4015D4
0x004015CC  push eax
0x004015CD  call ebx                                     ; -> import MSVCR80!free
0x004015CF  add esp, 4
0x004015D2  jmp loc_4015E7
0x004015D4  movzx ecx, cl
0x004015D7  mov edx, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x004015DE  mov [eax], edx
0x004015E0  mov dword_422820[ecx*4], eax                 ; -> 0x00422820 in .data
0x004015E7  lea esi, [edi+54740h]
0x004015ED  call sub_4013F0                              ; -> sub_4013F0 [FreeResourceSubBlock]
0x004015F2  push offset sub_401690                       ; -> sub_401690 [ResourceObject_Destructor]
0x004015F7  push 7
0x004015F9  push 0Ch
0x004015FB  lea eax, [edi+51EA4h]
0x00401601  push eax
0x00401602  call ??_M@YGXPAXIHP6EX0@Z@Z                  ; -> ??_M@YGXPAXIHP6EX0@Z@Z [vector_destructor]
0x00401607  mov eax, [edi+51E90h]
0x0040160D  test eax, eax
0x0040160F  jz loc_40165F
0x00401611  add eax, 0FFFFFFF8h
0x00401614  cmp word ptr [eax], 4D52h
0x00401619  jnz loc_40165F
0x0040161B  mov ecx, [eax+4]
0x0040161E  sub dword_42281C, ecx                        ; -> 0x0042281C in .data
0x00401624  mov cl, [eax+2]
0x00401627  cmp cl, 0FFh
0x0040162A  mov word ptr [eax], 0
0x0040162F  jnz loc_40164C
0x00401631  push eax
0x00401632  call ebx                                     ; -> import MSVCR80!free
0x00401634  add esp, 4
0x00401637  mov ecx, [esp+1Ch+var_C]
0x0040163B  mov large fs:0, ecx
0x00401642  pop ecx
0x00401643  pop edi
0x00401644  pop esi
0x00401645  pop ebx
0x00401646  add esp, 0Ch
0x00401649  retn 4
0x0040164C  movzx ecx, cl
0x0040164F  mov edx, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x00401656  mov [eax], edx
0x00401658  mov dword_422820[ecx*4], eax                 ; -> 0x00422820 in .data
0x0040165F  mov ecx, [esp+1Ch+var_C]
0x00401663  mov large fs:0, ecx
0x0040166A  pop ecx
0x0040166B  pop edi
0x0040166C  pop esi
0x0040166D  pop ebx
0x0040166E  add esp, 0Ch
0x00401671  retn 4
0x004013A0  mov eax, [eax]
0x004013A2  test eax, eax
0x004013A4  jz locret_4013E4                             ; -> 0x004013E4 in .text
0x004013A6  add eax, 0FFFFFFF8h
0x004013A9  cmp word ptr [eax], 4D52h
0x004013AE  jnz locret_4013E4                            ; -> 0x004013E4 in .text
0x004013B0  mov ecx, [eax+4]
0x004013B3  sub dword_42281C, ecx                        ; -> 0x0042281C in .data
0x004013B9  mov cl, [eax+2]
0x004013BC  cmp cl, 0FFh
0x004013BF  mov word ptr [eax], 0
0x004013C4  jnz loc_4013D1                               ; -> 0x004013D1 in .text
0x004013C6  push eax
0x004013C7  call ds:free                                 ; -> import MSVCR80!free
0x004013CD  add esp, 4
0x004013D0  retn
0x004013D1  movzx ecx, cl
0x004013D4  mov edx, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x004013DB  mov [eax], edx
0x004013DD  mov dword_422820[ecx*4], eax                 ; -> 0x00422820 in .data
0x004013E4  retn
0x0041A320  mov eax, [ebp+4]
0x0041A323  add eax, 51E90h
0x0041A328  jmp loc_4013A0                               ; -> 0x004013A0 in .text
0x0041A32D  mov edx, [esp+arg_4]
0x0041A331  lea eax, [edx-0Ch]
0x0041A334  mov ecx, [edx-10h]
0x0041A337  xor ecx, eax
0x0041A339  call @__security_check_cookie@4              ; -> @__security_check_cookie@4 [__security_check_cookie]
0x0041A33E  mov eax, offset stru_41DDC0                  ; -> 0x0041DDC0 in .rdata
0x0041A343  jmp __CxxFrameHandler3                       ; -> __CxxFrameHandler3
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
