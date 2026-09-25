//============================================================================
//  0x6F2C7460 - CSelectable::UpdateSelectionVisual: create the selection
//  visual on demand and attach or detach it.
//
//  Not a vtable slot: the helper CSelectable's slot 100
//  (selectable_slot100.cpp) calls with either the caller's own two flags or
//  a pair of zeros.  The object it owns is the +0x50 pointer selectable.h
//  already names.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "selectable.h"

//----------------------------------------------------------------------------
//  0x6F37B7C0 / 0x6F379A30 - the selection-visual object's own factory and
//  attach, in the 0x6F37xxxx rendering module.  Neither is in any dump this
//  repo has, so both are redirected.  The attach is a thiscall on the
//  *visual* with the selectable as its first stack argument, not the other
//  way round - the shipped call site loads the visual into ecx and pushes
//  `this`.
//
//  Its sibling 0x6F37BA90 is *not* redeclared here: it is already
//  ReleaseSelectionVisualObject (configgates.cpp), reached from
//  CSelectable::ReleaseSelectionVisual.  A second naked body at the same
//  address is the two-symbols-for-one-function defect
//  docs/notes/verifier-gate-link-failures.md warns about.
//----------------------------------------------------------------------------
struct SSelectionVisual
{
    void Attach(CSelectable* owner, int arg);
};

//  0x6F37BA90 - configgates.cpp.
void __fastcall ReleaseSelectionVisualObject(void* visual);

void* CreateSelectionVisual();

__declspec(naked) void* CreateSelectionVisual()
{
    __asm
    {
        mov     eax, 06F37B7C0h
        jmp     eax
    }
}

__declspec(naked) void SSelectionVisual::Attach(CSelectable*, int)
{
    __asm
    {
        mov     eax, 06F379A30h
        jmp     eax
    }
}

void CSelectable::UpdateSelectionVisual(int on, int arg)
{
    if (on)
    {
        //  Created lazily and then re-read rather than used from the call's
        //  own result: the shipped code stores the new pointer into +0x50
        //  and loads it straight back, which is what a separate `if` on the
        //  member gives and `if ((v = Create()) != 0)` does not.
        if (m_pSelectionVisual == 0)
            m_pSelectionVisual = CreateSelectionVisual();
        if (m_pSelectionVisual != 0)
            ((SSelectionVisual*)m_pSelectionVisual)->Attach(this, arg);
        return;
    }

    if (m_pSelectionVisual != 0)
        ReleaseSelectionVisualObject(m_pSelectionVisual);
}
