//============================================================================
//  0x6F381670 - UpdatePauseHighlights.
//
//  Clear whatever is highlighted, then, if anything is still to be
//  highlighted at all, walk the cursor's own tracked-widget list and offer
//  every entry to the per-widget gate.
//
//  The two flags are tested as `showAllies == 0 && showEnemies == 0` and not
//  as one `||`, because the shipped code jumps into the body on the first
//  one being set and only reads the second when it is clear.
//
//  Own translation unit: four real calls out of it, two of them in other
//  modules of the same subsystem.
//============================================================================
#include "gameui.h"
#include "playercolor.h"        // g_unk6FAB65F4

//  0x6F37D0D0 - gameuihighlightsweep.cpp.  A free __fastcall function and
//  not a member: the shipped `retn 4` with two arguments puts the widget in
//  ecx and the first flag in edx, which no __thiscall member can express.
class CWidget;
void __fastcall ApplyPauseHighlightToWidget(CWidget* widget, int checkAllies,
                                            int checkEnemies);

//  The cursor's tracked-widget list: a count and an element pointer.
struct STrackedWidgetList
{
    char          m_reserved00[0x04];
    unsigned int  m_count;          // +0x04
    CWidget**     m_pItems;         // +0x08
};

void __fastcall UpdatePauseHighlights(int active, int showAllies,
                                      int showEnemies)
{
    void* world = g_unk6FAB65F4;

    if (world == 0)
        return;
    if (*(int*)((char*)world + 0x3E0) == 0)
        return;

    ClearAllPauseHighlights();

    if (active == 0)
        return;
    if (showAllies == 0 && showEnemies == 0)
        return;

    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);

    STrackedWidgetList* list =
        (STrackedWidgetList*)ui->m_pWorldFrameWar3->GetTrackedWidgets();

    unsigned int count = list->m_count;

    for (unsigned int i = 0; i < count; ++i)
        ApplyPauseHighlightToWidget(list->m_pItems[i], showAllies,
                                    showEnemies);
}
