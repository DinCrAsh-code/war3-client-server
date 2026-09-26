# Function summary: sub_409940 (0x00409940)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x00409940
- Name: sub_409940 (auto-generated placeholder)
- Size: 1729 bytes, 436 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_40A0D0 -> sub_409940

## How callers use this function

### From sub_40A0D0 (0x0040A0D0) - call at 0x0040A198

What that caller does overall (preliminary): Validates, indexes, and appends a batch of items or sub-buffers from the main state/resource structure (ESI) based on an index parameter (EBX), updating internal tracking counters and invoking a recursive/subsystem routine (sub_409940) to process them.

Called after gathering and staging items from the resource state structure (ESI) into internal buffers. It receives the current index (EBX) and a saved count value (on the stack), with 'this' pointer in ESI via __thiscall convention. Its return value in EAX (and internal flag checks) determines whether this function reports success (al = 1) or failure (al = 0).

## Recursion

- It refers to itself directly (recursion): call at 0x00409E3B.
- It is part of a mutually recursive cycle - these functions all (directly or indirectly) call each other: sub_40A0D0 (0x0040A0D0), sub_40AD00 [ScheduleTimedEventOrState] (0x0040AD00), sub_419280 [EnqueueStateEntry] (0x00419280), sub_40A1E0 [DispatchEntityEvent] (0x0040A1E0).

## Functions it calls or references

### sub_4098B0 [VM_PushValue] (0x004098B0) - call at 0x00409A8B, 0x00409ADC, 0x00409AF9, 0x00409B37, 0x00409BB7, 0x00409C01, 0x00409C2E, 0x00409C89 (+9 more)

- Prototype: `bool __fastcall VM_PushValue(VMContext *vm, const Value64 *valPtr)`
- Summary: Pushes a 64-bit value onto the virtual machine's internal stack, checking for overflow and setting an error flag if the stack limit (256 entries) is exceeded.

### sub_4098F0 [VM_PopOperand] (0x004098F0) - call at 0x00409B16, 0x00409B4A, 0x00409B81, 0x00409B8C, 0x00409BC7, 0x00409C3E, 0x00409C49, 0x00409C99 (+11 more)

- Prototype: `bool __fastcall VM_PopOperand(VMContext *vm, Variant *outValue)`
- Summary: Pops an 8-byte value (variant/operand) from the virtual machine's internal stack structure, returning false if the stack is underflowed or true on success.

### sub_4094B0 [ExecuteBinaryOp] (0x004094B0) - call at 0x00409B9B

- Prototype: `int __fastcall ExecuteBinaryOp(void *operandA, void *operandB, int opCode)`
- Summary: Performs binary arithmetic operations (add, subtract, multiply, divide, modulo) on two script/bytecode variable operands, handling both integer and floating-point types.

### sub_409680 [CompareVariantValues] (0x00409680) - call at 0x00409C60

- Prototype: `bool __fastcall CompareVariantValues(const Variant *left, const Variant *right, int opCode)`
- Summary: Compares two variant/operand buffers (left in ECX, right in EAX) using a relational operator specified by EDX, supporting both integer and floating-point comparisons and returning a boolean result in AL.

### sub_409480 [CheckOperandTruthState] (0x00409480) - call at 0x00409CAB, 0x00409CB8, 0x00409D18, 0x00409D25, 0x00409D75, 0x00409DC5

- Prototype: `bool __cdecl CheckOperandTruthState(const void *operand)`
- Summary: Evaluates an operand structure's truth state or validity flag, returning true or false based on its type and value.

### sub_40A2A0 [OpcodeHandler_LoadConstantOrVariable] (0x0040A2A0) - call at 0x00409ECA; vtable entry: slot 0 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl sub_40A2A0(void *this_ptr, void *unknown1, void *unknown2, int *out_result)`
- Summary: Executes a virtual machine instruction handler that retrieves a value (possibly a constant or variable) and writes an 8-byte typed value structure to the output destination.

### sub_40A2D0 [OpcodeRandomRange] (0x0040A2D0) - call at 0x00409ECA; vtable entry: slot 1 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl OpcodeRandomRange(void *this_ptr, struct RangeArg *arg, int unused, int *out_status)`
- Summary: Executes a bytecode range/random instruction by extracting low and high bounds (converting floats to ints if needed), generating a random integer within that range, and storing the output result.

### sub_40A380 [ExecuteRandomRangeOp] (0x0040A380) - call at 0x00409ECA; vtable entry: slot 2 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `__thiscall void ExecuteRandomRangeOp(void *this, int *pOperandStruct, int count, int *pOutputStatus)`
- Summary: Virtual opcode handler that evaluates operands (possibly as integer bounds or floating-point values), queries a pseudo-random number generator, and writes out a computed result.

### sub_40A3F0 [ExecuteSpawnEntityScriptOpcode] (0x0040A3F0) - call at 0x00409ECA; vtable entry: slot 3 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __stdcall ExecuteSpawnEntityScriptOpcode(void *this_ptr, void *script_args, int unused, void *out_status)`
- Summary: Executes a bytecode script instruction that spawns and registers a game entity based on coordinates, type, and variant parameters extracted from the script operand block.

