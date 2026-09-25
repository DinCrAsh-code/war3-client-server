//============================================================================
//  0x6F332700 - "somebody's selection changed, redraw the overlay".
//
//  Was a `mov eax,<address> / jmp eax` redirect; it is eleven instructions
//  of singleton fetch and one field poke, and the singleton it fetches
//  (GetGameUI, 0x6F300710) is already declared in this repo.
//
//  __fastcall with a single argument: the mask to OR into the UI object's
//  own +0x284 "what needs redrawing" word.  CItem's slot 102
//  (item_selectionrefresh.cpp) always passes 0, which still trips the
//  unconditional `+0x26C = 1` dirty flag the callee sets - that flag, not
//  the mask, is what makes this a refresh.
//
//  GetGameUI is asked with (0, 0): do not create it, do not clear the
//  cached pointer.  Its answer really can be null - this runs before the UI
//  exists during load - which is what the null test is for.
//
//  Own translation unit: two real calls out of it, and its own caller must
//  not inline it.
//============================================================================

//  0x6F300710 - the lazily-constructed CGameUI singleton.  Behind an
//  __except_handler4-shaped SEH frame this toolchain cannot reproduce, so
//  it stays a redirect (selectable_attachcreate.cpp); declared here, not
//  re-thunked - a second naked body at the same address is the
//  two-symbols-for-one-function defect docs/notes/verifier-gate-link-
//  failures.md warns about.
struct SGameUI
{
    char          m_reserved00[0x2A0];
    unsigned int  m_uiUp;           // +0x2A0
    unsigned int  m_circlesLit;     // +0x2A4
};
SGameUI* __fastcall GetGameUI(int create, int reset);

//  0x6F2F3220 - already reconstructed as
//  SField284Owner::OrField284SetField26C (misc_trivial_getters.cpp).
struct SField284Owner
{
    void __thiscall OrField284SetField26C(unsigned int mask);
};

void __fastcall RefreshSelectionOverlay(int mask)
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SField284Owner*)ui)->OrField284SetField26C((unsigned int)mask);
}
