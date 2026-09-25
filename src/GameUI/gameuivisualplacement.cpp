//============================================================================
//  0x6F37BAB0 - SSelectionVisual::UpdatePlacement.
//
//  Put the unit tip where the selected widget currently is, or take it down.
//  The receiver is the same `SSelectionVisual` Widget/selectionvisualrelease.cpp
//  already named: it holds the widget at +0x08 and the tip frame it drives
//  at +0x0C.
//
//  Eight guards have to pass before anything is placed, and every one of
//  them leaves through the *same* two exits - `SSelectionVisual::Detach(0)`
//  for a widget that is simply not showing, and
//  ReleaseSelectionVisualObject for one whose handle no longer resolves at
//  all.  Written as one labelled exit each, for the reason
//  docs/msvc-vc8-idioms.md gives under "Shared exits need `goto`".
//
//  The middle of the body is a box clamp: the tip's own width and height are
//  asked of its two frames (the widget tip at +0x0C and, when the widget is
//  the focus one, the shared CUnitTip as well), the clip rectangle is inset
//  by half a width and a whole height, and the projected screen position is
//  clamped into what is left.  The full-height inset on the y axis and the
//  half-width one on x are the shipped code's, not a transcription slip -
//  the `fmul` by 0.5 happens once, on the width only.
//
//  Two scoped smart pointers put the `__except_handler4`-shaped frame this
//  toolchain cannot reproduce around the whole thing.
//
//  Own translation unit: nine real calls out of it.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "gameuiagenthandle.h"
#include "gameuihighlight.h"
#include "agenttypedslots.h"
#include "playercolor.h"        // g_unk6FAB65F4

//  0x6F37BA90 - Widget/selectionvisualrelease.cpp, under the same name so
//  both sides reach the one symbol.
void __fastcall ReleaseSelectionVisualObject(void* visual);

//  0x6F53F160 - Game/gamemode.cpp.
int __fastcall IsGameModeOne();
//  0x6F2AD5D0 and 0x6F26DE50 - Widget/widget_visibleflags.cpp and
//  Unit/unit_visibilitymask.cpp.  Declared under their real classes: MSVC
//  puts the class name in the mangled name, so a locally invented receiver
//  type is a second symbol that never links.
class CWidget { public: int IsVisibleToLocalPlayer(); };
class CUnit   { public: int QueryVisibleImpl(int which); };

//  Widget vtable slot 0x100 - a filtered visibility query.
typedef int (__thiscall *WidgetSlot0x100Fn)(void* self, int a, int b);
//  Tip-frame vtable slots: 0x18 width, 0x1C height, 0x6C refresh,
//  0x74 / 0x78 "show this subject".
typedef float (__thiscall *FrameMeasureFn)(void* self);
typedef void  (__thiscall *FrameRefreshFn)(void* self);
typedef void  (__thiscall *FrameSetSubjectFn)(void* self, void* subject);
//  slot 0x10 of the cursor's embedded +0xB4 sub-object.
typedef void  (__thiscall *GetScreenRectFn)(void* self, float* out);

//  The tip frame this visual drives.
struct SVisualTipFrame
{
    char m_reserved00[0x94];
    int  m_active;              // +0x94
};

struct SSelectionVisualBody
{
    char             m_reserved00[0x08];
    void*            m_pWidget;     // +0x08
    SVisualTipFrame* m_pTip;        // +0x0C
};

enum { kWidgetPinned = 1, kWidgetTipFlag = 8 };