### sub_40A540 [ResolveScriptReference] (0x0040A540) - call at 0x00409ECA; vtable entry: slot 4 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl ResolveScriptReference(void *this_ptr, const void *operand_struct)`
- Summary: Validates and dereferences a script/bytecode resource identifier, performing bounds checking against a global table and invoking a virtual method on the resolved target object.

### sub_40A5A0 [VM_GetGameVariableFloat] (0x0040A5A0) - call at 0x00409ECA; vtable entry: slot 5 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `float __cdecl VM_GetGameVariableFloat(void *operand, void *unused, struct OutVariant *outVariant)`
- Summary: Resolves a bytecode variable operand to a floating-point value from a global game state table, returning 0.0f if the reference is invalid.

### sub_40A620 [ExecuteScriptOpcode_TypeCheckOrRangeOp] (0x0040A620) - call at 0x00409ECA; vtable entry: slot 6 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl sub_40A620(void *this_ptr, const void *operand_struct)`
- Summary: Executes a specialized bytecode operation or validation handler from a script dispatch vtable, validating an operand reference and performing floating-point range or comparison logic.

### sub_40A6E0 [BytecodeOpcodeHandler_Slot7] (0x0040A6E0) - call at 0x00409ECA; vtable entry: slot 7 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __stdcall BytecodeOpcodeHandler_Slot7(void *this_ptr, void *operand, int count, void *status_output)`
- Summary: Executes a specific virtual bytecode instruction handler that dereferences an operand structure, checks its validity, and populates a status/result output structure with a type code and a secondary value.

### sub_40A720 [BytecodeOpcode_Slot8_EvalOrDefault] (0x0040A720) - call at 0x00409ECA; vtable entry: slot 8 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl sub_40A720(void *this_ptr, void *arg1, void *arg2, int *out_status)`
- Summary: Evaluates a nested script/bytecode operand structure or retrieves a default value, writing a type tag and a value pointer into an output status structure.

### sub_40A760 [VM_ResolveResourceHandle] (0x0040A760) - call at 0x00409ECA; vtable entry: slot 9 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl VM_ResolveResourceHandle(void *this_ptr, const void *operand, int count, int *out_status)`
- Summary: Validates and resolves a resource/object handle from bytecode operand data, returning a status code and index/handle value.

### sub_40A7E0 [ExecuteBytecodeOp_10] (0x0040A7E0) - call at 0x00409ECA; vtable entry: slot 10 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __fastcall ExecuteBytecodeOp_10(void *this, void *edx, void *arg1)`
- Summary: Executes virtual opcode slot 10, performing type checks/conversions on operands (using __ftol2_sse) and updating state variables stored in a global data structure.

### sub_40A830 [VM_OpcodeHandler_Slot11] (0x0040A830) - call at 0x00409ECA; vtable entry: slot 11 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __fastcall VM_OpcodeHandler_Slot11(void *pThis, void *edx_unused, void *arg_struct)`
- Summary: Executes a virtual machine bytecode instruction handler that processes typed operands (converting floating-point to integer if necessary) and updates a global state array based on an opcode index.

### sub_40A880 [ExecuteBytecodeTypeOp] (0x0040A880) - call at 0x00409ECA; vtable entry: slot 12 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __fastcall sub_40A880(void *this_ptr, int dummy, const void *operand_ptr, int count, int *output_struct)`
- Summary: Executes a virtual machine bytecode opcode handler that checks a type-tagged operand, extracts or converts its value, performs a lookup in a global table, and writes a status/result pair to an output structure.

### sub_40A8E0 [ExecuteBytecodeOpcode13] (0x0040A8E0) - call at 0x00409ECA; vtable entry: slot 13 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl ExecuteBytecodeOpcode13(void *pThis, int a2, void *pArgs, int *pStatus)`
- Summary: Executes a specific bytecode instruction handler (vtable slot 13) that validates a resource ID and type, writing status and resulting data values back to the output structure.

### sub_40A960 [EvalScriptReference] (0x0040A960) - call at 0x00409ECA; vtable entry: slot 14 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl EvalScriptReference(const void *arg0, int arg4, int arg8, void *outResult)`
- Summary: Evaluates a script or bytecode reference operand (checking type tag 3, index bounds, and validity) to produce a floating-point value written to an output buffer.

### sub_40A9E0 [ScriptOp_GetResourceValue] (0x0040A9E0) - call at 0x00409ECA; vtable entry: slot 15 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl ScriptOp_GetResourceValue(void *pThis, void *pArg1, int arg2, void *pResultStruct)`
- Summary: Evaluates a resource/object identifier operand from bytecode, bounds-checking it against a global resource manager table, and returns a floating-point or typed property value into a result structure.

