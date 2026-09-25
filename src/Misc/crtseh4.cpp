//============================================================================
//  __SEH_prolog4 (0x6F7E1C84), __SEH_epilog4 (0x6F7E1CC9),
//  ?__ArrayUnwind@@YGXPAXIHP6EX0@Z@Z (0x6F7E1604) and
//  ??_M@YGXPAXIHP6EX0@Z@Z (the `eh vector destructor iterator`, 0x6F7E1662)
//  - MSVC8's own "unified" /GS+/EH runtime support routines, statically
//  linked from the CRT (vs8/VC/crt/src/intel/*/sehprolg4.obj, chandler4.obj
//  and friends ship only as .obj in vs8/, not source the way gs_report.c
//  did for gsreport.cpp), reached by every function in this dump's own call
//  tree whose frame docs/msvc-vc8-idioms.md's "An `__except_handler4`
//  -shaped frame this toolchain cannot reproduce" already establishes this
//  fixed /GS- /EHs-c- toolchain never emits from any C++ spelling here.
//  That entry is about *this repo's own reconstructed functions* never
//  reaching these routines through a real call site; it says nothing about
//  reconstructing the routines themselves, which - like SecurityCheckCookie
//  (crtsec.cpp) and __report_gsfailure (gsreport.cpp) before them - are
//  leaves in the call tree in their own right and are transcribed directly,
//  the same "the disassembly transcribed" technique already used for
//  CFloatToInt/CountLeadingZeros (softfloat.cpp) and SecurityCheckCookie.
//  Nothing in this repo's own reconstructed C++ ever calls any of these
//  four, so there is no correctness question about the bodies being
//  exercised by anything - each is scored purely against its own dump
//  entry, exactly as __report_gsfailure already is.
//
//  __SEH_prolog4/epilog4's own bodies use only raw numeric stack/frame
//  offsets in the dump (no IDA struct-field names), so those two are a
//  direct, unambiguous transcription.  ArrayUnwind and the eh-vector
//  iterator additionally reference an IDA-applied `ms_exc.registration.*`
//  struct overlay (TryLevel, exc_ptr, old_esp) whose exact byte offsets
//  IDA does not print numerically in this text dump.  Resolved here from
//  __SEH_prolog4's own two hard offsets it *does* give in raw hex
//  (`lea eax,[ebp-10h]` for the installed EXCEPTION_REGISTRATION's `Next`
//  field, `mov [ebp-18h],esp` for the "safe esp to restore to" slot the
//  epilogue-adjacent code reads back as `ms_exc.old_esp`) plus the
//  publicly documented shape of MSVC8's extended EH4 registration record
//  (Next@-0x10, Handler@-0xC, TryLevel@-0x14) - i.e. best-effort, not
//  independently verified against raw IDA hex, and irrelevant to
//  correctness either way since nothing here is ever called.
//============================================================================

#include "crtsec.h"   // g_securityCookie
#include "crtseh.h"   // the two array iterators' own declarations

extern "C" void __except_handler4();

//  This function's own compiler-generated __try/__except scope table - not
//  reconstructed as real data (nothing here is ever called, so its content
//  is moot), just a symbol for `offset` to reference.
extern "C" char stru_6FA4AAF0[];
extern "C" char stru_6FA4AB10[];

#pragma warning(disable : 4035)   // naked: no return value, both leave via
                                  // the transcribed asm's own ret/retn

