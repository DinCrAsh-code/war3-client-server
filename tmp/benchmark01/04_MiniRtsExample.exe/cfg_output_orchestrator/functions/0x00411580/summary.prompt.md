# Function summary: sub_411580 (0x00411580)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00411580
- Name: sub_411580 (auto-generated placeholder)
- Size: 494 bytes, 158 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402050 -> sub_402560 -> sub_410720 -> sub_40B100 -> sub_411580

## How callers use this function

### From sub_40F3D0 (0x0040F3D0) - call at 0x0040F465

What that caller does overall (preliminary): Validates and transitions a resource/command state (passed in eax, with manager object in ecx), looking up or instantiating a sub-component via a helper when needed, and returns success (1) or failure (0).

Called when an existing matching component is not found in the manager's array (`ebx+210h`), to allocate or initialize a new component object; the resulting pointer is stored into the array and its virtual method at offset +4 is invoked.

### From sub_40B100 (0x0040B100) - call at 0x0040B2AB

What that caller does overall (preliminary): Initializes a newly allocated resource or sub-element structure passed in `ebx`, setting up its virtual method table pointer, configuring various state fields and floating-point parameters based on incoming arguments, and iterating through a lookup table to conditionally instantiate and register sub-components.

Called inside a loop (iterating 4 times) to instantiate or fetch a sub-resource when a table entry check succeeds (`cmp al, 0FFh` and state check `cmp byte ptr [ebx+5Eh], 5`). If successful, the returned object pointer is stored into an array within the main structure (`[ebx+ecx*4+210h]`), the count at `[ebx+5Eh]` is incremented, and a virtual method at offset +4 of the returned object is invoked.

## Functions it calls or references

### sub_4185D0 [AllocateCustomBlock] (0x004185D0) - call at 0x004115B5, 0x004115D1, 0x004115EE, 0x0041160B, 0x00411628, 0x00411645, 0x00411662, 0x0041167F (+8 more)

- Prototype: `void* __fastcall AllocateCustomBlock(int size)`
- Summary: Allocates a custom memory block of the requested size, utilizing a fixed-size memory pool allocator for small/medium allocations or falling back to malloc for large ones, and returns an inner payload pointer with a tracked header.

### sub_413870 [RtsSubComponentA_Init] (0x00413870) - call at 0x004115C6

- Prototype: `void* __stdcall RtsSubComponentA_Init(void *obj, int arg0, int arg1)`
- Summary: Initializes a specific sub-component object instance (case A of the factory dispatcher) using configuration parameters and a lookup table.

### sub_414150 [CComponentType2Factory_Constructor] (0x00414150) - call at 0x004115E3

- Prototype: `void* __fastcall sub_414150(void *this_ptr, void *edx_unused, int arg_ecx, int arg_0)`
- Summary: Initializes a specific component object (case 2 factory type) using configuration tables and a provided parameter index.

### sub_4117B0 [RtsComponentType3_Constructor] (0x004117B0) - call at 0x00411600

- Prototype: `void* __fastcall RtsComponentType3_Constructor(void *this, void *edx, int arg_ecx, char arg_0)`
- Summary: Initializes a newly allocated component object (factory case 3) by setting up its virtual table, storing configuration parameters, clamping and scaling an index to reference a lookup table entry, and extracting flags.

### sub_4119C0 [RtsComponentCase4_Init] (0x004119C0) - call at 0x0041161D

- Prototype: `void __fastcall RtsComponentCase4_Init(void *this, void *edx_unused, int index, char arg1)`
- Summary: Initializes a specific RTS component object (case 4 in the factory dispatcher) using an index, a byte parameter, and a configuration table look-up.

### sub_411CB0 [FactoryCase5Constructor] (0x00411CB0) - call at 0x0041163A

- Prototype: `void* __fastcall FactoryCase5Constructor(void *this, int edx_unused, int ecx_val, char arg0)`
- Summary: Initializes a sub-component object (case 5 in the factory dispatcher) by setting up a virtual table, storing configuration indexes and pointers, and initializing internal state fields from a lookup table.

### sub_411DD0 [RtsComponentType6_Init] (0x00411DD0) - call at 0x00411657

- Prototype: `void __fastcall RtsComponentType6_Init(void *obj, int unused, int arg1)`
- Summary: Initializes a subtype 6 component object instance using a lookup table (`byte_41BDC0`) and index parameters.

### sub_4121B0 [RtsComponent7_Constructor] (0x004121B0) - call at 0x00411674

- Prototype: `void* __fastcall RtsComponent7_Constructor(void *this, int edx, int ecx_id, char arg_0)`
- Summary: Initializes a newly allocated component object (case 7 in the factory dispatcher) using an ID index, a configuration byte argument, and a lookup table in read-only memory.

### sub_412330 [RtsComponentType8_Constructor] (0x00412330) - call at 0x00411691

