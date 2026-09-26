# Function summary: sub_401750 (0x00401750)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00401750
- Name: sub_401750 (auto-generated placeholder)
- Size: 2060 bytes, 560 instructions
- One call chain from the analysis root down to it: _main -> sub_401750

## How callers use this function

### From _main (0x00401050) - call at 0x00401225

What that caller does overall (preliminary): _main serves as the program's main entry point, handling initialization, command-line argument parsing, memory allocation for a resource block (approx. 0x5BBE4 bytes with a custom 'RM' header), execution/processing phases, and cleanup upon exit.

Called after memory allocation and initialization, passing the processed resource pointer and computed arguments (derived from command-line hash state); its boolean return value dictates whether another function (sub_403E50) is called.

## Functions it calls or references

### sub_418760 [InitializePrngState] (0x00418760) - call at 0x00401786

- Prototype: `void __fastcall InitializePrngState(unsigned int seed, void *state_struct)`
- Summary: Initializes a state structure with a pseudo-random number generator (PRNG) state based on a seed, employing hash mixing constants and repeated mixing rounds.

### sub_4186E0 [InitializeSubManager] (0x004186E0) - call at 0x00401791

- Prototype: `void __fastcall InitializeSubManager(void *manager)`
- Summary: Initializes a sub-manager structure by allocating an 8000-byte block and setting up a linked list or table of 4096 (0x1000) entries with zeroed values and index links.

### memset (0x0041A150) - call at 0x004017E0, 0x00401865, 0x00401895, 0x0040192C

- Summary: Import thunk: jumps straight to the imported API MSVCR80!memset.

### sub_401F60 [InitializeSubsystemRecords] (0x00401F60) - call at 0x004017ED

- Prototype: `void __stdcall InitializeSubsystemRecords(void *resourceBlock, unsigned int stateFlags)`
- Summary: Initializes an array of structured configuration records within a large resource block using a loop, followed by setting up two additional specific subsystem records.

### sub_416640 [UpdateResourceStateBlockOrSimulationStep] (0x00416640) - call at 0x00401838

- Prototype: `__stdcall void UpdateResourceStateBlockOrSimulationStep(void *resourceBlock, void *dataBuffer, int count)`
- Summary: Performs a comprehensive simulation, procedural generation, spatial grid update, and entity/particle batch processing step using a large state structure.

### sub_418470 [AllocateMemoryPoolBlock] (0x00418470) - call at 0x00401A28

- Prototype: `bool __fastcall AllocateMemoryPoolBlock(int bucket_index)`
- Summary: Allocates a large 64KB memory chunk via malloc, initializes a linked list of fixed-size sub-blocks within it for a specific pool bucket, and registers the new chunk with global tracking structures.

### sub_410A90 [ResourceEntry_Initialize] (0x00410A90) - call at 0x00401A86

- Prototype: `void* __fastcall ResourceEntry_Initialize(void *this, void *edx, int Src)`
- Summary: Initializes a newly allocated resource or sub-system entry object passed in `ecx` using an identifier/lookup value (`Src`) passed in `eax`.

### sub_4185D0 [AllocateCustomBlock] (0x004185D0) - call at 0x00401B42, 0x00401B78, 0x00401E31, 0x00401E67

- Prototype: `void* __fastcall AllocateCustomBlock(int size)`
- Summary: Allocates a custom memory block of the requested size, utilizing a fixed-size memory pool allocator for small/medium allocations or falling back to malloc for large ones, and returns an inner payload pointer with a tracked header.

### memcpy (0x0041A22A) - call at 0x00401B99, 0x00401E88

- Summary: Import thunk: jumps straight to the imported API MSVCR80!memcpy.

### sub_402050 [SpawnResourceOrEntityBatch] (0x00402050) - call at 0x00401C42

- Prototype: `void __stdcall SpawnResourceOrEntityBatch(void *resourceBlock, int resourceIndex)`
- Summary: Processes a specific resource or state entry by calculating spatial metrics, normalizing vectors, validating grid positions, and spawning/registering entity instances via factory routines.

### __ftol2_sse (0x0041A160) - call at 0x00401CA3, 0x00401CF2

- Prototype: `int __cdecl __ftol2_sse(void)`
- Summary: Converts the top-of-stack x87 floating-point value to a 32-bit signed integer using SSE or fallback FPU routines, returning the integer result in EAX.

### sub_414CE0 [CheckGridAreaPassesCriteria] (0x00414CE0) - call at 0x00401D31