### sub_40AA60 [ExecuteEntityInteractionOrCommandOpcode] (0x0040AA60) - call at 0x00409ECA; vtable entry: slot 16 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __fastcall ExecuteEntityInteractionOrCommandOpcode(void *this_ptr, void *unused, const void *operand_struct, void *status_output)`
- Summary: Executes a specialized entity interaction or command bytecode instruction, validating an entity reference and marshaling arguments into a parameter structure before invoking ProcessEntityInteractionOrCommand.

### sub_40AB80 [ExecuteBytecodeVtableSlot17] (0x0040AB80) - call at 0x00409ECA; vtable entry: slot 17 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `int __cdecl ExecuteBytecodeVtableSlot17(void *this_ptr, int arg1, int arg2, int *out_status)`
- Summary: Executes a virtual opcode handler that evaluates a type-checked operand (converting floats to integers or reading integer fields), computes a table lookup index, and stores a status flag and result.

### sub_40ABE0 [HandleBytecodeStatusOrEvent] (0x0040ABE0) - call at 0x00409ECA; vtable entry: slot 18 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl HandleBytecodeStatusOrEvent(int a1, const void *operand)`
- Summary: Extracts an integer value or type code from a bytecode operand structure and uses it as an index to trigger event handlers if it falls within a valid range (0-5).

### sub_40AC10 [ExecuteOpcode19] (0x0040AC10) - call at 0x00409ECA; vtable entry: slot 19 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `__thiscall void ExecuteOpcode19(void *this, int *arg1, int arg2, int *outStatus)`
- Summary: Implements a bytecode virtual machine instruction handler (vtable slot 19) that checks type and value flags against a global constraint table.

### sub_40AC70 [ExecuteScriptMathOrConversionOp] (0x0040AC70) - call at 0x00409ECA; vtable entry: slot 20 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl ExecuteScriptMathOrConversionOp(void *thisPtr, void *scriptVarInput, int unused, void *scriptVarOutput)`
- Summary: Executes a virtual script opcode that performs math, type conversion, or variable creation on input script values, returning a newly allocated script variable result.

### sub_40AD00 [ScheduleTimedEventOrState] (0x0040AD00) - call at 0x00409ECA; vtable entry: slot 21 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl ScheduleTimedEventOrState(void *this_ptr, const void *param1, int param2, void *status_out)`
- Summary: Evaluates a timed/parameterized script value, checks against scheduling limits, and enqueues a new state or event entry via EnqueueStateEntry.

### sub_40ADF0 [VM_OpcodeHandler_Slot22] (0x0040ADF0) - call at 0x00409ECA; vtable entry: slot 22 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __fastcall VM_OpcodeHandler_Slot22(void *pVMState, void *pArg1, int count, void *pResultStruct)`
- Summary: Executes a virtual machine opcode handler (vtable slot 22) that reads an operand, converts or extracts an integer value, checks bounds against an array limit, and appends or registers an item into a VM state queue or buffer.

### sub_40AE80 [ExecuteScriptOpcode_Slot23] (0x0040AE80) - call at 0x00409ECA; vtable entry: slot 23 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl ExecuteScriptOpcode_Slot23(void *pThis, const void *pOperand1, int count, void *pOutput)`
- Summary: Executes a virtual opcode handler (vtable slot 23) in the bytecode interpreter, fetching and converting operand data based on type flags to perform a math or conversion operation returning a floating-point/variant value.

### sub_40AEF0 [ExecuteVmOpcodeSlot24] (0x0040AEF0) - call at 0x00409ECA; vtable entry: slot 24 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __stdcall ExecuteVmOpcodeSlot24(void *this_ptr, const void *operandPtr, int count, void *outputStruct)`
- Summary: Executes a virtual machine opcode handler (vtable slot 24) that processes operand types, converts values using floating-point operations or type checks, and stores a formatted result into an output structure.

### sub_40AF60 [BytecodeOpcode_HandleReferenceOrResource] (0x0040AF60) - call at 0x00409ECA; vtable entry: slot 25 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `int __cdecl BytecodeOpcode_HandleReferenceOrResource(void *this_ptr, void *operand, int count, void *output_result)`
- Summary: Resolves a packed resource/reference identifier from a bytecode operand, validates it against a global manager table and object state, invokes a virtual method on the target object, and writes a success/failure status and result value to an output structure.

### sub_40AFE0 [VerifyObjectReference] (0x0040AFE0) - call at 0x00409ECA; vtable entry: slot 26 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `__cdecl int VerifyObjectReference(void *this_ptr, void *arg1, int arg2, int *out_status)`
- Summary: Validates a structured object/resource reference from bytecode operands against a global registry table and returns a status flag.

### sub_40B060 [VirtualOpcodeHandler_Slot27] (0x0040B060) - call at 0x00409ECA; vtable entry: slot 27 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __stdcall VirtualOpcodeHandler_Slot27(void *this_ptr, int arg4, int arg8, int *out_status)`
- Summary: Executes a specific script/bytecode virtual opcode method from vtable slot 27, writing a status code and an integer value to an output structure.

