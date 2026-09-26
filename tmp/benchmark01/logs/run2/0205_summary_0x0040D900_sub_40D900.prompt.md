# Function summary: sub_40D900 (0x0040D900)

You are an expert reverse engineer analyzing one function of a stripped Windows binary (no symbols, no debug info). Names like `sub_00401000` are auto-generated placeholders; any other name (an export, an import thunk like `j_malloc`, a library function the disassembler recognized) is real and can be trusted. A name in square brackets, e.g. `sub_00401596 [PushFront]`, is a name suggested by an earlier step of this same analysis - informed, but not ground truth.

This is 32-bit x86 code from an optimizing compiler (possibly MSVC with whole-program optimization, /GL + /LTCG). Besides __cdecl, __stdcall, __thiscall (`this` in ecx) and __fastcall, non-exported functions may use custom register-based conventions - infer parameters from which registers and stack slots are read before being written, and note that floating-point values come back in st(0).

The call graph rooted at _main (0x00401050) is being analyzed bottom-up. Every function this one calls has already been analyzed - their summaries are below, and the disassembly is annotated with their suggested names. Every function that calls this one has described, from its own side, what it uses this function for ("How callers use this function"). Use all of that as context, but the disassembly is the ground truth: where the context and the code disagree, trust the code and say so.

## Binary

04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1

## Function

- Address: 0x0040D900
- Name: sub_40D900 (auto-generated placeholder)
- Size: 397 bytes, 165 instructions
- One call chain from the analysis root down to it: _main -> sub_401750 -> sub_402980 -> sub_4084C0 -> sub_4080A0 -> sub_40DAC0 -> sub_40D900

## How callers use this function

### From sub_40DAC0 (0x0040DAC0) - call at 0x0040DB15

What that caller does overall (preliminary): Validates and processes a state or command transition for an entity (passed in ecx with additional parameter data in eax), handling property updates, type checks, and invoking associated subsystem helpers depending on entity flags and type codes.

Called to retrieve or transform attribute/type data into a local structure when the primary entity type is 7, passing the entity pointer in eax and a destination buffer pointer in edi.

## Functions it calls or references

### sub_40CAF0 [ValidateEntityPairCompatibility] (0x0040CAF0) - call at 0x0040D9CE, 0x0040DA59

- Prototype: `bool __fastcall ValidateEntityPairCompatibility(void *entityA, void *entityB)`
- Summary: Performs a multi-stage validation check between two entities (passed in ebx and esi) to determine if they are compatible for interaction or spatial association.

## Disassembly

Comments after `; ->` say what an operand address points at.