- Prototype: `bool __fastcall CheckGridAction(void *pDataStruct, int edx_val, int ecx_val, int arg0)`
- Summary: Tests whether a 2D rectangular grid region within a structured data array meets specific validity and value criteria, returning true if all checked elements satisfy the conditions.

### sub_402560 [CreateAndRegisterResourceEntity] (0x00402560) - call at 0x00401D64

- Prototype: `void* __fastcall CreateAndRegisterResourceEntity(void *resourceBlock, int unused_edx, int type_index, int sub_variant_flag, int flags)`
- Summary: Allocates, initializes, and configures a new resource or RTS object entity based on type lookup tables, coordinates, and flags, registering it into spatial grid and resource tracking containers.

### sub_407B50 [ProcessRtsResourceSpawningOrPathNodes] (0x00407B50) - call at 0x00401DA9

- Prototype: `void __fastcall ProcessRtsResourceSpawningOrPathNodes(void *resourceBlock)`
- Summary: Iterates through waypoint or resource spawn definitions, evaluates spatial metrics and procedural selection checks via geometric calculations and pseudo-random generators, and instantiates new resource entities using sub_402560.

### sub_402980 [EntityInteractionEventCallback] (0x00402980) - address taken at 0x00401DC0, 0x00401EE9

- Prototype: `__thiscall void EntityInteractionEventCallback(void *this, int arg1, int arg2)`
- Summary: Serves as a callback wrapper that validates an object context and state before forwarding an event to the entity interaction handler.

### sub_404570 [InitializeEntityRegistration] (0x00404570) - call at 0x00401F12

- Prototype: `void __fastcall InitializeEntityRegistration(void *esi_entity_struct, void *edx_unused, int arg_id)`
- Summary: Initializes a new entity/resource structure and registers it into a global tracking array or proximity-matching container.

### sub_4097E0 [InitializeStateBlock] (0x004097E0) - call at 0x00401F2E

- Prototype: `bool __thiscall InitializeStateBlock(void *this)`
- Summary: Initializes a state or configuration block object via `this` (passed in `ecx`), setting up vtable/type pointers, zeroing fields, and populating internal configuration tables from constant data.

### sub_40A0D0 [VM_ExecuteEntityEventScript] (0x0040A0D0) - call at 0x00401F44

- Prototype: `bool __fastcall VM_ExecuteEntityEventScript(void *vm_context, void *unused, int event_index)`
- Summary: Validates and executes a virtual machine bytecode block associated with a specific entity event, managing VM state backup and restoration around the execution.

### sub_4036B0 [UpdateSimulationGridRegions] (0x004036B0) - call at 0x00401F4C

- Prototype: `void __fastcall UpdateSimulationGridRegions(void *resourceBlock)`
- Summary: Iterates through sub-components and grid regions in the simulation resource structure, clearing flags and conditionally invoking grid region updates.

Reference kinds used above:
- *address taken*: the function's address is used as data there - e.g. passed as a callback or stored as a function pointer - rather than called directly.

## Imported APIs referenced