### sub_40B090 [GetSystemStateOrValue] (0x0040B090) - call at 0x00409ECA; vtable entry: slot 28 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl sub_40B090(void *this_ptr, int arg4, int arg8, void *out_struct)`
- Summary: Retrieves a global state value and writes a 64-bit output structure containing a boolean flag and the retrieved value.

### sub_40B0C0 [HandleOpcodeType2] (0x0040B0C0) - call at 0x00409ECA; vtable entry: slot 29 of vtable 0x0041D228, vtable used at 0x00409EAC

- Prototype: `void __cdecl HandleOpcodeType2(void *this_ptr, const void *operand_struct)`
- Summary: Handles a specific bytecode/script operand or instruction by checking its type tag and storing either a converted floating-point or direct integer value into a global state structure.

### sub_409460 [GetFloatOperand] (0x00409460) - call at 0x00409F25

- Prototype: `double __cdecl GetFloatOperand(const void *operand)`
- Summary: Extracts and converts a floating-point operand value based on its type tag, returning it as a double in st(0).

### __ftol2_sse (0x0041A160) - call at 0x00409F6A

- Prototype: `int __cdecl __ftol2_sse(void)`
- Summary: Converts the top-of-stack x87 floating-point value to a 32-bit signed integer using SSE or fallback FPU routines, returning the integer result in EAX.