- Prototype: `void __fastcall RtsComponentType8_Constructor(void *this_ptr, void *edx_unused, int index, char arg0)`
- Summary: Initializes a newly allocated component object of type 8, setting up its virtual method table, configuration index, and lookup-derived flags.

### sub_4124B0 [SubComponentType9_Constructor] (0x004124B0) - call at 0x004116AE

- Prototype: `void* __stdcall SubComponentType9_Constructor(void *objectInstance, int index, char arg_0)`
- Summary: Initializes an object instance (allocated at `eax`) for factory case 9, setting up its virtual method table, boundary-checked configuration values, and reference data fetched from a lookup table (`byte_41BDC0`).

### sub_412950 [RtsComponent10_Constructor] (0x00412950) - call at 0x004116CB

- Prototype: `void* __fastcall RtsComponent10_Constructor(void *this_obj, int ecx, int arg0)`
- Summary: Initializes a specific RTS game component structure (factory case 10) by setting up its virtual method table, storing initialization parameters, and configuring internal flags based on a lookup table.

### sub_412C80 [RtsComponentType11_Init] (0x00412C80) - call at 0x004116E8

- Prototype: `void __fastcall RtsComponentType11_Init(void *pThis, int edx_unused, int arg_ecx, int arg_0)`
- Summary: Initializes a specific RTS component object (factory case 11) using configuration data from a lookup table and storing various parameters.

### sub_412D40 [RtsSubComponent12_Constructor] (0x00412D40) - call at 0x00411701

- Prototype: `void __fastcall RtsSubComponent12_Constructor(void *obj, void *edx_unused, int arg_ecx, int arg_stack0)`
- Summary: Initializes an object instance (case 12 sub-component) by setting up virtual method tables, storing configuration parameters, and calculating lookup properties from a static table.

### sub_412E50 [FactoryItem13_Constructor] (0x00412E50) - call at 0x0041171A

- Prototype: `void __fastcall FactoryItem13_Constructor(void *this_ptr, int unused, int arg_ecx, char arg_0)`
- Summary: Initializes a newly allocated component object (factory case 13) using parameters passed in registers and on the stack, setting up virtual table pointers and internal configuration fields.

### sub_413050 [InitializeComponentType14] (0x00413050) - call at 0x00411733

- Prototype: `void __fastcall InitializeComponentType14(void *this_obj, int edx_unused, int arg_ecx, int arg_stack)`
- Summary: Initializes a newly allocated component object (factory case 14) by setting up its virtual method table, initializing fields based on the provided index and argument, and performing lookup operations on a global configuration table.

### sub_413390 [RtsSubComponent15_Constructor] (0x00413390) - call at 0x0041174C

- Prototype: `void __stdcall RtsSubComponent15_Constructor(void *this_obj, int param_ecx, int param_arg0)`
- Summary: Initializes a specific sub-component object (case 15 factory instance) by setting up virtual tables, member fields, and bounds-checked lookup references.

### sub_4137C0 [ComponentTypeInfo_Initialize] (0x004137C0) - call at 0x00411765