- MSVCR80!malloc
- MSVCR80!free

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00401750  push ebp
0x00401751  mov ebp, esp
0x00401753  and esp, 0FFFFFFF8h
0x00401756  sub esp, 44h
0x00401759  cmp eax, 2
0x0040175C  push ebx
0x0040175D  push esi
0x0040175E  push edi
0x0040175F  jge loc_401768
0x00401761  mov eax, 2
0x00401766  jmp loc_401772
0x00401768  cmp eax, 4
0x0040176B  jle loc_401772
0x0040176D  mov eax, 4
0x00401772  mov ebx, [ebp+arg_0]
0x00401775  mov [ebx+5BBBCh], eax
0x0040177B  mov eax, [ebp+arg_4]
0x0040177E  lea edi, [ebx+51EF8h]
0x00401784  mov ecx, edi
0x00401786  call sub_418760                              ; -> sub_418760 [InitializePrngState]
0x0040178B  lea esi, [ebx+51E80h]
0x00401791  call sub_4186E0                              ; -> sub_4186E0 [InitializeSubManager]
0x00401796  fldz
0x00401798  push 4800h
0x0040179D  fstp
0x004017A3  fld                                          ; -> 0x0041DC08 in .rdata
0x004017A9  xor esi, esi
0x004017AB  lea eax, [ebx+573BCh]
0x004017B1  fstp
0x004017B7  push 0FFh
0x004017BC  push eax
0x004017BD  mov [ebx+5BBC0h], esi
0x004017C3  mov [ebx+5BBC4h], esi
0x004017C9  mov byte ptr [ebx+5BBD0h], 0
0x004017D0  mov dword ptr [ebx+5BBD4h], 0FFFFFFFFh
0x004017DA  mov [ebx+5BBD8h], esi
0x004017E0  call memset                                  ; -> memset
0x004017E5  mov ecx, [ebp+arg_4]
0x004017E8  add esp, 0Ch
0x004017EB  push ecx
0x004017EC  push ebx
0x004017ED  call sub_401F60                              ; -> sub_401F60 [InitializeSubsystemRecords]
0x004017F2  mov edx, [ebx+5BBBCh]
0x004017F8  cmp edx, esi
0x004017FA  jle loc_401829
0x004017FC  mov esi, edx
0x004017FE  lea ecx, [esp+50h+var_20]
0x00401802  lea eax, [ebx+0ECh]
0x00401808  mov [esp+50h+var_38], esi
0x0040180C  lea esp, [esp+0]
0x00401810  mov esi, [eax]
0x00401812  mov [ecx], esi
0x00401814  mov esi, [eax+4]
0x00401817  mov [ecx+4], esi
0x0040181A  add eax, 108h
0x0040181F  add ecx, 8
0x00401822  sub [esp+50h+var_38], 1
0x00401827  jnz loc_401810
0x00401829  push edx
0x0040182A  lea edx, [esp+54h+var_20]
0x0040182E  push edx
0x0040182F  lea esi, [ebx+1A20h]
0x00401835  push edi
0x00401836  mov eax, esi
0x00401838  call sub_416640                              ; -> sub_416640 [UpdateResourceStateBlockOrSimulationStep]
0x0040183D  mov [ebx+1016Ch], esi
0x00401843  xor esi, esi
0x00401845  push 4800h
0x0040184A  lea eax, [ebx+2B170h]
0x00401850  push esi
0x00401851  push eax
0x00401852  mov [ebx+43D74h], si
0x00401859  mov [ebx+43D70h], esi
0x0040185F  mov [ebx+43D78h], esi
0x00401865  call memset                                  ; -> memset
0x0040186A  add esp, 0Ch
0x0040186D  xor eax, eax
0x0040186F  lea edi, [ebx+43D7Ch]
0x00401875  mov ecx, 240h
0x0040187A  stosd
0x0040187C  push 0D800h
0x00401881  lea ecx, [ebx+44680h]
0x00401887  lea eax, [ebx+1A20h]
0x0040188D  push esi
0x0040188E  push ecx
0x0040188F  mov [ebx+4467Ch], eax
0x00401895  call memset                                  ; -> memset
0x0040189A  lea eax, [ebx+54740h]
0x004018A0  fldz
0x004018A2  mov [ebx+51E94h], esi
0x004018A8  fstp
0x004018AE  mov dword ptr [ebx+51E9Ch], 1
0x004018B8  mov [eax+2000h], esi
0x004018BE  mov [eax+2008h], esi
0x004018C4  mov [eax+2014h], esi
0x004018CA  mov [eax+201Ch], esi
0x004018D0  mov [eax+2020h], esi
0x004018D6  mov [eax+2024h], esi
0x004018DC  add esp, 0Ch
0x004018DF  mov [ebx+51F00h], eax
0x004018E5  lea eax, [ebx+51F08h]
0x004018EB  mov ecx, 4
0x004018F0  or edx, 0FFFFFFFFh
0x004018F3  mov [eax-4], edx
0x004018F6  mov [eax], esi
0x004018F8  add eax, 808h
0x004018FD  sub ecx, 1
0x00401900  jnz loc_4018F3
0x00401902  push 800h
0x00401907  lea eax, [ebx+53F24h]
0x0040190D  push esi
0x0040190E  push eax
0x0040190F  mov [ebx+54724h], eax
0x00401915  mov dword ptr [ebx+54728h], 800h
0x0040191F  mov [ebx+5472Ch], esi
0x00401925  mov byte ptr [ebx+54730h], 0
0x0040192C  call memset                                  ; -> memset
0x00401931  add esp, 0Ch
0x00401934  mov [ebx+54734h], esi
0x0040193A  mov [ebx+54738h], esi
0x00401940  mov [ebx+5473Ch], esi
0x00401946  cmp [ebx+100DCh], esi
0x0040194C  mov [esp+50h+var_2C], esi
0x00401950  jle loc_401C35
0x00401956  lea edx, [ebx+0F524h]
0x0040195C  mov [esp+50h+var_38], edx
0x00401960  cmp byte_42285C, 0                           ; -> 0x0042285C in .data
0x00401967  mov eax, [esp+50h+var_38]
0x0040196B  movzx ecx, word ptr [eax]
0x0040196E  mov edi, 24h
0x00401973  mov [esp+50h+Src], ecx
0x00401977  jnz loc_4019D5
0x00401979  xor eax, eax
0x0040197B  mov dword_422820, eax                        ; -> 0x00422820 in .data
0x00401980  mov dword_422824, eax                        ; -> 0x00422824 in .data
0x00401985  mov dword_422828, eax                        ; -> 0x00422828 in .data
0x0040198A  mov dword_42282C, eax                        ; -> 0x0042282C in .data
0x0040198F  mov dword_422830, eax                        ; -> 0x00422830 in .data
0x00401994  mov dword_422834, eax                        ; -> 0x00422834 in .data
0x00401999  mov dword_422838, eax                        ; -> 0x00422838 in .data
0x0040199E  mov dword_42283C, eax                        ; -> 0x0042283C in .data
0x004019A3  mov dword_422840, eax                        ; -> 0x00422840 in .data
0x004019A8  mov dword_422844, eax                        ; -> 0x00422844 in .data
0x004019AD  mov dword_422848, eax                        ; -> 0x00422848 in .data
0x004019B2  mov dword_42284C, eax                        ; -> 0x0042284C in .data
0x004019B7  mov dword_422850, eax                        ; -> 0x00422850 in .data
0x004019BC  mov Block, esi                               ; -> 0x00422858 in .data
0x004019C2  mov dword_42281C, esi                        ; -> 0x0042281C in .data
0x004019C8  mov dword_422854, esi                        ; -> 0x00422854 in .data
0x004019CE  mov byte_42285C, 1                           ; -> 0x0042285C in .data
0x004019D5  xor ecx, ecx
0x004019D7  mov [esp+50h+var_40], ecx
0x004019DB  mov edx, 0Ch
0x004019E0  lea eax, [edx+ecx]
0x004019E3  sar eax, 1
0x004019E5  cmp ds:word_41B150[eax*2], di                ; -> 0x0041B150 in .rdata
0x004019ED  jnb loc_4019FA
0x004019EF  add eax, 1
0x004019F2  mov [esp+50h+var_40], eax
0x004019F6  mov ecx, eax
0x004019F8  jmp loc_4019FC
0x004019FA  mov edx, eax
0x004019FC  cmp ecx, edx
0x004019FE  jl loc_4019E0
0x00401A00  cmp ecx, esi
0x00401A02  jge loc_401A1D
0x00401A04  push 2Ch
0x00401A06  call ds:malloc                               ; -> import MSVCR80!malloc
0x00401A0C  add esp, 4
0x00401A0F  cmp eax, esi
0x00401A11  jz loc_401A93
0x00401A17  mov byte ptr [eax+2], 0FFh
0x00401A1B  jmp loc_401A48
0x00401A1D  cmp dword_422820[ecx*4], esi                 ; -> 0x00422820 in .data
0x00401A24  jnz loc_401A35
0x00401A26  mov esi, ecx
0x00401A28  call sub_418470                              ; -> sub_418470 [AllocateMemoryPoolBlock]
0x00401A2D  test al, al
0x00401A2F  jz loc_401A93
0x00401A31  mov ecx, [esp+50h+var_40]
0x00401A35  mov eax, dword_422820[ecx*4]                 ; -> 0x00422820 in .data
0x00401A3C  mov edx, [eax]
0x00401A3E  mov dword_422820[ecx*4], edx                 ; -> 0x00422820 in .data
0x00401A45  mov [eax+2], cl
0x00401A48  mov word ptr [eax], 4D52h
0x00401A4D  mov byte ptr [eax+3], 0
0x00401A51  mov ecx, 24h
0x00401A56  add dword_42281C, ecx                        ; -> 0x0042281C in .data
0x00401A5C  mov [eax+4], ecx
0x00401A5F  lea ecx, [eax+8]
0x00401A62  xor eax, eax
0x00401A64  test ecx, ecx
0x00401A66  mov [ecx], eax
0x00401A68  mov [ecx+4], eax
0x00401A6B  mov [ecx+8], eax
0x00401A6E  mov [ecx+0Ch], eax
0x00401A71  mov [ecx+10h], eax
0x00401A74  mov [ecx+14h], eax
0x00401A77  mov [ecx+18h], eax
0x00401A7A  mov [ecx+1Ch], eax
0x00401A7D  mov [ecx+20h], eax
0x00401A80  jz loc_401A93
0x00401A82  mov eax, [esp+50h+Src]
0x00401A86  call sub_410A90                              ; -> sub_410A90 [ResourceEntry_Initialize]
0x00401A8B  mov ecx, eax
0x00401A8D  mov [esp+50h+var_3C], ecx
0x00401A91  jmp loc_401A9F
0x00401A93  mov [esp+50h+var_3C], 0
0x00401A9B  mov ecx, [esp+50h+var_3C]
0x00401A9F  mov edx, [ebx+51E88h]
0x00401AA5  cmp edx, [ebx+51E84h]
0x00401AAB  jge loc_401C27
0x00401AB1  mov eax, [ebx+51E80h]
0x00401AB7  movzx esi, word ptr [eax+edx*8+6]
0x00401ABC  lea eax, [eax+edx*8]
0x00401ABF  mov [ebx+51E88h], esi
0x00401AC5  mov [eax], ecx
0x00401AC7  add dword ptr [ebx+51E8Ch], 1
0x00401ACE  movzx eax, word ptr [eax+4]
0x00401AD2  shl eax, 10h
0x00401AD5  add edx, 1
0x00401AD8  or eax, edx
0x00401ADA  mov [ecx+4], eax
0x00401ADD  jz loc_401C27
0x00401AE3  mov dx, [ebx+573B4h]
0x00401AEA  mov eax, [esp+50h+Src]
0x00401AEE  mov [ebx+eax*2+573BCh], dx
0x00401AF6  mov edx, [ebx+573B4h]
0x00401AFC  mov eax, [ebx+573B8h]
0x00401B02  cmp edx, eax
0x00401B04  jnz loc_401BF2
0x00401B0A  test eax, eax
0x00401B0C  lea ecx, [edx+1]
0x00401B0F  lea esi, [eax+eax]
0x00401B12  jnz loc_401B19
0x00401B14  mov esi, 10h
0x00401B19  cmp esi, ecx
0x00401B1B  mov [esp+50h+var_40], esi
0x00401B1F  jge loc_401B2B
0x00401B21  add esi, esi
0x00401B23  cmp esi, ecx
0x00401B25  jl loc_401B21
0x00401B27  mov [esp+50h+var_40], esi
0x00401B2B  mov eax, [ebx+573B0h]
0x00401B31  test eax, eax
0x00401B33  lea edi, ds:0[esi*4]
0x00401B3A  mov [esp+50h+Src], eax
0x00401B3E  jnz loc_401B4C
0x00401B40  mov eax, edi
0x00401B42  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00401B47  jmp loc_401BE2
0x00401B4C  mov cl, [eax-6]
0x00401B4F  cmp cl, 0FFh
0x00401B52  lea esi, [eax-8]
0x00401B55  jz loc_401B76
0x00401B57  movzx ecx, cl
0x00401B5A  movzx edx, ds:word_41B150[ecx*2]             ; -> 0x0041B150 in .rdata
0x00401B62  cmp edi, edx
0x00401B64  ja loc_401B76
0x00401B66  mov ecx, edi
0x00401B68  sub ecx, [esi+4]
0x00401B6B  mov [esi+4], edi
0x00401B6E  add dword_42281C, ecx                        ; -> 0x0042281C in .data
0x00401B74  jmp loc_401BDE
0x00401B76  mov eax, edi
0x00401B78  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00401B7D  test eax, eax
0x00401B7F  mov [esp+50h+var_24], eax
0x00401B83  jz loc_401BDE
0x00401B85  mov eax, [esi+4]
0x00401B88  cmp eax, edi
0x00401B8A  jnb loc_401B8E
0x00401B8C  mov edi, eax
0x00401B8E  mov edx, [esp+50h+Src]
0x00401B92  push edi
0x00401B93  mov edi, [esp+54h+var_24]
0x00401B97  push edx
0x00401B98  push edi
0x00401B99  call memcpy                                  ; -> memcpy
0x00401B9E  add esp, 0Ch
0x00401BA1  cmp word ptr [esi], 4D52h
0x00401BA6  jnz loc_401BDC
0x00401BA8  mov eax, [esi+4]
0x00401BAB  sub dword_42281C, eax                        ; -> 0x0042281C in .data
0x00401BB1  mov al, [esi+2]
0x00401BB4  cmp al, 0FFh
0x00401BB6  mov word ptr [esi], 0
0x00401BBB  jnz loc_401BC9
0x00401BBD  push esi
0x00401BBE  call ds:free                                 ; -> import MSVCR80!free
0x00401BC4  add esp, 4
0x00401BC7  jmp loc_401BDC
0x00401BC9  movzx eax, al
0x00401BCC  mov ecx, dword_422820[eax*4]                 ; -> 0x00422820 in .data
0x00401BD3  mov [esi], ecx
0x00401BD5  mov dword_422820[eax*4], esi                 ; -> 0x00422820 in .data
0x00401BDC  mov eax, edi
0x00401BDE  mov esi, [esp+50h+var_40]
0x00401BE2  mov ecx, [esp+50h+var_3C]
0x00401BE6  mov [ebx+573B0h], eax
0x00401BEC  mov [ebx+573B8h], esi
0x00401BF2  mov edx, [ebx+573B4h]
0x00401BF8  mov eax, [ebx+573B0h]
0x00401BFE  add [esp+50h+var_38], 2
0x00401C03  mov [eax+edx*4], ecx
0x00401C06  mov eax, [esp+50h+var_2C]
0x00401C0A  add dword ptr [ebx+573B4h], 1
0x00401C11  add eax, 1
0x00401C14  cmp eax, [ebx+100DCh]
0x00401C1A  mov [esp+50h+var_2C], eax
0x00401C1E  jge loc_401C33
0x00401C20  xor esi, esi
0x00401C22  jmp loc_401960
0x00401C27  test ecx, ecx
0x00401C29  jz loc_401C33
0x00401C2B  mov edx, [ecx]
0x00401C2D  mov eax, [edx]
0x00401C2F  push 1
0x00401C31  call eax
0x00401C33  xor esi, esi
0x00401C35  xor edi, edi
0x00401C37  cmp [ebx+5BBBCh], esi
0x00401C3D  jle loc_401C52
0x00401C3F  nop
0x00401C40  push edi
0x00401C41  push ebx
0x00401C42  call sub_402050                              ; -> sub_402050 [SpawnResourceOrEntityBatch]
0x00401C47  add edi, 1
0x00401C4A  cmp edi, [ebx+5BBBCh]
0x00401C50  jl loc_401C40
0x00401C52  xor edi, edi
0x00401C54  cmp [ebx+10100h], esi
0x00401C5A  jle loc_401D91
0x00401C60  lea ecx, [ebx+100E0h]
0x00401C66  mov [esp+50h+var_40], ecx
0x00401C6A  lea ebx, [ebx+0]
0x00401C70  mov eax, [esp+50h+var_40]
0x00401C74  fld                                          ; -> 0x0041D960 in .rdata
0x00401C7A  mov edx, [eax]
0x00401C7C  fld                                          ; -> 0x0041D8C0 in .rdata
0x00401C82  fld                                          ; -> 0x0041D9F0 in .rdata
0x00401C88  mov eax, [eax+4]
0x00401C8B  mov [esp+50h+var_2C], edx
0x00401C8F  fld
0x00401C93  fmul st, st(3)
0x00401C95  mov [esp+50h+var_28], eax
0x00401C99  fstp
0x00401C9D  fld
0x00401CA1  fld
0x00401CA3  call __ftol2_sse                             ; -> __ftol2_sse
0x00401CA8  mov ecx, eax
0x00401CAA  mov [esp+50h+var_24], ecx
0x00401CAE  fild
0x00401CB2  fcompp
0x00401CB4  fnstsw ax
0x00401CB6  test ah, 41h
0x00401CB9  jnz loc_401CBE
0x00401CBB  sub ecx, 1
0x00401CBE  test ecx, ecx
0x00401CC0  jge loc_401CCA
0x00401CC2  xor esi, esi
0x00401CC4  mov [esp+50h+var_38], esi
0x00401CC8  jmp loc_401CE0
0x00401CCA  cmp ecx, 5Fh
0x00401CCD  jle loc_401CDA
0x00401CCF  mov esi, 5Fh
0x00401CD4  mov [esp+50h+var_38], esi
0x00401CD8  jmp loc_401CE0
0x00401CDA  mov esi, ecx
0x00401CDC  mov [esp+50h+var_38], ecx
0x00401CE0  fld
0x00401CE4  fmulp st(3), st
0x00401CE6  fxch
0x00401CE8  fstp
0x00401CEC  fld
0x00401CF0  fld
0x00401CF2  call __ftol2_sse                             ; -> __ftol2_sse
0x00401CF7  mov ecx, eax
0x00401CF9  mov [esp+50h+var_24], ecx
0x00401CFD  fild
0x00401D01  fcompp
0x00401D03  fnstsw ax
0x00401D05  test ah, 41h
0x00401D08  jnz loc_401D0D
0x00401D0A  sub ecx, 1
0x00401D0D  test ecx, ecx
0x00401D0F  jge loc_401D15
0x00401D11  xor ecx, ecx
0x00401D13  jmp loc_401D1F
0x00401D15  cmp ecx, 5Fh
0x00401D18  jle loc_401D1F
0x00401D1A  mov ecx, 5Fh
0x00401D1F  lea eax, [ebx+1A20h]
0x00401D25  push eax
0x00401D26  mov eax, 3
0x00401D2B  mov edx, esi
0x00401D2D  mov [esp+54h+var_3C], ecx
0x00401D31  call sub_414CE0                              ; -> sub_414CE0 [CheckGridAreaPassesCriteria]
0x00401D36  test al, al
0x00401D38  jz loc_401D79
0x00401D3A  fild
0x00401D3E  push 0
0x00401D40  push ecx
0x00401D41  lea ecx, [esp+58h+Src]
0x00401D45  fmul st, st(1)
0x00401D47  mov edx, 27h
0x00401D4C  fadd st, st(2)
0x00401D4E  fstp
0x00401D52  fimul
0x00401D56  faddp st(1), st
0x00401D58  fstp
0x00401D5C  fldz
0x00401D5E  fstp
0x00401D61  push 5
0x00401D63  push ebx
0x00401D64  call sub_402560                              ; -> sub_402560 [CreateAndRegisterResourceEntity]
0x00401D69  test eax, eax
0x00401D6B  jz loc_401D7D
0x00401D6D  mov dword ptr [eax+2B8h], 1F40h
0x00401D77  jmp loc_401D7D
0x00401D79  fstp
0x00401D7B  fstp
0x00401D7D  add [esp+50h+var_40], 8
0x00401D82  add edi, 1
0x00401D85  cmp edi, [ebx+10100h]
0x00401D8B  jl loc_401C70
0x00401D91  lea eax, [ebx+571D4h]
0x00401D97  push eax
0x00401D98  mov dword ptr [eax+1B0h], 0
0x00401DA2  mov byte ptr [eax+1B4h], 0
0x00401DA9  call sub_407B50                              ; -> sub_407B50 [ProcessRtsResourceSpawningOrPathNodes]
0x00401DAE  mov edx, [ebx+51EB4h]
0x00401DB4  xor eax, eax
0x00401DB6  test edx, edx
0x00401DB8  jle loc_401DE1
0x00401DBA  mov ecx, [ebx+51EB0h]
0x00401DC0  cmp dword ptr [ecx], offset sub_402980       ; -> sub_402980 [EntityInteractionEventCallback]
0x00401DC6  jnz loc_401DCD
0x00401DC8  cmp [ecx+4], ebx
0x00401DCB  jz loc_401DD9
0x00401DCD  add eax, 1
0x00401DD0  add ecx, 8
0x00401DD3  cmp eax, edx
0x00401DD5  jl loc_401DC0
0x00401DD7  jmp loc_401DE1
0x00401DD9  test eax, eax
0x00401DDB  jge loc_401EFC
0x00401DE1  mov ecx, [ebx+51EB4h]
0x00401DE7  mov eax, [ebx+51EB8h]
0x00401DED  cmp ecx, eax
0x00401DEF  jnz loc_401EDD
0x00401DF5  add ecx, 1
0x00401DF8  test eax, eax
0x00401DFA  lea esi, [eax+eax]
0x00401DFD  jnz loc_401E04
0x00401DFF  mov esi, 10h
0x00401E04  cmp esi, ecx
0x00401E06  mov [esp+50h+var_40], esi
0x00401E0A  jge loc_401E1A
0x00401E0C  lea esp, [esp+0]
0x00401E10  add esi, esi
0x00401E12  cmp esi, ecx
0x00401E14  jl loc_401E10
0x00401E16  mov [esp+50h+var_40], esi
0x00401E1A  mov eax, [ebx+51EB0h]
0x00401E20  test eax, eax
0x00401E22  lea edi, ds:0[esi*8]
0x00401E29  mov [esp+50h+var_2C], eax
0x00401E2D  jnz loc_401E3B
0x00401E2F  mov eax, edi
0x00401E31  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00401E36  jmp loc_401ED1
0x00401E3B  mov cl, [eax-6]
0x00401E3E  cmp cl, 0FFh
0x00401E41  lea esi, [eax-8]
0x00401E44  jz loc_401E65
0x00401E46  movzx ecx, cl
0x00401E49  movzx edx, ds:word_41B150[ecx*2]             ; -> 0x0041B150 in .rdata
0x00401E51  cmp edi, edx
0x00401E53  ja loc_401E65
0x00401E55  mov ecx, edi
0x00401E57  sub ecx, [esi+4]
0x00401E5A  mov [esi+4], edi
0x00401E5D  add dword_42281C, ecx                        ; -> 0x0042281C in .data
0x00401E63  jmp loc_401ECD
0x00401E65  mov eax, edi
0x00401E67  call sub_4185D0                              ; -> sub_4185D0 [AllocateCustomBlock]
0x00401E6C  test eax, eax
0x00401E6E  mov [esp+50h+var_24], eax
0x00401E72  jz loc_401ECD
0x00401E74  mov eax, [esi+4]
0x00401E77  cmp eax, edi
0x00401E79  jnb loc_401E7D
0x00401E7B  mov edi, eax
0x00401E7D  mov edx, [esp+50h+var_2C]
0x00401E81  push edi
0x00401E82  mov edi, [esp+54h+var_24]
0x00401E86  push edx
0x00401E87  push edi
0x00401E88  call memcpy                                  ; -> memcpy
0x00401E8D  add esp, 0Ch
0x00401E90  cmp word ptr [esi], 4D52h
0x00401E95  jnz loc_401ECB
0x00401E97  mov eax, [esi+4]
0x00401E9A  sub dword_42281C, eax                        ; -> 0x0042281C in .data
0x00401EA0  mov al, [esi+2]
0x00401EA3  cmp al, 0FFh
0x00401EA5  mov word ptr [esi], 0
0x00401EAA  jnz loc_401EB8
0x00401EAC  push esi
0x00401EAD  call ds:free                                 ; -> import MSVCR80!free
0x00401EB3  add esp, 4
0x00401EB6  jmp loc_401ECB
0x00401EB8  movzx eax, al
0x00401EBB  mov ecx, dword_422820[eax*4]                 ; -> 0x00422820 in .data
0x00401EC2  mov [esi], ecx
0x00401EC4  mov dword_422820[eax*4], esi                 ; -> 0x00422820 in .data
0x00401ECB  mov eax, edi
0x00401ECD  mov esi, [esp+50h+var_40]
0x00401ED1  mov [ebx+51EB0h], eax
0x00401ED7  mov [ebx+51EB8h], esi
0x00401EDD  mov ecx, [ebx+51EB4h]
0x00401EE3  mov eax, [ebx+51EB0h]
0x00401EE9  mov edx, offset sub_402980                   ; -> sub_402980 [EntityInteractionEventCallback]
0x00401EEE  mov [eax+ecx*8], edx
0x00401EF1  mov [eax+ecx*8+4], ebx
0x00401EF5  add dword ptr [ebx+51EB4h], 1
0x00401EFC  xor edi, edi
0x00401EFE  cmp [ebx+5BBBCh], edi
0x00401F04  jle loc_401F28
0x00401F06  lea esi, [ebx+630h]
0x00401F0C  lea esp, [esp+0]
0x00401F10  mov eax, edi
0x00401F12  call sub_404570                              ; -> sub_404570 [InitializeEntityRegistration]
0x00401F17  add edi, 1
0x00401F1A  add esi, 4FCh
0x00401F20  cmp edi, [ebx+5BBBCh]
0x00401F26  jl loc_401F10
0x00401F28  lea ecx, [ebx+56768h]
0x00401F2E  call sub_4097E0                              ; -> sub_4097E0 [InitializeStateBlock]
0x00401F33  test al, al
0x00401F35  jnz loc_401F40
0x00401F37  pop edi
0x00401F38  pop esi
0x00401F39  pop ebx
0x00401F3A  mov esp, ebp
0x00401F3C  pop ebp
0x00401F3D  retn 8
0x00401F40  xor ebx, ebx
0x00401F42  mov esi, ecx
0x00401F44  call sub_40A0D0                              ; -> sub_40A0D0 [VM_ExecuteEntityEventScript]
0x00401F49  mov esi, [ebp+arg_0]
0x00401F4C  call sub_4036B0                              ; -> sub_4036B0 [UpdateSimulationGridRegions]
0x00401F51  pop edi
0x00401F52  pop esi
0x00401F53  mov al, 1
0x00401F55  pop ebx
0x00401F56  mov esp, ebp
0x00401F58  pop ebp
0x00401F59  retn 8
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
