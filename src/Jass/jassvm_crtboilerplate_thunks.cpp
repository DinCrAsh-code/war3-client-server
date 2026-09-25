//============================================================================
//  jassvm-sweep-K1 finding: a large share of this session's assigned
//  0x6F448000-0x6F44FB00 range is not JASS code at all - it is statically
//  linked Visual C++ runtime/Concurrency-runtime template plumbing the
//  linker laid out in the same image neighbourhood as Jass.cpp.  Every
//  address below carries a FLIRT-matched name (or is byte-identical to a
//  FLIRT-matched sibling) for `Concurrency::details::_ReaderWriterLock`'s
//  constructor or `StructuredWorkStealingQueue<..>::Reinitialize`, and none
//  has a static xref to or from anything in this session's own worktree -
//  not JASS's own code, just co-located boilerplate.
//
//  Per CLAUDE.md's own efficiency rule ("mark THUNK immediately... for pure
//  CRT/MSVC internals"): each gets a naked mov/jmp redirect to its real,
//  unhooked body instead of a from-scratch reconstruction, since there is no
//  JASS behaviour here to reconstruct.  Names are shape-based, not semantic,
//  and say so.
//============================================================================

//----------------------------------------------------------------------------
//  `_ReaderWriterLock` constructor shape (mov eax,ecx; xor ecx,ecx; three
//  ecx-relative zero stores; retn 0).  12 keep FLIRT's own
//  `??0_ReaderWriterLock@details@Concurrency@@QAE@XZ_10N` name; the other 12
//  are byte-identical `unknown_libname_NNN` siblings FLIRT left unresolved.
//----------------------------------------------------------------------------
//  0x6F44A2E0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_1(void*)
{
    __asm
    {
        mov eax, 06F44A2E0h
        jmp eax
    }
}

//  0x6F44A540
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_2(void*)
{
    __asm
    {
        mov eax, 06F44A540h
        jmp eax
    }
}

//  0x6F44A620
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_3(void*)
{
    __asm
    {
        mov eax, 06F44A620h
        jmp eax
    }
}

//  0x6F44A670
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_4(void*)
{
    __asm
    {
        mov eax, 06F44A670h
        jmp eax
    }
}

//  0x6F44A790
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_5(void*)
{
    __asm
    {
        mov eax, 06F44A790h
        jmp eax
    }
}

//  0x6F44AF20
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_6(void*)
{
    __asm
    {
        mov eax, 06F44AF20h
        jmp eax
    }
}

//  0x6F44AF50
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_7(void*)
{
    __asm
    {
        mov eax, 06F44AF50h
        jmp eax
    }
}

//  0x6F44AF60
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_8(void*)
{
    __asm
    {
        mov eax, 06F44AF60h
        jmp eax
    }
}

//  0x6F44AF70
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_9(void*)
{
    __asm
    {
        mov eax, 06F44AF70h
        jmp eax
    }
}

//  0x6F44AF80
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_10(void*)
{
    __asm
    {
        mov eax, 06F44AF80h
        jmp eax
    }
}

//  0x6F44AFB0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_11(void*)
{
    __asm
    {
        mov eax, 06F44AFB0h
        jmp eax
    }
}

//  0x6F44AFC0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_12(void*)
{
    __asm
    {
        mov eax, 06F44AFC0h
        jmp eax
    }
}

//  0x6F44A130
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_13(void*)
{
    __asm
    {
        mov eax, 06F44A130h
        jmp eax
    }
}

//  0x6F44A190
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_14(void*)
{
    __asm
    {
        mov eax, 06F44A190h
        jmp eax
    }
}

//  0x6F44A1D0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_15(void*)
{
    __asm
    {
        mov eax, 06F44A1D0h
        jmp eax
    }
}

//  0x6F44A460
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_16(void*)
{
    __asm
    {
        mov eax, 06F44A460h
        jmp eax
    }
}

//  0x6F44A710
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_17(void*)
{
    __asm
    {
        mov eax, 06F44A710h
        jmp eax
    }
}

//  0x6F44A8C0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_18(void*)
{
    __asm
    {
        mov eax, 06F44A8C0h
        jmp eax
    }
}

//  0x6F44A9C0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_19(void*)
{
    __asm
    {
        mov eax, 06F44A9C0h
        jmp eax
    }
}

//  0x6F44AAB0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_20(void*)
{
    __asm
    {
        mov eax, 06F44AAB0h
        jmp eax
    }
}

//  0x6F44AB90
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_21(void*)
{
    __asm
    {
        mov eax, 06F44AB90h
        jmp eax
    }
}

//  0x6F44AC40
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_22(void*)
{
    __asm
    {
        mov eax, 06F44AC40h
        jmp eax
    }
}

