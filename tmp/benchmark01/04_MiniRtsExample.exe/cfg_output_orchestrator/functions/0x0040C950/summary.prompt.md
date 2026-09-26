# Function summary: sub_40C950 (0x0040C950)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x0040C950
- Name: sub_40C950 (auto-generated placeholder)
- Size: 329 bytes, 122 instructions
- One call chain from the analysis root down to it: _main -> sub_403E50 -> sub_403D30 -> sub_403460 -> sub_4079D0 -> sub_4071C0 -> sub_40C950

## How callers use this function

### From sub_40CAA0 (0x0040CAA0) - call at 0x0040CAAC

What that caller does overall (preliminary): Acts as a callback/comparator function (passed to an entity collection or spatial query routine) that validates a specific entity/component pair against multiple criteria flags and structural conditions before conditionally invoking a downstream handling routine.

Called at the beginning of the function, passing the two input arguments (`arg_0` as `esi` and `arg_4` as `edi`) to perform an initial preliminary check or compatibility test between them. If the return value (`al`) is zero, the function immediately aborts and returns false.

### From sub_406000 (0x00406000) - address taken at 0x00406029

What that caller does overall (preliminary): Calculates spatial proximity, range, or distance-based metrics for a simulation entity relative to a set of other entities or spatial targets, querying a spatial partition or container via a callback function and aggregating numerical results.

Passed as a callback function pointer (along with stack space for results and search parameters) to sub_4177F0, likely serving as a predicate or collector callback invoked during spatial range-query or collision/proximity checks.

### From sub_4071C0 (0x004071C0) - address taken at 0x00407205, 0x004074E1

What that caller does overall (preliminary): Evaluates candidate targets or points within range/sight for an entity component's action/spell/ability (such as selecting targets based on criteria, line of sight, and positional filters) and dispatches action execution via sub_4046B0 if a valid target is found.

Passed as a callback function pointer in spatial query/filtering calls (along with entity and search parameters) to test or filter candidate entities.

Reference kinds used above:
- *address taken*: the function's address is used as data there - e.g. passed as a callback or stored as a function pointer - rather than called directly.

## Functions it calls or references

### sub_4042B0 [CheckEntityTypeMatch] (0x004042B0) - call at 0x0040C9A5

- Prototype: `bool __fastcall CheckEntityTypeMatch(const void *entity, unsigned int entityType)`
- Summary: Evaluates whether a given entity matches a target entity type or bitmask category, returning true if it matches and false otherwise.

### sub_403320 [CheckEntityCriteriaMatch] (0x00403320) - call at 0x0040CA7E