Reference kinds used above:
- *vtable entry*: the function is an entry of a vtable whose address is used there - e.g. a constructor installing the vtable - so it may later be called virtually through it, but isn't called directly there.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x00409940  sub esp, 114h
0x00409946  push ebx
0x00409947  push ebp
0x00409948  push esi
0x00409949  push edi
0x0040994A  mov edi, [esp+124h+arg_0]
0x00409951  test edi, edi
0x00409953  mov esi, ecx
0x00409955  jl loc_4099E9
0x0040995B  cmp edi, [esi+90h]
0x00409961  jge loc_4099E9
0x00409967  cmp dword ptr [esi+99Ch], 10h
0x0040996E  jge loc_4099E9
0x00409970  xor edx, edx
0x00409972  cmp [esi+edi*4+13h], dl
0x00409976  jbe loc_4099B9
0x00409978  jmp loc_409980
0x00409980  mov eax, [esi+998h]
0x00409986  xor ecx, ecx
0x00409988  cmp eax, 100h
0x0040998D  mov byte ptr [esp+124h+var_F4], 1
0x00409992  jge loc_4099E2
0x00409994  mov ebx, [esp+124h+var_F4]
0x00409998  mov [esi+eax*8+198h], ebx
0x0040999F  mov [esi+eax*8+19Ch], ecx
0x004099A6  add dword ptr [esi+998h], 1
0x004099AD  movzx eax, byte ptr [esi+edi*4+13h]
0x004099B2  add edx, 1
0x004099B5  cmp edx, eax
0x004099B7  jl loc_409980
0x004099B9  add dword ptr [esi+99Ch], 1
0x004099C0  movzx edi, word ptr [esi+edi*4+10h]
0x004099C5  xor eax, eax
0x004099C7  cmp edi, [esi+0Ch]
0x004099CA  jb loc_409A04
0x004099CC  or eax, 0FFFFFFFFh
0x004099CF  add [esi+99Ch], eax
0x004099D5  pop edi
0x004099D6  pop esi
0x004099D7  pop ebp
0x004099D8  pop ebx
0x004099D9  add esp, 114h
0x004099DF  retn 8
0x004099E2  mov byte ptr [esi+0A68h], 1
0x004099E9  or eax, 0FFFFFFFFh
0x004099EC  pop edi
0x004099ED  pop esi
0x004099EE  pop ebp
0x004099EF  pop ebx
0x004099F0  add esp, 114h
0x004099F6  retn 8
0x00409A00  mov eax, [esp+124h+var_F8]
0x00409A04  mov ecx, [esi+0A64h]
0x00409A0A  add eax, 1
0x00409A0D  lea edx, [ecx+eax]
0x00409A10  cmp edx, 7530h
0x00409A16  mov [esp+124h+var_F8], eax
0x00409A1A  jg loc_4099CC
0x00409A1C  cmp byte ptr [esi+0A68h], 0
0x00409A23  jnz loc_4099CC
0x00409A25  mov eax, [esi+8]
0x00409A28  mov bl, [eax+edi]
0x00409A2B  movzx ebp, bl
0x00409A2E  add edi, 1
0x00409A31  cmp ebp, 63h
0x00409A34  ja loc_409F93
0x00409A3A  movzx ecx, ss:byte_40A060[ebp]               ; -> 0x0040A060 in .text
0x00409A41  jmp ds:jpt_409A41[ecx*4]                     ; -> 0x0040A004 in .text
0x00409A48  movzx ecx, byte ptr [eax+edi+1]
0x00409A4D  xor edx, edx
0x00409A4F  mov dh, [eax+edi+3]
0x00409A53  mov byte ptr [esp+124h+var_74], 1
0x00409A5B  mov dl, [eax+edi+2]
0x00409A5F  movzx eax, byte ptr [eax+edi]
0x00409A63  shl edx, 8
0x00409A66  or edx, ecx
0x00409A68  mov ecx, [esp+124h+var_74]
0x00409A6F  shl edx, 8
0x00409A72  or eax, edx
0x00409A74  mov [esp+124h+var_78], eax
0x00409A7B  lea edx, [esp+124h+var_7C]
0x00409A82  mov eax, esi
0x00409A84  mov [esp+124h+var_7C], ecx
0x00409A8B  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409A90  add edi, 4
0x00409A93  jmp loc_409F9A
0x00409A98  movzx ecx, byte ptr [eax+edi+1]
0x00409A9D  xor edx, edx
0x00409A9F  mov dh, [eax+edi+3]
0x00409AA3  mov byte ptr [esp+124h+var_EC], 2
0x00409AA8  mov dl, [eax+edi+2]
0x00409AAC  movzx eax, byte ptr [eax+edi]
0x00409AB0  shl edx, 8
0x00409AB3  or edx, ecx
0x00409AB5  mov ecx, [esp+124h+var_EC]
0x00409AB9  shl edx, 8
0x00409ABC  or edx, eax
0x00409ABE  mov [esp+124h+var_E4], edx
0x00409AC2  fld
0x00409AC6  fstp
0x00409ACA  mov edx, [esp+124h+var_E8]
0x00409ACE  mov [esp+124h+var_B8], edx
0x00409AD2  lea edx, [esp+124h+var_BC]
0x00409AD6  mov eax, esi
0x00409AD8  mov [esp+124h+var_BC], ecx
0x00409ADC  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409AE1  add edi, 4
0x00409AE4  jmp loc_409F9A
0x00409AE9  movzx eax, byte ptr [eax+edi]
0x00409AED  and eax, 1Fh
0x00409AF0  lea edx, [esi+eax*8+94h]
0x00409AF7  mov eax, esi
0x00409AF9  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409AFE  add edi, 1
0x00409B01  jmp loc_409F9A
0x00409B06  movzx ecx, byte ptr [eax+edi]
0x00409B0A  and ecx, 1Fh
0x00409B0D  lea ecx, [esi+ecx*8+94h]
0x00409B14  mov eax, esi
0x00409B16  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409B1B  add edi, 1
0x00409B1E  jmp loc_409F9A
0x00409B23  movzx edx, byte ptr [eax+edi]
0x00409B27  add edx, [esp+124h+arg_4]
0x00409B2E  mov eax, esi
0x00409B30  lea edx, [esi+edx*8+198h]
0x00409B37  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409B3C  add edi, 1
0x00409B3F  jmp loc_409F9A
0x00409B44  lea ecx, [esp+124h+var_110]
0x00409B48  mov eax, esi
0x00409B4A  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409B4F  mov eax, [esi+8]
0x00409B52  movzx eax, byte ptr [edi+eax]
0x00409B56  add eax, [esp+124h+arg_4]
0x00409B5D  mov ecx, [esp+124h+var_110]
0x00409B61  mov edx, [esp+124h+var_10C]
0x00409B65  mov [esi+eax*8+198h], ecx
0x00409B6C  mov [esi+eax*8+19Ch], edx
0x00409B73  add edi, 1
0x00409B76  jmp loc_409F9A
0x00409B7B  lea ecx, [esp+124h+var_108]
0x00409B7F  mov eax, esi
0x00409B81  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409B86  lea ecx, [esp+124h+var_110]
0x00409B8A  mov eax, esi
0x00409B8C  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409B91  lea ecx, [esp+124h+var_108]
0x00409B95  lea eax, [esp+124h+var_110]
0x00409B99  mov edx, ebp
0x00409B9B  call sub_4094B0                              ; -> sub_4094B0 [ExecuteBinaryOp]
0x00409BA0  mov [esp+124h+var_94], eax
0x00409BA7  mov [esp+124h+var_90], edx
0x00409BAE  lea edx, [esp+124h+var_94]
0x00409BB5  mov eax, esi
0x00409BB7  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409BBC  jmp loc_409F9A
0x00409BC1  lea ecx, [esp+124h+var_110]
0x00409BC5  mov eax, esi
0x00409BC7  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409BCC  cmp byte ptr [esp+124h+var_110], 2
0x00409BD1  jnz loc_409C0B
0x00409BD3  fld
0x00409BD7  mov byte ptr [esp+124h+var_E0], 2
0x00409BDC  mov eax, [esp+124h+var_E0]
0x00409BE0  fchs
0x00409BE2  fstp
0x00409BE6  mov ecx, [esp+124h+var_DC]
0x00409BEA  mov [esp+124h+var_8C], eax
0x00409BF1  lea edx, [esp+124h+var_8C]
0x00409BF8  mov eax, esi
0x00409BFA  mov [esp+124h+var_88], ecx
0x00409C01  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409C06  jmp loc_409F9A
0x00409C0B  mov eax, [esp+124h+var_10C]
0x00409C0F  neg eax
0x00409C11  mov byte ptr [esp+124h+var_54], 1
0x00409C19  mov edx, [esp+124h+var_54]
0x00409C20  mov [esp+124h+var_C4], edx
0x00409C24  mov [esp+124h+var_C0], eax
0x00409C28  lea edx, [esp+124h+var_C4]
0x00409C2C  mov eax, esi
0x00409C2E  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409C33  jmp loc_409F9A
0x00409C38  lea ecx, [esp+124h+var_108]
0x00409C3C  mov eax, esi
0x00409C3E  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409C43  lea ecx, [esp+124h+var_110]
0x00409C47  mov eax, esi
0x00409C49  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409C4E  lea ecx, [esp+124h+var_108]
0x00409C52  lea eax, [esp+124h+var_110]
0x00409C56  mov edx, ebp
0x00409C58  mov byte ptr [esp+124h+var_4C], 4
0x00409C60  call sub_409680                              ; -> sub_409680 [CompareVariantValues]
0x00409C65  mov ecx, [esp+124h+var_4C]
0x00409C6C  neg al
0x00409C6E  lea edx, [esp+124h+var_A4]
0x00409C75  mov [esp+124h+var_A4], ecx
0x00409C7C  sbb eax, eax
0x00409C7E  neg eax
0x00409C80  mov [esp+124h+var_A0], eax
0x00409C87  mov eax, esi
0x00409C89  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409C8E  jmp loc_409F9A
0x00409C93  lea ecx, [esp+124h+var_108]
0x00409C97  mov eax, esi
0x00409C99  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409C9E  lea ecx, [esp+124h+var_110]
0x00409CA2  mov eax, esi
0x00409CA4  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409CA9  mov eax, ecx
0x00409CAB  call sub_409480                              ; -> sub_409480 [CheckOperandTruthState]
0x00409CB0  test al, al
0x00409CB2  jz loc_409CC5
0x00409CB4  lea eax, [esp+124h+var_108]
0x00409CB8  call sub_409480                              ; -> sub_409480 [CheckOperandTruthState]
0x00409CBD  test al, al
0x00409CBF  jz loc_409CC5
0x00409CC1  mov al, 1
0x00409CC3  jmp loc_409CC7
0x00409CC5  xor al, al
0x00409CC7  xor edx, edx
0x00409CC9  test al, al
0x00409CCB  setnz dl
0x00409CCE  mov byte ptr [esp+124h+var_5C], 4
0x00409CD6  mov ecx, [esp+124h+var_5C]
0x00409CDD  mov [esp+124h+var_84], ecx
0x00409CE4  mov eax, edx
0x00409CE6  mov [esp+124h+var_80], eax
0x00409CED  lea edx, [esp+124h+var_84]
0x00409CF4  mov eax, esi
0x00409CF6  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409CFB  jmp loc_409F9A
0x00409D00  lea ecx, [esp+124h+var_108]
0x00409D04  mov eax, esi
0x00409D06  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409D0B  lea ecx, [esp+124h+var_110]
0x00409D0F  mov eax, esi
0x00409D11  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409D16  mov eax, ecx
0x00409D18  call sub_409480                              ; -> sub_409480 [CheckOperandTruthState]
0x00409D1D  test al, al
0x00409D1F  jnz loc_409D2E
0x00409D21  lea eax, [esp+124h+var_108]
0x00409D25  call sub_409480                              ; -> sub_409480 [CheckOperandTruthState]
0x00409D2A  test al, al
0x00409D2C  jz loc_409D30
0x00409D2E  mov al, 1
0x00409D30  xor edx, edx
0x00409D32  test al, al
0x00409D34  setnz dl
0x00409D37  mov byte ptr [esp+124h+var_6C], 4
0x00409D3F  mov ecx, [esp+124h+var_6C]
0x00409D46  mov [esp+124h+var_B4], ecx
0x00409D4A  mov eax, edx
0x00409D4C  mov [esp+124h+var_B0], eax
0x00409D50  lea edx, [esp+124h+var_B4]
0x00409D54  mov eax, esi
0x00409D56  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409D5B  jmp loc_409F9A
0x00409D60  lea ecx, [esp+124h+var_110]
0x00409D64  mov eax, esi
0x00409D66  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409D6B  mov eax, ecx
0x00409D6D  mov byte ptr [esp+124h+var_64], 4
0x00409D75  call sub_409480                              ; -> sub_409480 [CheckOperandTruthState]
0x00409D7A  mov edx, [esp+124h+var_64]
0x00409D81  neg al
0x00409D83  mov [esp+124h+var_AC], edx
0x00409D87  lea edx, [esp+124h+var_AC]
0x00409D8B  sbb eax, eax
0x00409D8D  add eax, 1
0x00409D90  mov [esp+124h+var_A8], eax
0x00409D94  mov eax, esi
0x00409D96  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409D9B  jmp loc_409F9A
0x00409DA0  xor ecx, ecx
0x00409DA2  mov ch, [eax+edi+1]
0x00409DA6  movzx eax, byte ptr [eax+edi]
0x00409DAA  movsx edx, cx
0x00409DAD  or edx, eax
0x00409DAF  lea edi, [edi+edx+2]
0x00409DB3  jmp loc_409F9A
0x00409DB8  lea ecx, [esp+124h+var_110]
0x00409DBC  mov eax, esi
0x00409DBE  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409DC3  mov eax, ecx
0x00409DC5  call sub_409480                              ; -> sub_409480 [CheckOperandTruthState]
0x00409DCA  xor edx, edx
0x00409DCC  cmp bl, 42h
0x00409DCF  setz dl
0x00409DD2  movzx ecx, al
0x00409DD5  cmp ecx, edx
0x00409DD7  jnz loc_409DF4
0x00409DD9  mov eax, [esi+8]
0x00409DDC  add eax, edi
0x00409DDE  xor ecx, ecx
0x00409DE0  mov ch, [eax+1]
0x00409DE3  movzx eax, byte ptr [eax]
0x00409DE6  movsx edx, cx
0x00409DE9  or edx, eax
0x00409DEB  lea edi, [edi+edx+2]
0x00409DEF  jmp loc_409F9A
0x00409DF4  add edi, 2
0x00409DF7  jmp loc_409F9A
0x00409DFC  movzx ecx, byte ptr [eax+edi]
0x00409E00  add edi, 1
0x00409E03  cmp ecx, [esi+90h]
0x00409E09  jge loc_409F93
0x00409E0F  movzx ebx, byte ptr [esi+ecx*4+12h]
0x00409E14  mov eax, [esi+998h]
0x00409E1A  sub eax, ebx
0x00409E1C  cmp eax, [esp+124h+arg_4]
0x00409E23  jl loc_409F93
0x00409E29  push eax
0x00409E2A  push ecx
0x00409E2B  mov ecx, esi
0x00409E2D  mov [esi+0A64h], edx
0x00409E33  mov [esp+12Ch+var_F8], 0
0x00409E3B  call sub_409940                              ; -> self
0x00409E40  test eax, eax
0x00409E42  jl loc_4099CC
0x00409E48  jmp loc_409F9A
0x00409E4D  movzx ecx, byte ptr [eax+edi]
0x00409E51  movzx ebx, byte ptr [eax+edi+1]
0x00409E56  add edi, 2
0x00409E59  cmp ecx, 1Eh
0x00409E5C  mov [esp+124h+var_D8], ecx
0x00409E60  jge loc_409F93
0x00409E66  cmp ebx, 8
0x00409E69  jg loc_409F93
0x00409E6F  mov edx, [esi+998h]
0x00409E75  sub edx, ebx
0x00409E77  cmp edx, [esp+124h+arg_4]
0x00409E7E  jl loc_409F93
0x00409E84  lea ebp, [ebx-1]
0x00409E87  test ebp, ebp
0x00409E89  jl loc_409EA7
0x00409E8B  lea ecx, [esp+ebp*8+124h+var_44]
0x00409E92  mov eax, esi
0x00409E94  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409E99  sub ebp, 1
0x00409E9C  sub ecx, 8
0x00409E9F  test ebp, ebp
0x00409EA1  jge loc_409E92
0x00409EA3  mov ecx, [esp+124h+var_D8]
0x00409EA7  lea eax, [esp+124h+var_D4]
0x00409EAB  push eax
0x00409EAC  mov eax, ds:funcs_409ECA[ecx*4]              ; -> vtable (30 entries)
0x00409EB3  push ebx
0x00409EB4  lea edx, [esp+12Ch+var_44]
0x00409EBB  push edx
0x00409EBC  push esi
0x00409EBD  mov [esp+134h+var_D4], 0
0x00409EC2  mov [esp+134h+var_D0], 0
0x00409ECA  call eax                                     ; -> sub_40A2A0 [OpcodeHandler_LoadConstantOrVariable], sub_40A2D0 [OpcodeRandomRange], sub_40A380 [ExecuteRandomRangeOp], sub_40A3F0 [ExecuteSpawnEntityScriptOpcode], sub_40A540 [ResolveScriptReference], sub_40A5A0 [VM_GetGameVariableFloat], sub_40A620 [ExecuteScriptOpcode_TypeCheckOrRangeOp], sub_40A6E0 [BytecodeOpcodeHandler_Slot7], sub_40A720 [BytecodeOpcode_Slot8_EvalOrDefault], sub_40A760 [VM_ResolveResourceHandle], sub_40A7E0 [ExecuteBytecodeOp_10], sub_40A830 [VM_OpcodeHandler_Slot11], sub_40A880 [ExecuteBytecodeTypeOp], sub_40A8E0 [ExecuteBytecodeOpcode13], sub_40A960 [EvalScriptReference], sub_40A9E0 [ScriptOp_GetResourceValue], sub_40AA60 [ExecuteEntityInteractionOrCommandOpcode], sub_40AB80 [ExecuteBytecodeVtableSlot17], sub_40ABE0 [HandleBytecodeStatusOrEvent], sub_40AC10 [ExecuteOpcode19], sub_40AC70 [ExecuteScriptMathOrConversionOp], sub_40AD00 [ScheduleTimedEventOrState], sub_40ADF0 [VM_OpcodeHandler_Slot22], sub_40AE80 [ExecuteScriptOpcode_Slot23], sub_40AEF0 [ExecuteVmOpcodeSlot24], sub_40AF60 [BytecodeOpcode_HandleReferenceOrResource], sub_40AFE0 [VerifyObjectReference], sub_40B060 [VirtualOpcodeHandler_Slot27], sub_40B090 [GetSystemStateOrValue], sub_40B0C0 [HandleOpcodeType2]
0x00409ECC  add esp, 10h
0x00409ECF  lea edx, [esp+124h+var_D4]
0x00409ED3  mov eax, esi
0x00409ED5  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409EDA  jmp loc_409F9A
0x00409EDF  lea ecx, [esp+124h+var_110]
0x00409EE3  mov eax, esi
0x00409EE5  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409EEA  jmp loc_409F9A
0x00409EEF  lea ecx, [esp+124h+var_110]
0x00409EF3  mov eax, esi
0x00409EF5  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409EFA  mov edx, ecx
0x00409EFC  mov eax, esi
0x00409EFE  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409F03  lea edx, [esp+124h+var_110]
0x00409F07  mov eax, esi
0x00409F09  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409F0E  jmp loc_409F9A
0x00409F13  lea ecx, [esp+124h+var_110]
0x00409F17  mov eax, esi
0x00409F19  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409F1E  mov eax, ecx
0x00409F20  mov byte ptr [esp+124h+var_100], 2
0x00409F25  call sub_409460                              ; -> sub_409460 [GetFloatOperand]
0x00409F2A  fstp
0x00409F2E  mov edx, [esp+124h+var_FC]
0x00409F32  mov ecx, [esp+124h+var_100]
0x00409F36  mov [esp+124h+var_98], edx
0x00409F3D  lea edx, [esp+124h+var_9C]
0x00409F44  mov eax, esi
0x00409F46  mov [esp+124h+var_9C], ecx
0x00409F4D  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409F52  jmp loc_409F9A
0x00409F54  lea ecx, [esp+124h+var_110]
0x00409F58  mov eax, esi
0x00409F5A  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409F5F  cmp byte ptr [esp+124h+var_110], 2
0x00409F64  jnz loc_409F71
0x00409F66  fld
0x00409F6A  call __ftol2_sse                             ; -> __ftol2_sse
0x00409F6F  jmp loc_409F75
0x00409F71  mov eax, [esp+124h+var_10C]
0x00409F75  mov byte ptr [esp+124h+var_CC], 1
0x00409F7A  mov ecx, [esp+124h+var_CC]
0x00409F7E  mov [esp+124h+var_F0], eax
0x00409F82  lea edx, [esp+124h+var_F4]
0x00409F86  mov eax, esi
0x00409F88  mov [esp+124h+var_F4], ecx
0x00409F8C  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409F91  jmp loc_409F9A
0x00409F93  mov byte ptr [esi+0A68h], 1
0x00409F9A  cmp edi, [esi+0Ch]
0x00409F9D  jb loc_409A00
0x00409FA3  jmp loc_4099CC
0x00409FA8  mov byte ptr [esp+124h+var_CC], 1
0x00409FAD  mov edx, [esp+124h+var_CC]
0x00409FB1  xor eax, eax
0x00409FB3  cmp bl, 53h
0x00409FB6  mov [esp+124h+var_100], edx
0x00409FBA  mov [esp+124h+var_FC], eax
0x00409FBE  jnz loc_409FCB
0x00409FC0  lea ecx, [esp+124h+var_100]
0x00409FC4  mov eax, esi
0x00409FC6  call sub_4098F0                              ; -> sub_4098F0 [VM_PopOperand]
0x00409FCB  mov eax, [esp+124h+arg_4]
0x00409FD2  mov [esi+998h], eax
0x00409FD8  lea edx, [esp+124h+var_100]
0x00409FDC  mov eax, esi
0x00409FDE  call sub_4098B0                              ; -> sub_4098B0 [VM_PushValue]
0x00409FE3  add dword ptr [esi+99Ch], 0FFFFFFFFh
0x00409FEA  mov eax, [esp+124h+var_F8]
0x00409FEE  add [esi+0A64h], eax
0x00409FF4  pop edi
0x00409FF5  pop esi
0x00409FF6  pop ebp
0x00409FF7  pop ebx
0x00409FF8  add esp, 114h
0x00409FFE  retn 8
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