//  0x6F44ACF0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_23(void*)
{
    __asm
    {
        mov eax, 06F44ACF0h
        jmp eax
    }
}

//  0x6F44ADA0
__declspec(naked) void __fastcall ConcurrencyReaderWriterLockCtor_24(void*)
{
    __asm
    {
        mov eax, 06F44ADA0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  `StructuredWorkStealingQueue<_UnrealizedChore,_CriticalNonReentrantLock>
//  ::Reinitialize` shape (xor eax,eax; three ecx-relative zero stores using
//  eax as the zero source; retn 0).  12 keep FLIRT's name, 5 are
//  byte-identical `unknown_libname_NNN` siblings.
//----------------------------------------------------------------------------
//  0x6F448CE0
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_1(void*)
{
    __asm
    {
        mov eax, 06F448CE0h
        jmp eax
    }
}

//  0x6F449090
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_2(void*)
{
    __asm
    {
        mov eax, 06F449090h
        jmp eax
    }
}

//  0x6F449180
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_3(void*)
{
    __asm
    {
        mov eax, 06F449180h
        jmp eax
    }
}

//  0x6F449220
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_4(void*)
{
    __asm
    {
        mov eax, 06F449220h
        jmp eax
    }
}

//  0x6F449540
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_5(void*)
{
    __asm
    {
        mov eax, 06F449540h
        jmp eax
    }
}

//  0x6F449820
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_6(void*)
{
    __asm
    {
        mov eax, 06F449820h
        jmp eax
    }
}

//  0x6F449840
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_7(void*)
{
    __asm
    {
        mov eax, 06F449840h
        jmp eax
    }
}

//  0x6F449850
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_8(void*)
{
    __asm
    {
        mov eax, 06F449850h
        jmp eax
    }
}

//  0x6F449860
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_9(void*)
{
    __asm
    {
        mov eax, 06F449860h
        jmp eax
    }
}

//  0x6F449870
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_10(void*)
{
    __asm
    {
        mov eax, 06F449870h
        jmp eax
    }
}

//  0x6F449890
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_11(void*)
{
    __asm
    {
        mov eax, 06F449890h
        jmp eax
    }
}

//  0x6F4498A0
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_12(void*)
{
    __asm
    {
        mov eax, 06F4498A0h
        jmp eax
    }
}

//  0x6F448BF0
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_13(void*)
{
    __asm
    {
        mov eax, 06F448BF0h
        jmp eax
    }
}

//  0x6F448F30
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_14(void*)
{
    __asm
    {
        mov eax, 06F448F30h
        jmp eax
    }
}

//  0x6F4490C0
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_15(void*)
{
    __asm
    {
        mov eax, 06F4490C0h
        jmp eax
    }
}

//  0x6F449360
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_16(void*)
{
    __asm
    {
        mov eax, 06F449360h
        jmp eax
    }
}

//  0x6F449630
__declspec(naked) void __fastcall ConcurrencyWorkStealingQueueReinit_17(void*)
{
    __asm
    {
        mov eax, 06F449630h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  `mov eax,4; retn` - four addresses FLIRT calls `__RTC_NumErrors`, a real
//  CRT symbol that does not actually return a fixed literal 4 - a
//  byte-identical coincidence with a trivial constant-return shape used
//  elsewhere in this range too, not four real copies of the RTC counter.
//----------------------------------------------------------------------------
//  0x6F4488B0
__declspec(naked) int __fastcall CrtDuplicateConstFour_1(void)
{
    __asm
    {
        mov eax, 06F4488B0h
        jmp eax
    }
}

//  0x6F449830
__declspec(naked) int __fastcall CrtDuplicateConstFour_2(void)
{
    __asm
    {
        mov eax, 06F449830h
        jmp eax
    }
}

//  0x6F449880
__declspec(naked) int __fastcall CrtDuplicateConstFour_3(void)
{
    __asm
    {
        mov eax, 06F449880h
        jmp eax
    }
}

//  0x6F4498B0
__declspec(naked) int __fastcall CrtDuplicateConstFour_4(void)
{
    __asm
    {
        mov eax, 06F4498B0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  Two one-off stubs, unresolved by FLIRT, whose byte shapes are not shared
//  by anything else named in this range.
//----------------------------------------------------------------------------
//  0x6F448490 - retn 0, one ecx-relative zero store
__declspec(naked) void __fastcall CrtDuplicateZeroWord_1(void*)
{
    __asm
    {
        mov eax, 06F448490h
        jmp eax
    }
}

//  0x6F4484A0 - retn 4, one stack arg copied into [ecx]
__declspec(naked) void __fastcall CrtDuplicateSetWord_1(void* obj, void* unused, void* value)
{
    __asm
    {
        mov eax, 06F4484A0h
        jmp eax
    }
}
