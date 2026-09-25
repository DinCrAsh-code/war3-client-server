//============================================================================
//  0x6F501D50 - reached from CWidget::Save.  NOT a decode bug: a prior
//  session flagged this address as landing "in the middle of an unrelated
//  Concurrency::SchedulerBase symbol" and asked whether the address itself
//  was even right.  It is.  The dump's own last instruction really is
//  `jmp loc_6F513980`, and 0x6F513980 really is the byte IDA labels
//  `Concurrency::details::SchedulerBase::GetNumberOfBoundContexts` - not
//  that function's entry point, but the middle of it (its own
//  `test byte ptr [ecx+54h], 80h` flag check).
//
//  That is the shipped compiler/linker's own code layout, not ours to
//  reproduce: this function's tail was placed to fall straight into a
//  byte range that belongs to the C++ Concurrency Runtime's own compiled
//  object code (libconcrt), which this repo does not compile and cannot
//  `goto` into the middle of - there is no C++ spelling that jumps from
//  one translation unit into an arbitrary instruction offset inside
//  another, already-linked one.  Left a redirect to 0x6F501D50 itself
//  (not to the Concurrency Runtime address it tail-jumps to) for exactly
//  that reason - same shape as every other "below this dump's depth cut"
//  thunk in this closure, except here the reason is architectural rather
//  than a missing dump.
//============================================================================
struct SWidgetSchedulerPick
{
    //  `this` in ecx, no stack arguments (`retn`, i.e. `retn 0`).
    int Method(void);
};

__declspec(naked) int SWidgetSchedulerPick::Method(void)
{
    __asm
    {
        mov     eax, 06F501D50h
        jmp     eax
    }
}
