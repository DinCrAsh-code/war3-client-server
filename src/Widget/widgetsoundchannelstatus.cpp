//============================================================================
//  0x6F30A3A0 - reached through CWidget::NegateAndForwardOffset's closure
//  from the same still out-of-scope enclosing function
//  (Widget/widgetsoundplayflags.cpp's own header note) as
//  PackSoundPlayFlags: the two call sites both do `mov eax, <value>` /
//  `call sub_6F30A3A0` with nothing else set up and the result left in
//  eax, right after a sound-channel-create call whose own return also
//  came back in eax.
//
//  That is the whole-program custom calling convention
//  docs/msvc-vc8-idioms.md's "Arguments in eax and esi" section documents:
//  no MSVC calling convention puts a parameter in eax, so this cannot be
//  declared to match its real callers exactly.  Following that section's
//  own precedent (CConfigFile::FindEntry/ValueAt) rather than reaching for
//  `__declspec(naked)`: declared `__fastcall` with the value nominally in
//  ecx (the closest a normal declaration gets), which reproduces every
//  instruction and constant of the body but not the register the shipped
//  callers actually use - the entry register is the only difference (`ret
//  0` happens to match either way), and calling this reconstruction from
//  any real, unhooked caller (which sets eax, never ecx) would read
//  whatever ecx happens to hold instead.  In tools/unhookable.py under
//  'NarrowSoundChannelStatus'.
//============================================================================

//----------------------------------------------------------------------------
//  0, 1 and 3 are the only values this remaps (to 0, 1 and 2); anything
//  else answers 4.  Read as a "channel/result kind" narrowing - three
//  meaningful sound-channel-create outcomes collapsed to a compact index,
//  with 4 as the catch-all - but the caller that would confirm that
//  reading is the enclosing, out-of-scope sound-play function.
//----------------------------------------------------------------------------
int __fastcall NarrowSoundChannelStatus(int status)
{
    switch (status)
    {
    case 0: return 0;
    case 1: return 1;
    case 3: return 2;
    default: return 4;
    }
}