void SSelectionVisual::UpdatePlacement()
{
    SSelectionVisualBody* self = (SSelectionVisualBody*)this;

    void* widget = self->m_pWidget;

    if (widget == 0)
        goto release;

    {
        SItemHandleObject* handle = (SItemHandleObject*)
            LookupHandle(((SAgentHandlePair*)widget)->m_handle,
                         ((SAgentHandlePair*)widget)->m_handleType);
        if (handle == 0)
            goto release;

        SItemHandleObject* checked =
            (handle->m_kindTag == kAgentTagPathable) ? handle : 0;
        if (checked == 0)
            goto release;
        if (checked->m_pendingFlag != 0)
            goto release;
        if (self->m_pTip == 0)
            goto release;
    }

    {
        CGameUI* ui = (CGameUI*)GetGameUI(0, 0);

        if (ui == 0)
            return;

        if (((CWidget*)widget)->IsVisibleToLocalPlayer() != 0
            && IsGameModeOne() != 0
            && (*(unsigned char*)(*(char**)((char*)g_unk6FAB65F4 + 0x34) + 0x24) & 1) == 0)
            goto detachOnly;

        if (((WidgetSlot0x100Fn)(*(void***)widget)[0x100 / 4])(widget, 0, 4) == 0)
            goto detachOnly;
        if (IsWorldViewReady() == 0)
            goto detachOnly;
        if (ui->m_pauseRequested != 0)
            goto detachOnly;
        if (ui->m_field29C == 0)
            goto detachOnly;

        {
            SCheckedUnitSlot unit((CAgent*)self->m_pWidget);

            SCheckedDestructableSlot agent;
            agent.Construct((CAgent*)self->m_pWidget);

            CUnitTip* tip = 0;

            if (g_pFocusWidget == (CSelectable*)self->m_pWidget)
                tip = AcquireUnitTip();

            //  One flag and not a `goto`: the shipped code really does leave
            //  every one of these edges through the *same* Detach call and
            //  then through the one scope-exit that releases both slots, and
            //  a `goto` past two live objects is not something C++ allows.
            int detachNeeded = 0;

            if (unit.m_value != 0
                && (((WidgetSlot0x100Fn)(*(void***)unit.m_value)[0x13C / 4])(
                        unit.m_value, 0, 0) != 0
                    || (*(unsigned char*)((char*)unit.m_value + 0x20)
                        & kWidgetPinned) != 0))
            {
                detachNeeded = 1;
            }
            else
            {
                float position[2];
                float depth;

                position[0] = 0.0f;
                position[1] = 0.0f;
                depth = 0.0f;

                if (IsWorldViewReady() == 0
                    || ProjectAgentToScreen(widget, position, &depth) == 0)
                {
                    detachNeeded = 1;
                }
                else
                {
                    float clip[4];

                    clip[0] = 0.0f;
                    clip[1] = 0.0f;
                    clip[2] = 0.0f;
                    clip[3] = 0.0f;

                    void* rectSource = (char*)ui->m_pWorldFrameWar3 + 0xB4;

                    ((GetScreenRectFn)(*(void***)rectSource)[0x10 / 4])(
                        rectSource, clip);

                    float width = 0.0f;
                    float height = 0.0f;

                    if (self->m_pTip->m_active == 0)
                    {
                        if (unit.m_value == 0
                            || (*(unsigned char*)((char*)unit.m_value + 0x20)
                                & kWidgetTipFlag) == 0
                            || ((CUnit*)unit.m_value)
                                   ->QueryVisibleImpl(1) == 0)
                            ((CSimpleFrame*)self->m_pTip)->Hide();
                    }

                    if (self->m_pTip->m_active != 0)
                    {
                        width = ((FrameMeasureFn)
                                 (*(void***)self->m_pTip)[0x18 / 4])(
                                    self->m_pTip);
                        height = (float)(((FrameMeasureFn)
                                  (*(void***)self->m_pTip)[0x1C / 4])(
                                     self->m_pTip) + 0.0);
                    }

                    if (tip != 0 && tip->m_shown != 0)
                    {
                        ((FrameRefreshFn)(*(void***)tip)[0x6C / 4])(tip);

                        float tipWidth =
                            ((FrameMeasureFn)(*(void***)tip)[0x18 / 4])(tip);

                        if (tipWidth > width)
                            width = tipWidth;

                        height = ((FrameMeasureFn)
                                  (*(void***)tip)[0x1C / 4])(tip) + height;
                    }

                    width = (float)(width * 0.5);

                    //  Inset the clip rectangle by the tip's own half-width
                    //  on x and by its *whole* height on y - the `fmul` by
                    //  0.5 happens once, on the width only - and clamp the
                    //  projected point into what is left.
                    clip[1] = clip[1] + width < clip[3] ? clip[1] + width
                                                        : clip[3];
                    clip[3] = clip[1] > clip[3] - width ? clip[1]
                                                        : clip[3] - width;

                    clip[0] = clip[0] + height < clip[2] ? clip[0] + height
                                                         : clip[2];
                    clip[2] = clip[0] > clip[2] - height ? clip[0]
                                                         : clip[2] - height;

                    if (position[0] < clip[1])
                        position[0] = clip[1];
                    if (position[0] > clip[3])
                        position[0] = clip[3];

                    if (position[1] < clip[0])
                        position[1] = clip[0];
                    if (position[1] > clip[2])
                        position[1] = clip[2];

                    if (self->m_pTip->m_active != 0)
                    {
                        if (unit.m_value != 0)
                            ((FrameSetSubjectFn)
                             (*(void***)self->m_pTip)[0x74 / 4])(
                                self->m_pTip, unit.m_value);
                        else if (agent.m_value != 0)
                            ((FrameSetSubjectFn)
                             (*(void***)self->m_pTip)[0x78 / 4])(
                                self->m_pTip, agent.m_value);

                        ((CSimpleFrame*)self->m_pTip)->SetPoint(
                            1, position[0], position[1], 1);
                        ((CSimpleFrame*)self->m_pTip)->QueueForLayout(1);
                        ((CSimpleFrame*)self->m_pTip)->Hide();

                        if (tip != 0)
                            ((CSimpleFrame*)tip)->SetPointTo(
                                7, (CLayoutFrame*)self->m_pTip, 1,
                                0.0f, 0.002f, 1);
                    }
                    else if (tip != 0)
                    {
                        ((CSimpleFrame*)tip)->SetPoint(7, position[0],
                                                       position[1], 1);
                    }
                }
            }

            if (detachNeeded != 0)
                Detach(0);

            //  The agent slot's own release, inlined here the way the
            //  shipped scope exit has it; the unit slot's destructor does
            //  the same for its own.
            if (agent.m_value != 0)
            {
                CAgent* held = (CAgent*)agent.m_value;
                if (--held->m_refcount == 0)
                    held->ReleaseSelf();
            }

            return;
        }
    }

detachOnly:
    Detach(0);
    return;

release:
    ReleaseSelectionVisualObject(this);
}
