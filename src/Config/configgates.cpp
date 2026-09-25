//============================================================================
//  Redirects to functions this session's three vtables reach but does not
//  reconstruct - the same `mov eax, <address>` / `jmp eax` shape item.cpp's
//  QueryGroundHeightHelper and itemgroundlookup.cpp already use, and for
//  the same reason: the call sites match exactly, and the redirect keeps
//  the live build calling the real code rather than a stub that lies.
//
//  Each one is out of scope for a *stated* reason, not because it was hard;
//  none of them is widget/item logic.  Every one of them is registered in
//  tools/funcmap.py with a DIFFERS verdict saying so, so nothing here can
//  masquerade as a reconstruction.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F009E30 - a second copy of GetConfigFloat's body (already
//  reconstructed at 0x6F009EE0, configfloat.cpp) that hands its answer back
//  in st0 as a hardware float instead of as a CFloat.  Faithfully
//  reproducing it needs hardware-float-returning declarations of
//  CMiscCustom::GetFloat and CConfigFile::GetFloatValue at the same two
//  addresses those CFloat-returning declarations already occupy, which is
//  the exact two-symbols-for-one-address collision
//  docs/notes/verifier-gate-link-failures.md warns about.
__declspec(naked) float __fastcall GetConfigFloatHW(const char* section,
                                                     const char* key,
                                                     int index)
{
    __asm
    {
        mov     eax, 06F009E30h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F00DB60 - the last of the three visual-attachment gates still
//  redirected (0x6F00D9E0 is now reconstructed in attachmentsetvisible.cpp
//  and 0x6F00D970 in stormattachmentgates.cpp)
//  CSelectable's slots 13/105/106 poke: a selection-circle decal and a
//  walkable-terrain Z marker, keyed by the two attachment slot indices at
//  +0x44/+0x48.  selectable.h has flagged these as out of scope since
//  CItem__Method_0x1A0 (docs/targets/CItem__Method_0x1A0.md) - rendering
//  bookkeeping, not simulation state.
__declspec(naked) void __fastcall AttachmentSetScale(unsigned int slot,
                                                      const float* scaleXY)
{
    __asm
    {
        mov     eax, 06F00DB60h
        jmp     eax
    }
}
