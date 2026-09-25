//============================================================================
//  0x6F2C74B0 - CSelectable::RefreshSelectionVisual.
//
//  Five instructions: read the selection visual out of +0x50 and, if there
//  is one, tail-jump into its own placement update.  The `jmp` is MSVC's
//  tail call on a `void` call in tail position, not something the source
//  spells.
//
//  Own translation unit: 0x6F37BAB0 is a real call out of it, and the two
//  are different shipped modules.
//============================================================================
#include "gameui.h"

//----------------------------------------------------------------------------
//  Declared locally rather than by including Widget/selectable.h, the way
//  every other call site of this class in this repo already does: MSVC puts
//  only the class *name* in a member's mangled name, and +0x50 is the same
//  m_pSelectionVisual that file names.
//----------------------------------------------------------------------------
class CSelectable
{
public:
    char              m_reserved00[0x50];
    SSelectionVisual* m_pSelectionVisual;   // +0x50

    void RefreshSelectionVisual();
};

void CSelectable::RefreshSelectionVisual()
{
    SSelectionVisual* visual = m_pSelectionVisual;

    if (visual)
        visual->UpdatePlacement();
}
