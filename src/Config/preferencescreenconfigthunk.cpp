//============================================================================
//  0x6F00BC20 - thunked rather than reconstructed this session.  207
//  instructions: a GPU-vendor-and-memory-tier detection pass (CPUID
//  feature bits, GlobalMemoryStatus, a 15-record capability table walked
//  twice under two different key shapes) feeding fourteen
//  CPreferencesWar3::StoreValue calls (Config/preferencestore.cpp, already
//  reconstructed) through a `mov ecx,esi` receiver this call tree never
//  otherwise names - i.e. the *shape* of every individual call is
//  reachable, but the two 15-record table walks (dword_6FA4F868/6F868A/...,
//  a `word`-keyed vendor/device match against a fixed `word_6FA4F868[eax]`
//  array first exact then by nearest-below-threshold) are unique to this
//  function and not shared with anything already reconstructed elsewhere
//  in this repo, so getting the walk's own comparison order and rounding
//  exactly right without a live reference was judged not worth the risk of
//  a wrong StoreValue call landing at a real hook.  A genuine attempt, not
//  a shortcut past tedium - see CLAUDE.md's own line on that.
//============================================================================

//  retn 0, no arguments - this call tree never shows a caller for it (its
//  own dump ends in a bare `retn`), consistent with it being reached only
//  from outside this dump's own depth cut.
__declspec(naked) void DetectScreenConfigThunk()
{
    __asm { mov eax, 06F00BC20h }
    __asm { jmp eax }
}