- Prototype: `bool __fastcall CheckEntityCriteriaMatch(const void *entityPtr, int param)`
- Summary: Evaluates a specific filter criterion on an entity by inspecting its status flags, type attributes, and either checking a bitfield or falling back to a grid lookup function.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x0040C950  push edi
0x0040C951  mov edi, [esp+4+arg_0]
0x0040C955  mov al, [edi+18h]
0x0040C958  not al
0x0040C95A  test al, 1
0x0040C95C  jz loc_40CA95
0x0040C962  test word ptr [edi+18h], 202h
0x0040C968  jnz loc_40CA95
0x0040C96E  push ebx
0x0040C96F  push ebp
0x0040C970  mov ebp, [esp+0Ch+arg_4]
0x0040C974  mov ebx, [ebp+8]
0x0040C977  test ebx, 200h
0x0040C97D  jz loc_40C98A
0x0040C97F  cmp edi, [ebp+0]
0x0040C982  jnz loc_40C98A
0x0040C984  pop ebp
0x0040C985  pop ebx
0x0040C986  xor al, al
0x0040C988  pop edi
0x0040C989  retn
0x0040C98A  push esi
0x0040C98B  mov esi, [ebp+4]
0x0040C98E  imul esi
0x0040C994  add esi, dword_42AC60                        ; -> 0x0042AC60 in .data
0x0040C99A  test bl, 1
0x0040C99D  jz loc_40C9B2
0x0040C99F  movzx ecx, byte ptr [edi+1Bh]
0x0040C9A3  mov eax, esi
0x0040C9A5  call sub_4042B0                              ; -> sub_4042B0 [CheckEntityTypeMatch]
0x0040C9AA  test al, al
0x0040C9AC  jz loc_40CA87
0x0040C9B2  test bl, 2
0x0040C9B5  jz loc_40C9DC
0x0040C9B7  movzx ecx, byte ptr [edi+1Bh]
0x0040C9BB  cmp ecx, 5
0x0040C9BE  ja loc_40CA87
0x0040C9C4  mov edx, 1
0x0040C9C9  shl edx, cl
0x0040C9CB  test [esi+0E8h], edx
0x0040C9D1  setnz al
0x0040C9D4  test al, al
0x0040C9D6  jz loc_40CA87
0x0040C9DC  test bl, 20h
0x0040C9DF  jz loc_40C9ED
0x0040C9E1  mov eax, [edi+1Ch]
0x0040C9E4  test byte ptr [eax], 1
0x0040C9E7  jnz loc_40CA87
0x0040C9ED  test bl, 10h
0x0040C9F0  jz loc_40C9FE
0x0040C9F2  mov ecx, [edi+1Ch]
0x0040C9F5  test byte ptr [ecx], 1
0x0040C9F8  jz loc_40CA87
0x0040C9FE  test bl, 40h
0x0040CA01  jz loc_40CA0E
0x0040CA03  mov edx, [edi+1Ch]
0x0040CA06  mov al, [edx]
0x0040CA08  shr al, 1
0x0040CA0A  test al, 1
0x0040CA0C  jz loc_40CA87
0x0040CA0E  mov eax, ebx
0x0040CA10  shr eax, 2
0x0040CA13  mov ecx, ebx
0x0040CA15  shr ecx, 3
0x0040CA18  and al, 1
0x0040CA1A  and cl, 1
0x0040CA1D  test al, al
0x0040CA1F  jz loc_40CA32
0x0040CA21  test cl, cl
0x0040CA23  jnz loc_40CA36
0x0040CA25  mov edx, [edi+1Ch]
0x0040CA28  mov dl, [edx]
0x0040CA2A  shr dl, 3
0x0040CA2D  test dl, 1
0x0040CA30  jnz loc_40CA87
0x0040CA32  test cl, cl
0x0040CA34  jz loc_40CA47
0x0040CA36  test al, al
0x0040CA38  jnz loc_40CA47
0x0040CA3A  mov eax, [edi+1Ch]
0x0040CA3D  mov cl, [eax]
0x0040CA3F  shr cl, 3
0x0040CA42  test cl, 1
0x0040CA45  jz loc_40CA87
0x0040CA47  test bl, bl
0x0040CA49  jns loc_40CA5D
0x0040CA4B  fld
0x0040CA4E  fld
0x0040CA51  fmul
0x0040CA54  fcompp
0x0040CA56  fnstsw ax
0x0040CA58  test ah, 41h
0x0040CA5B  jnp loc_40CA87
0x0040CA5D  test ebx, 400h
0x0040CA63  jz loc_40CA71
0x0040CA65  fldz
0x0040CA67  fcomp
0x0040CA6A  fnstsw ax
0x0040CA6C  test ah, 1
0x0040CA6F  jz loc_40CA87
0x0040CA71  test ebx, 100h
0x0040CA77  jz loc_40CA8E
0x0040CA79  mov eax, [ebp+4]
0x0040CA7C  mov edx, edi
0x0040CA7E  call sub_403320                              ; -> sub_403320 [CheckEntityCriteriaMatch]
0x0040CA83  test al, al
0x0040CA85  jnz loc_40CA8E
0x0040CA87  pop esi
0x0040CA88  pop ebp
0x0040CA89  pop ebx
0x0040CA8A  xor al, al
0x0040CA8C  pop edi
0x0040CA8D  retn
0x0040CA8E  pop esi
0x0040CA8F  pop ebp
0x0040CA90  pop ebx
0x0040CA91  mov al, 1
0x0040CA93  pop edi
0x0040CA94  retn
0x0040CA95  xor al, al
0x0040CA97  pop edi
0x0040CA98  retn
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
