//============================================================================
//  0x6F37D0D0 and 0x6F37D260 - the two halves of the pause highlight: put
//  one widget into the highlighted set, and empty the set again.  Neither
//  calls the other.
//
//  Both hold the focus widget in a scoped SCheckedUnitSlot for the whole
//  body, which is what puts the `__except_handler4`-shaped frame this
//  toolchain cannot reproduce around each of them (docs/msvc-vc8-idioms.md).
//  Past that frame the bodies are what is below.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "gameuiagenthandle.h"
#include "gameuihighlight.h"
#include "agenttypedslots.h"
#include "playercolor.h"        // g_unk6FAB65F4

struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
};

//  0x6F2AD5D0 - Widget/widget_visibleflags.cpp.
//  0x6F2C7460 - Widget/selectable_selectionvisual.cpp.
class CSelectable
{
public:
    void UpdateSelectionVisual(int on, int arg);
};



//  The array the highlight set really is; only Append is reached here and
//  it is a redirect (gameuithunks.h).  Its destructor is
//  TSGrowableArray<CAgentPtr>::~TSGrowableArray, 0x6F286590, already
//  reconstructed as Agent/agentptrarray.cpp.
void __fastcall DestroyAgentPtrArray(void* array);

__declspec(naked) void __fastcall DestroyAgentPtrArray(void*)
{
    __asm
    {
        mov     eax, 06F286590h
        jmp     eax
    }
}

//  The widget as this pair reads it.  The highlight step is a *free*
//  __fastcall function and not a member, and that is the shipped ABI and not
//  a style choice: it cleans four bytes (`retn 4`) with two arguments, so
//  the widget arrives in ecx and the first flag in edx - which no
//  __thiscall member can express and which this compiler rejects
//  __thiscall on a free function for (docs/msvc-vc8-idioms.md).
//  tools/abi_audit.py's FATAL bucket is what caught the member spelling.
class CWidget
{
public:
    char         m_reserved000[0x20];
    unsigned int m_stateFlags;      // +0x20, bit 1 = registered
    char         m_reserved024[0x3C];
    unsigned char m_pinned;         // +0x60, bit 0
    char         m_reserved061[0x21F];
    unsigned int m_highlightFlags;  // +0x280

    //  0x6F2AD5D0 - Widget/widget_visibleflags.cpp.  Declared here under its
    //  real class, because MSVC puts the class name in the mangled name: a
    //  locally invented receiver type is a second symbol that never links
    //  (docs/notes/verifier-gate-link-failures.md, cause 1).
    int IsVisibleToLocalPlayer();
};

void __fastcall ApplyPauseHighlightToWidget(CWidget* widget, int checkAllies,
                                            int checkEnemies);

//  slot 0x13C - "is this widget suppressed"; slot 0x100 - a filtered
//  visibility query; slot 0xEC - the widget's owning player index.
typedef int (__thiscall *WidgetSlot0x13CFn)(void* self);
typedef int (__thiscall *WidgetSlot0x100Fn)(void* self, int a, int b);
typedef unsigned int (__thiscall *WidgetSlot0xECFn)(void* self);

enum { kHighlightApplied = 0x100, kHighlightSuppressed = 0x400 };

//----------------------------------------------------------------------------
//  0x6F37D0D0 - highlight one widget if it passes every one of the eight
//  gates, and remember it so the sweep below can undo it.
//----------------------------------------------------------------------------
void __fastcall ApplyPauseHighlightToWidget(CWidget* self, int checkAllies,
                                            int checkEnemies)
{
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

    if (world == 0)
        return;
    if (*(int*)((char*)world + 0x3E0) == 0)
        return;

    SCheckedUnitSlot focus((CAgent*)g_pFocusWidget);

    unsigned int localSlot = world->m_actingSlot;

    if (self != 0
        && IsAgentHandleLive(self) != 0
        && (self->m_highlightFlags & kHighlightApplied) == 0
        && (self->m_stateFlags & 2) != 0
        && (self->m_highlightFlags & kHighlightSuppressed) == 0
        && ((WidgetSlot0x13CFn)(*(void***)self)[0x13C / 4])(self) == 0
        && self->IsVisibleToLocalPlayer() == 0
        && ((WidgetSlot0x100Fn)(*(void***)self)[0x100 / 4])(self, 0, 4) != 0)
    {
        unsigned int owner =
            ((WidgetSlot0xECFn)(*(void***)self)[0xEC / 4])(self);

        int hit = 0;

        if (checkAllies != 0
            && (((CGameWorld*)world)->TestRelationBit(owner, (int)localSlot) != 0
                || (self->m_pinned & 1) != 0))
            hit = 1;

        if (hit == 0
            && checkEnemies != 0
            && ((CGameWorld*)world)->TestRelationBitClear(owner, (int)localSlot) != 0
            && (self->m_pinned & 1) == 0)
            hit = 1;

        if (hit != 0)
        {
            ((CSelectable*)self)->UpdateSelectionVisual(
                1, ((CAgent*)self == focus.m_value) ? 1 : 0);

            self->m_highlightFlags |= kHighlightApplied;

            ((TSGrowableArray<SCheckedUnitSlot>*)&g_highlightedWidgetAlloc)->New();
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F37D260 - take every widget out of the highlighted set again and drop
//  the set itself.
//----------------------------------------------------------------------------
void ClearAllPauseHighlights()
{
    SCheckedUnitSlot focus((CAgent*)g_pFocusWidget);

    for (unsigned int i = 0; i < g_highlightedWidgetCount; ++i)
    {
        CSelectable* widget = g_pHighlightedWidgets[i];

        if (widget == 0)
            continue;

        SItemHandleObject* agent = (SItemHandleObject*)
            LookupHandle(((SAgentHandlePair*)widget)->m_handle,
                         ((SAgentHandlePair*)widget)->m_handleType);
        if (agent == 0)
            continue;

        SItemHandleObject* checked =
            (agent->m_kindTag == kAgentTagPathable) ? agent : 0;
        if (checked == 0)
            continue;
        if (checked->m_pendingFlag != 0)
            continue;

        ((CWidget*)widget)->m_highlightFlags &= ~kHighlightApplied;

        int isFocus = ((CAgent*)widget == focus.m_value) ? 1 : 0;

        widget->UpdateSelectionVisual(isFocus, isFocus);
    }

    DestroyAgentPtrArray(&g_highlightedWidgetAlloc);

    g_highlightedWidgetAlloc = 0;
    g_highlightedWidgetCount = 0;
    g_pHighlightedWidgets = 0;
}