- Prototype: `void __fastcall ComponentTypeInfo_Initialize(void *this_obj, int edx_unused, int type_index, char arg0)`
- Summary: Initializes a component descriptor/configuration object (`this` in `eax`) using a lookup table (`byte_41BDC0`) indexed by a validated `type_index` (`ecx`), and sets up virtual method table pointers.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00411580  xor eax, eax
0x00411582  cmp esi, 17h
0x00411585  ja locret_41176D
0x0041158B  cmp esi, 18h
0x0041158E  setnl al
0x00411591  sub eax, 1
0x00411594  and eax, esi
0x00411596  imul eax
0x00411599  movzx eax, ds:byte_41BDC0[eax]               ; -> 0x0041BDC0 in .rdata
0x004115A0  cmp eax, 0Fh
0x004115A3  ja def_4115A9
0x004115A9  jmp ds:jpt_4115A9[eax*4]                     ; -> 0x00411770 in .text
0x004115B0  mov eax, 28h
0x004115B5  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004115BA  test eax, eax
0x004115BC  jz def_4115A9
0x004115C2  push edi
0x004115C3  push esi
0x004115C4  mov edx, ebx
0x004115C6  call sub_413870                              ; -> sub_413870 [RtsSubComponentA_Init]
0x004115CB  retn
0x004115CC  mov eax, 20h
0x004115D1  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004115D6  test eax, eax
0x004115D8  jz def_4115A9
0x004115DE  push edi
0x004115DF  mov ecx, esi
0x004115E1  mov edx, ebx
0x004115E3  call sub_414150                              ; -> sub_414150 [CComponentType2Factory_Constructor]
0x004115E8  retn
0x004115E9  mov eax, 1Ch
0x004115EE  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004115F3  test eax, eax
0x004115F5  jz def_4115A9
0x004115FB  push edi
0x004115FC  mov ecx, esi
0x004115FE  mov edx, ebx
0x00411600  call sub_4117B0                              ; -> sub_4117B0 [RtsComponentType3_Constructor]
0x00411605  retn
0x00411606  mov eax, 1Ch
0x0041160B  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00411610  test eax, eax
0x00411612  jz def_4115A9
0x00411618  push edi
0x00411619  mov ecx, esi
0x0041161B  mov edx, ebx
0x0041161D  call sub_4119C0                              ; -> sub_4119C0 [RtsComponentCase4_Init]
0x00411622  retn
0x00411623  mov eax, 1Ch
0x00411628  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x0041162D  test eax, eax
0x0041162F  jz def_4115A9
0x00411635  push edi
0x00411636  mov ecx, esi
0x00411638  mov edx, ebx
0x0041163A  call sub_411CB0                              ; -> sub_411CB0 [FactoryCase5Constructor]
0x0041163F  retn
0x00411640  mov eax, 1Ch
0x00411645  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x0041164A  test eax, eax
0x0041164C  jz def_4115A9
0x00411652  push edi
0x00411653  mov ecx, esi
0x00411655  mov edx, ebx
0x00411657  call sub_411DD0                              ; -> sub_411DD0 [RtsComponentType6_Init]
0x0041165C  retn
0x0041165D  mov eax, 1Ch
0x00411662  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00411667  test eax, eax
0x00411669  jz def_4115A9
0x0041166F  push edi
0x00411670  mov ecx, esi
0x00411672  mov edx, ebx
0x00411674  call sub_4121B0                              ; -> sub_4121B0 [RtsComponent7_Constructor]
0x00411679  retn
0x0041167A  mov eax, 1Ch
0x0041167F  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00411684  test eax, eax
0x00411686  jz def_4115A9
0x0041168C  push edi
0x0041168D  mov ecx, esi
0x0041168F  mov edx, ebx
0x00411691  call sub_412330                              ; -> sub_412330 [RtsComponentType8_Constructor]
0x00411696  retn
0x00411697  mov eax, 28h
0x0041169C  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004116A1  test eax, eax
0x004116A3  jz def_4115A9
0x004116A9  push edi
0x004116AA  mov ecx, esi
0x004116AC  mov edx, ebx
0x004116AE  call sub_4124B0                              ; -> sub_4124B0 [SubComponentType9_Constructor]
0x004116B3  retn
0x004116B4  mov eax, 1Ch
0x004116B9  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004116BE  test eax, eax
0x004116C0  jz def_4115A9
0x004116C6  push edi
0x004116C7  mov ecx, esi
0x004116C9  mov edx, ebx
0x004116CB  call sub_412950                              ; -> sub_412950 [RtsComponent10_Constructor]
0x004116D0  retn
0x004116D1  mov eax, 1Ch
0x004116D6  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004116DB  test eax, eax
0x004116DD  jz def_4115A9
0x004116E3  push edi
0x004116E4  mov ecx, esi
0x004116E6  mov edx, ebx
0x004116E8  call sub_412C80                              ; -> sub_412C80 [RtsComponentType11_Init]
0x004116ED  retn
0x004116EE  mov eax, 1Ch
0x004116F3  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x004116F8  test eax, eax
0x004116FA  jz def_4115A9
0x004116FC  push edi
0x004116FD  mov ecx, esi
0x004116FF  mov edx, ebx
0x00411701  call sub_412D40                              ; -> sub_412D40 [RtsSubComponent12_Constructor]
0x00411706  retn
0x00411707  mov eax, 1Ch
0x0041170C  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00411711  test eax, eax
0x00411713  jz def_4115A9
0x00411715  push edi
0x00411716  mov ecx, esi
0x00411718  mov edx, ebx
0x0041171A  call sub_412E50                              ; -> sub_412E50 [FactoryItem13_Constructor]
0x0041171F  retn
0x00411720  mov eax, 1Ch
0x00411725  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x0041172A  test eax, eax
0x0041172C  jz def_4115A9
0x0041172E  push edi
0x0041172F  mov ecx, esi
0x00411731  mov edx, ebx
0x00411733  call sub_413050                              ; -> sub_413050 [InitializeComponentType14]
0x00411738  retn
0x00411739  mov eax, 1Ch
0x0041173E  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00411743  test eax, eax
0x00411745  jz def_4115A9
0x00411747  push edi
0x00411748  mov ecx, esi
0x0041174A  mov edx, ebx
0x0041174C  call sub_413390                              ; -> sub_413390 [RtsSubComponent15_Constructor]
0x00411751  retn
0x00411752  mov eax, 1Ch
0x00411757  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x0041175C  test eax, eax
0x0041175E  jz def_4115A9
0x00411760  push edi
0x00411761  mov ecx, esi
0x00411763  mov edx, ebx
0x00411765  call sub_4137C0                              ; -> sub_4137C0 [ComponentTypeInfo_Initialize]
0x0041176A  retn
0x0041176B  xor eax, eax
0x0041176D  retn
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