//----------------------------------------------------------------------------
//  0x6F7E1C84 - the shared prologue every __except_handler4-shaped frame in
//  this binary calls with (push <scope table size>, push <scope table
//  pointer>) already on the stack.  Builds the frame (cookie-XORed EBP
//  chain, three callee-save pushes, a new fs:[0] SEH record whose handler
//  is __except_handler4) and returns to its caller's real body.
//----------------------------------------------------------------------------
__declspec(naked) void __SEH_prolog4()
{
    __asm
    {
        push    offset __except_handler4
        push    dword ptr fs:[0]
        mov     eax, [esp + 8 + 8]
        mov     [esp + 8 + 8], ebp
        lea     ebp, [esp + 8 + 8]
        sub     esp, eax
        push    ebx
        push    esi
        push    edi
        mov     eax, g_securityCookie
        xor     [ebp - 4], eax
        xor     eax, ebp
        push    eax
        mov     [ebp - 0x18], esp
        push    dword ptr[ebp - 8]
        mov     eax, [ebp - 4]
        mov     dword ptr[ebp - 4], 0FFFFFFFEh
        mov     [ebp - 8], eax
        lea     eax, [ebp - 0x10]
        mov     fs:[0], eax
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F7E1CC9 - the matching epilogue: restore fs:[0] from the frame's own
//  saved slot, pop the three callee-saves, tear the frame down and return
//  to the caller's caller (the `push ecx`/`retn` pair re-pushes the return
//  address the caller's own `call __SEH_epilog4` consumed, the standard
//  MSVC8 SEH-epilog tail-call trick).
//----------------------------------------------------------------------------
__declspec(naked) void __SEH_epilog4()
{
    __asm
    {
        mov     ecx, [ebp - 0x10]
        mov     fs:[0], ecx
        pop     ecx
        pop     edi
        pop     edi
        pop     esi
        pop     ebx
        mov     esp, ebp
        pop     ebp
        push    ecx
        retn
    }
}

//----------------------------------------------------------------------------
//  ?__ArrayUnwind@@YGXPAXIHP6EX0@Z@Z (0x6F7E1604) - destroy `count`
//  elements of an array under unwind (arg_0=array, arg_4=elementSize,
//  arg_8=count, arg_C=destructor).  A detached block after the main path
//  (reached only by the OS unwinder passing an EXCEPTION_POINTERS*, never
//  by ordinary control flow, matching the dump's own layout) tail-calls
//  terminate() if the in-flight exception is itself a C++ exception
//  (0E06D7363h, MSVC's "msc" exception code) reaching this a second time.
//----------------------------------------------------------------------------
void terminate();   // MSVC8 mangles plain `void terminate()` to ?terminate@@YAXXZ

__declspec(naked) void __ArrayUnwind()
{
    __asm
    {
        push    0x14
        push    offset stru_6FA4AAF0
        call    __SEH_prolog4
        and     dword ptr[ebp - 0x14], 0        // ms_exc.registration.TryLevel = 0
    retry:
        dec     dword ptr[ebp + 0x10]            // arg_8 (count) - single instruction,
        js      epilog                           // jumped back to at the loop's end below
        mov     ecx, [ebp + 8]                   // arg_0 (array)
        sub     ecx, [ebp + 0xC]                 // arg_4 (elementSize)
        mov     [ebp + 8], ecx
        call    dword ptr[ebp + 0x14]            // arg_C (destructor)
        jmp     retry
        //  Detached filter/handler block - the scope table's own target,
        //  reached only via the OS unwinder, not by fallthrough from the
        //  loop above (matching the dump's own address layout, where this
        //  block sits between the loop and the shared epilogue).
    filter:
        mov     eax, [ebp - 0x28]                // ms_exc.exc_ptr
        mov     [ebp - 0x1C], eax
        mov     eax, [ebp - 0x1C]
        mov     eax, [eax]
        mov     [ebp - 0x20], eax
        mov     eax, [ebp - 0x20]
        cmp     dword ptr[eax], 0E06D7363h
        jz      is_msc
        mov     dword ptr[ebp - 0x24], 0
        mov     eax, [ebp - 0x24]
        retn
    is_msc:
        call    terminate
        mov     esp, [ebp - 0x18]                // ms_exc.old_esp
    epilog:
        mov     dword ptr[ebp - 0x14], 0FFFFFFFEh   // TryLevel = -2
        call    __SEH_epilog4
        retn    0x10
    }
}

//----------------------------------------------------------------------------
//  ??_M@YGXPAXIHP6EX0@Z@Z (0x6F7E1662) - the `eh vector destructor
//  iterator` (arg_0=array end, arg_4=elementSize, arg_8=count,
//  arg_C=destructor): step backward destroying `count` elements of
//  `elementSize`-byte stride, tail-calling __ArrayUnwind to finish the job
//  if the destructor itself throws partway through (the `var_1C` flag,
//  0 on entry and set to 1 only once the loop finishes normally, is
//  AbnormalTermination()'s own storage - __ArrayUnwind only runs when it
//  is still 0, i.e. an exception cut the loop short).
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall EhVectorDestructorIterator(
    void* arrayEnd, unsigned int elementSize, unsigned int count,
    void(__thiscall* dtor)(void*))
{
    __asm
    {
        push    0x0C
        push    offset stru_6FA4AB10
        call    __SEH_prolog4
        and     dword ptr[ebp - 0x1C], 0
        mov     esi, [ebp + 0xC]                 // arg_4 (elementSize)
        mov     eax, esi
        imul    eax, [ebp + 0x10]                // arg_8 (count)
        add     [ebp + 8], eax                   // arg_0 (end) += size*count
        and     dword ptr[ebp - 0x14], 0         // TryLevel = 0
    retry:
        dec     dword ptr[ebp + 0x10]            // arg_8 (count) - single instruction,
        js      finish                           // jumped back to at the loop's end below
        sub     [ebp + 8], esi
        mov     ecx, [ebp + 8]
        call    dword ptr[ebp + 0x14]            // arg_C (destructor)
        jmp     retry
    finish:
        mov     dword ptr[ebp - 0x1C], 1
        mov     dword ptr[ebp - 0x14], 0FFFFFFFEh   // TryLevel = -2
        call    unwind_tail
        call    __SEH_epilog4
        retn    0x10
    unwind_tail:
        cmp     dword ptr[ebp - 0x1C], 0
        jnz     skip_unwind
        push    dword ptr[ebp + 0x14]
        push    dword ptr[ebp + 0x10]
        push    dword ptr[ebp + 0xC]
        push    dword ptr[ebp + 8]
        call    __ArrayUnwind
    skip_unwind:
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F7E16C5 - the `eh vector constructor iterator' (IDA prints this one
//  demangled; the mangled name is ??_L@YGXPAXIHP6EX0@Z1@Z).  arg_0=array
//  base, arg_4=elementSize, arg_8=count, arg_C=constructor, arg_10=the
//  destructor to unwind with.  Construct `count` elements forward; if one
//  of the constructors throws partway through, __ArrayUnwind destroys the
//  ones already built.  Same transcription rationale, same frame-slot
//  resolution and the same "nothing here ever calls it" caveat as the
//  destructor iterator above; `var_20` is this one's AbnormalTermination
//  flag and `var_1C` its running element index, which is also the count
//  __ArrayUnwind is handed.
//
//  Reached in this dump's tree from sub_6F6E30D0.  Under this build's fixed
//  /EHs-c- there is no exception model to protect, so no reconstruction here
//  will ever *emit* a call to it - see the CAbilityInventory::CAbilityInventory
//  entry in funcmap.py's BEHAVIOUR table for the same gap seen from the
//  caller's side.
//----------------------------------------------------------------------------
extern "C" char stru_6FA4AB30[];

__declspec(naked) void __stdcall EhVectorConstructorIterator(
    void* array, unsigned int elementSize, int count,
    void(__thiscall* ctor)(void*), void(__thiscall* dtor)(void*))
{
    __asm
    {
        push    0x10
        push    offset stru_6FA4AB30
        call    __SEH_prolog4
        xor     eax, eax
        mov     [ebp - 0x20], eax                // AbnormalTermination flag
        mov     [ebp - 0x14], eax                // TryLevel = 0
        mov     [ebp - 0x1C], eax                // i = 0
    top:
        mov     eax, [ebp - 0x1C]
        cmp     eax, [ebp + 0x10]                // arg_8 (count)
        jge     finish
        mov     esi, [ebp + 8]                   // arg_0 (cursor)
        mov     ecx, esi
        call    dword ptr[ebp + 0x14]            // arg_C (constructor)
        add     esi, [ebp + 0x0C]                // arg_4 (elementSize)
        mov     [ebp + 8], esi
        inc     dword ptr[ebp - 0x1C]
        jmp     top
    finish:
        mov     dword ptr[ebp - 0x20], 1
        mov     dword ptr[ebp - 0x14], 0FFFFFFFEh   // TryLevel = -2
        call    unwind_tail
        call    __SEH_epilog4
        retn    0x14
    unwind_tail:
        cmp     dword ptr[ebp - 0x20], 0
        jnz     skip_unwind
        push    dword ptr[ebp + 0x18]            // arg_10 (destructor)
        push    dword ptr[ebp - 0x1C]            // elements already built
        push    dword ptr[ebp + 0x0C]            // arg_4 (elementSize)
        push    dword ptr[ebp + 8]               // arg_0 (cursor)
        call    __ArrayUnwind
    skip_unwind:
        retn
    }
}
