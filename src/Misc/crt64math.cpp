//============================================================================
//  __allmul (0x6F7E1730), __aullshr (0x6F7E17A0) and __alldiv (0x6F7E1A10) -
//  MSVC8's own 64-bit multiply, unsigned shift-right and signed divide
//  helpers, statically linked from the CRT (vs8/VC/crt/src/intel/llmul.asm,
//  ullshr.asm and lldiv.asm ship only as .obj here, the same as
//  sehprolg4.obj does for crtseh4.cpp).  Not game code and not spellable in
//  C++: the compiler *emits* the calls to them, so there is no source shape
//  that would produce their bodies.  Transcribed directly, the same
//  technique crtseh4.cpp, gsreport.cpp and crtsec.cpp already use.
//
//  These really are called from this build: every 64-bit multiply and shift
//  MSVC generates inside CFloatDistanceSquared's own module reaches
//  __allmul/__aullshr, CFloatSinCos (cfloatsincos.cpp) carries eight such
//  call sites, and __alldiv is reached from sub_6F6C4CD0's own 64-bit
//  QueryPerformanceCounter delta division (gamemain-batch-3,
//  Sync/timerfrequencycalibrate.cpp).  So the definitions here are what the
//  `.mix` binds those calls to, and they must carry the CRT's own decorated
//  names - `extern "C"` on `_allmul` / `_aullshr` / `_alldiv`, which is
//  exactly `__allmul` / `__aullshr` / `__alldiv` in the object.
//
//  None takes stack arguments in the ordinary sense: __allmul's and
//  __alldiv's four words are pushed by the caller and cleaned by the
//  callee (`retn 10h` both), and __aullshr's operand arrives in edx:eax
//  with the count in cl and nothing on the stack at all.
//============================================================================

#pragma warning(disable : 4035)   // naked: no return value, both leave via
                                  // the transcribed asm's own ret/retn

extern "C" void __cdecl _allmul();
extern "C" void __cdecl _aullshr();
extern "C" void __cdecl _alldiv();

//----------------------------------------------------------------------------
//  0x6F7E1730 - (a * b) for two 64-bit operands pushed low-word-first.
//
//  The fast path is the one both high words are zero on, which is a single
//  32x32->64 `mul`.  The slow path adds the two cross products into the high
//  half; the third partial product (high * high) is discarded, because it
//  only reaches bits 64 and up.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl _allmul()
{
    __asm
    {
        mov     eax, [esp + 8]          ; a.hi
        mov     ecx, [esp + 16]         ; b.hi
        or      ecx, eax
        mov     ecx, [esp + 12]         ; b.lo
        jnz     short hard
        mov     eax, [esp + 4]          ; a.lo
        mul     ecx
        retn    16
hard:
        push    ebx
        mul     ecx                     ; a.hi * b.lo
        mov     ebx, eax
        mov     eax, [esp + 4 + 4]      ; a.lo
        mul     dword ptr [esp + 4 + 16]; a.lo * b.hi
        add     ebx, eax
        mov     eax, [esp + 4 + 4]      ; a.lo
        mul     ecx                     ; a.lo * b.lo
        add     edx, ebx
        pop     ebx
        retn    16
    }
}

//----------------------------------------------------------------------------
//  0x6F7E17A0 - edx:eax >>= cl, unsigned, for a count of 0..63; 64 and up
//  answer zero rather than wrapping the shift count the way the bare
//  instruction would.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl _aullshr()
{
    __asm
    {
        cmp     cl, 64
        jnb     short retzero
        cmp     cl, 32
        jnb     short more32
        shrd    eax, edx, cl
        shr     edx, cl
        retn
more32:
        mov     eax, edx
        xor     edx, edx
        and     cl, 31
        shr     eax, cl
        retn
retzero:
        xor     eax, eax
        xor     edx, edx
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F7E1A10 - signed 64-bit divide, dividend / divisor, both pushed
//  low-word-first (divisor pushed last/nearest the return address, matching
//  vs8's own lldiv.asm DVND/DVSR stack comment).  Direct transcription of
//  that file: sign the result from the two operands' high words, force both
//  positive, take the cheap single-word-divisor path when possible and the
//  shift-and-correct long path otherwise, then re-apply the sign.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl _alldiv()
{
    __asm
    {
        push    edi
        push    esi
        push    ebx

        xor     edi, edi

        mov     eax, [esp + 20]         ; a.hi
        or      eax, eax
        jge     short L1
        inc     edi
        mov     edx, [esp + 16]         ; a.lo
        neg     eax
        neg     edx
        sbb     eax, 0
        mov     [esp + 20], eax
        mov     [esp + 16], edx
L1:
        mov     eax, [esp + 28]         ; b.hi
        or      eax, eax
        jge     short L2
        inc     edi
        mov     edx, [esp + 24]         ; b.lo
        neg     eax
        neg     edx
        sbb     eax, 0
        mov     [esp + 28], eax
        mov     [esp + 24], edx
L2:
        or      eax, eax
        jnz     short L3
        mov     ecx, [esp + 24]         ; b.lo
        mov     eax, [esp + 20]         ; a.hi
        xor     edx, edx
        div     ecx
        mov     ebx, eax
        mov     eax, [esp + 16]         ; a.lo
        div     ecx
        mov     edx, ebx
        jmp     short L4

L3:
        mov     ebx, eax                ; b.hi
        mov     ecx, [esp + 24]         ; b.lo
        mov     edx, [esp + 20]         ; a.hi
        mov     eax, [esp + 16]         ; a.lo
L5:
        shr     ebx, 1
        rcr     ecx, 1
        shr     edx, 1
        rcr     eax, 1
        or      ebx, ebx
        jnz     short L5
        div     ecx
        mov     esi, eax

        mul     dword ptr [esp + 28]    ; quot * b.hi
        mov     ecx, eax
        mov     eax, [esp + 24]         ; b.lo
        mul     esi                     ; quot * b.lo
        add     edx, ecx
        jb      short L6

        cmp     edx, [esp + 20]         ; a.hi
        ja      short L6
        jb      short L7
        cmp     eax, [esp + 16]         ; a.lo
        jbe     short L7
L6:
        dec     esi
L7:
        xor     edx, edx
        mov     eax, esi

L4:
        dec     edi
        jnz     short L8
        neg     edx
        neg     eax
        sbb     edx, 0
L8:
        pop     ebx
        pop     esi
        pop     edi

        retn    10h
    }
}