```asm
0x0040D900  push ecx
0x0040D901  mov ecx, [eax]
0x0040D903  mov edx, [eax+4]
0x0040D906  mov [edi], ecx
0x0040D908  mov ecx, [eax+8]
0x0040D90B  mov [edi+4], edx
0x0040D90E  mov edx, [eax+0Ch]
0x0040D911  push ebx
0x0040D912  mov ebx, [esp+8+arg_0]
0x0040D916  mov [edi+8], ecx
0x0040D919  push ebp
0x0040D91A  mov ebp, dword_42AC60                        ; -> 0x0042AC60 in .data
0x0040D920  mov [edi+0Ch], edx
0x0040D923  test byte ptr [edi+1], 0Ah
0x0040D927  push esi
0x0040D928  jz loc_40D964
0x0040D92A  mov eax, [edi+4]
0x0040D92D  test eax, eax
0x0040D92F  jz loc_40D964
0x0040D931  mov ecx, eax
0x0040D933  and ecx, 0FFFFh
0x0040D939  sub ecx, 1
0x0040D93C  js loc_40D95E
0x0040D93E  cmp ecx, [ebp+51E84h]
0x0040D944  jge loc_40D95E
0x0040D946  mov edx, [ebp+51E80h]
0x0040D94C  lea ecx, [edx+ecx*8]
0x0040D94F  shr eax, 10h
0x0040D952  cmp [ecx+4], ax
0x0040D956  jnz loc_40D95E
0x0040D958  mov ecx, [ecx]
0x0040D95A  mov esi, ecx
0x0040D95C  jmp loc_40D966
0x0040D95E  xor ecx, ecx
0x0040D960  mov esi, ecx
0x0040D962  jmp loc_40D966
0x0040D964  xor esi, esi
0x0040D966  test esi, esi
0x0040D968  jz loc_40DA31
0x0040D96E  mov eax, [esi+8]
0x0040D971  mov ecx, [esi+0Ch]
0x0040D974  mov [edi+8], eax
0x0040D977  mov al, [esi+1Ah]
0x0040D97A  cmp al, 1
0x0040D97C  mov [edi+0Ch], ecx
0x0040D97F  jnz loc_40DA3B
0x0040D985  mov bl, [ebx+1Bh]
0x0040D988  mov dl, [esi+1Bh]
0x0040D98B  movzx eax, bl
0x0040D98E  imul eax
0x0040D994  add eax, ebp
0x0040D996  movzx ebp, byte ptr [eax]
0x0040D999  movzx ecx, dl
0x0040D99C  cmp ecx, ebp
0x0040D99E  mov [esp+10h+var_2], bl
0x0040D9A2  jz loc_40D9EA
0x0040D9A4  cmp ecx, 5
0x0040D9A7  ja loc_40D9EA
0x0040D9A9  test byte ptr [eax+3], 10h
0x0040D9AD  jnz loc_40D9EA
0x0040D9AF  cmp ecx, 5
0x0040D9B2  jz loc_40D9EA
0x0040D9B4  mov ebp, 1
0x0040D9B9  shl ebp, cl
0x0040D9BB  test [eax+0E8h], ebp
0x0040D9C1  setz al
0x0040D9C4  test al, al
0x0040D9C6  jz loc_40D9EA
0x0040D9C8  mov ebp, [esp+10h+arg_0]
0x0040D9CC  mov ebx, ebp
0x0040D9CE  call sub_40CAF0                              ; -> sub_40CAF0 [ValidateEntityPairCompatibility]
0x0040D9D3  test al, al
0x0040D9D5  jz loc_40D9E1
0x0040D9D7  pop esi
0x0040D9D8  pop ebp
0x0040D9D9  mov byte ptr [edi], 4
0x0040D9DC  mov eax, edi
0x0040D9DE  pop ebx
0x0040D9DF  pop ecx
0x0040D9E0  retn
0x0040D9E1  mov bl, [esp+10h+var_2]
0x0040D9E5  mov dl, [esi+1Bh]
0x0040D9E8  jmp loc_40D9EE
0x0040D9EA  mov ebp, [esp+10h+arg_0]
0x0040D9EE  mov eax, [esi+1Ch]
0x0040D9F1  movzx eax, word ptr [eax]
0x0040D9F4  test al, al
0x0040D9F6  jns loc_40DA0F
0x0040D9F8  mov ecx, [ebp+1Ch]
0x0040D9FB  mov cl, [ecx]
0x0040D9FD  shr cl, 2
0x0040DA00  test cl, 1
0x0040DA03  jz loc_40DA0F
0x0040DA05  pop esi
0x0040DA06  pop ebp
0x0040DA07  mov byte ptr [edi], 8
0x0040DA0A  mov eax, edi
0x0040DA0C  pop ebx
0x0040DA0D  pop ecx
0x0040DA0E  retn
0x0040DA0F  mov ecx, [ebp+1Ch]
0x0040DA12  mov cl, [ecx]
0x0040DA14  shr cl, 2
0x0040DA17  test cl, 1
0x0040DA1A  jz loc_40DA31
0x0040DA1C  cmp dl, bl
0x0040DA1E  jnz loc_40DA31
0x0040DA20  test eax, 140h
0x0040DA25  jz loc_40DA31
0x0040DA27  pop esi
0x0040DA28  pop ebp
0x0040DA29  mov byte ptr [edi], 9
0x0040DA2C  mov eax, edi
0x0040DA2E  pop ebx
0x0040DA2F  pop ecx
0x0040DA30  retn
0x0040DA31  pop esi
0x0040DA32  pop ebp
0x0040DA33  mov byte ptr [edi], 3
0x0040DA36  mov eax, edi
0x0040DA38  pop ebx
0x0040DA39  pop ecx
0x0040DA3A  retn
0x0040DA3B  cmp al, 3
0x0040DA3D  jnz loc_40DA70
0x0040DA3F  mov edx, [ebx+1Ch]
0x0040DA42  mov al, [edx]
0x0040DA44  shr al, 2
0x0040DA47  test al, 1
0x0040DA49  jz loc_40DA59
0x0040DA4B  pop esi
0x0040DA4C  mov eax, 8
0x0040DA51  pop ebp
0x0040DA52  mov [edi], al
0x0040DA54  mov eax, edi
0x0040DA56  pop ebx
0x0040DA57  pop ecx
0x0040DA58  retn
0x0040DA59  call sub_40CAF0                              ; -> sub_40CAF0 [ValidateEntityPairCompatibility]
0x0040DA5E  neg al
0x0040DA60  pop esi
0x0040DA61  pop ebp
0x0040DA62  pop ebx
0x0040DA63  sbb eax, eax
0x0040DA65  neg eax
0x0040DA67  add eax, 3
0x0040DA6A  mov [edi], al
0x0040DA6C  mov eax, edi
0x0040DA6E  pop ecx
0x0040DA6F  retn
0x0040DA70  mov ecx, [ebx+1Ch]
0x0040DA73  mov dl, [ecx]
0x0040DA75  shr dl, 1
0x0040DA77  and dl, 1
0x0040DA7A  neg dl
0x0040DA7C  sbb dl, dl
0x0040DA7E  pop esi
0x0040DA7F  and dl, 0Bh
0x0040DA82  add dl, 3
0x0040DA85  pop ebp
0x0040DA86  mov [edi], dl
0x0040DA88  mov eax, edi
0x0040DA8A  pop ebx
0x0040DA8B  pop ecx
0x0040DA8C  retn
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
