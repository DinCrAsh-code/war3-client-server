//============================================================================
//  One raw CPU-feature leaf from GameMain's own startup closure
//  (gamemain-batch-5).  Its neighbour, 0x6F6C4CC0's `rdtsc` reader, was
//  independently assigned to both this batch and gamemain-batch-3, which
//  reconstructed it first as ReadTimeStampCounter (Sync/
//  timerfrequencycalibrate.cpp) - dropped from here rather than committed
//  twice under two names at the same address.
//============================================================================

//  0x6F6CDB60 - classic three-stage CPUID feature probe: toggle EFLAGS bit
//  21 (ID) through the stack to confirm CPUID exists at all, then read
//  leaf 1 (standard features - ebx/ecx/edx into *arg_0), leaf 1's own edx
//  again into *arg_4, and - only once leaf 0x80000000 answers past that
//  value - leaf 0x80000001's edx (the AMD/long-mode extended feature set)
//  into *arg_8.  cdecl: the three out-pointers are stack arguments and the
//  caller cleans them; the return is the ID-bit toggle succeeding at all,
//  which every branch past the first early-out sets to 1.
//
//  Left with the shipped `pushf`/`popf` toggle-and-compare and the raw
//  `cpuid` leaves spelled out, because MSVC has no intrinsic for either in
//  this /GS- /Zl toolchain's headers and the dump's own three back-to-back
//  `cpuid`s (with ebx/ecx/edx read out in a different order each time) is
//  not reproducible from `__cpuid()`-style C without guessing at register
//  scheduling the naked block does not need.
__declspec(naked) int __cdecl ProbeCpuidFeatures(unsigned int* stdFeatures,
                                                 unsigned int* stdFeaturesEdx,
                                                 unsigned int* extFeaturesEdx)
{
    __asm
    {
        push    ebp
        mov     ebp, esp
        push    ecx
        mov     eax, [ebp+8]
        mov     edx, [ebp+16]
        xor     ecx, ecx
        mov     [eax], ecx
        mov     [eax+4], ecx
        mov     [eax+8], ecx
        mov     eax, [ebp+12]
        mov     [eax], ecx
        push    ebx
        mov     [edx], ecx
        mov     [ebp-4], ecx
        push    eax
        push    ebx
        push    ecx
        push    edx
        pushf
        pop     eax
        mov     ecx, eax
        xor     eax, 200000h
        push    eax
        popf
        pushf
        pop     eax
        xor     eax, ecx
        jz      no_cpuid
        mov     eax, 0
        cpuid
        test    eax, eax
        jz      no_cpuid
        mov     dword ptr [ebp-4], 1
        mov     eax, [ebp+8]
        mov     [eax], ebx
        mov     [eax+4], ecx
        mov     [eax+8], edx
        mov     eax, 1
        cpuid
        mov     eax, [ebp+12]
        mov     [eax], edx
        mov     eax, 80000000h
        cpuid
        cmp     eax, 80000000h
        jbe     no_cpuid
        mov     eax, 80000001h
        cpuid
        mov     eax, [ebp+16]
        mov     [eax], edx
    no_cpuid:
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        mov     eax, [ebp-4]
        pop     ebx
        mov     esp, ebp
        pop     ebp
        retn
    }
}
