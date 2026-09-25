//============================================================================
//  0x6F333CB0 and 0x6F333FB0 - project a world point onto the screen, and
//  find the world point an agent should be projected from.  Neither calls
//  the other; the caller of both is 0x6F334180, one module along.
//
//  Every float constant in here was read out of the binary rather than
//  guessed - `ida_query bytes` on each of IDA's `flt_`/`dbl_` names:
//
//      flt_6FA7733C  0x7F800000  +infinity
//      dbl_6F9397E8  0.95
//      dbl_6F875120  0.5
//
//  +infinity is spelled through its bits because this build has no <math.h>
//  and no INFINITY; a literal cannot express it and `1.0f / 0.0f` is a
//  constant-folded division by zero.  Both sides canonicalise the operand to
//  an anonymous symbol either way, so the spelling costs nothing and the
//  value is right.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "gameuiagenthandle.h"
#include "agenttypedslots.h"
#include "fvec3.h"
#include "widget.h"   // CWidget::GetOffsetPosition (0x6F278190)

//  0x6F26DE50 - Unit/unit_visibilitymask.cpp.  Declared under its real
//  class: MSVC puts the class name in the mangled name, so a locally
//  invented receiver type is a second symbol that never links.
class CUnit { public: int QueryVisibleImpl(int which); };
//  0x6F26BA80 - Misc/trivialpredicates.cpp.
int __fastcall GetField0x28UnlessFlag_6F26BA80(const void* self);

//  0x6F302950 - gameuispriteorigin.cpp.
struct SSpriteHost { float* __thiscall GetAttachmentOrigin(float* out); };

//: 0x7F800000 - positive infinity, as the shipped constant pool holds it.
static const unsigned int kInfinityBits = 0x7F800000u;
#define kInfinity (*(const float*)&kInfinityBits)

//  slot 0x10 of the cursor's embedded +0xB4 sub-object: the screen
//  rectangle everything is clipped to.
typedef void (__thiscall *GetScreenRectFn)(void* self, float* out);
//  slot 0xE0 of an agent: its position, with the same four arguments
//  SPositionedWidget::GetOffsetPosition takes.
typedef float* (__thiscall *AgentGetPositionFn)(void* self, float* out,
                                                int a, int b, int c);

//----------------------------------------------------------------------------
//  0x6F333CB0 - project (x, y, z) onto the screen.
//
//  Answers whether the result landed inside the clip rectangle, writes the
//  two screen coordinates through `out`, and - when the caller supplied one
//  - the projected depth through `outDepth`.  `outDepth` is pre-loaded with
//  +infinity before anything can fail, so a caller that ignores the return
//  value still sees "infinitely far away" rather than a stale value.
//
//  The near-plane rejection is `length(delta) * 0.95 > radius`, and the
//  0.95 really is a double: the shipped `fmul` reads eight bytes.
//----------------------------------------------------------------------------
int __fastcall ProjectWorldPointToScreen(float* out, float* outDepth,
                                         float x, float y, float z)
{
    CGameUI* ui = (CGameUI*)GetGameUI(0, 0);

    if (ui == 0)
        return 0;

    //  The identity, then whatever the renderer's own view-projection is.
    //  Written as sixteen stores rather than an initialiser list because
    //  the shipped code writes them off two loaded constants - one `fld1`
    //  and one `fldz` - which is what a run of assignments gives.
    float matrix[16];

    matrix[0]  = 1.0f;
    matrix[1]  = 0.0f;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;
    matrix[4]  = 0.0f;
    matrix[5]  = 1.0f;
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;
    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    GetViewProjectionMatrix(matrix);

    float left, top, right, bottom, nearPlane, farPlane;

    GetViewportRect(&left, &right, &top, &bottom, &nearPlane, &farPlane);

    float clip[4];

    clip[0] = 0.0f;
    clip[1] = 0.0f;
    clip[2] = 0.0f;
    clip[3] = 0.0f;

    void* rectSource = (char*)ui->m_pWorldFrameWar3 + 0xB4;

    ((GetScreenRectFn)(*(void***)rectSource)[0x10 / 4])(rectSource, clip);

    void* anchor = ui->GetCamera();

    if (anchor == 0)
        return 0;

    SItemHandleObject* agent = (SItemHandleObject*)
        LookupHandle(((SAgentHandlePair*)anchor)->m_handle,
                     ((SAgentHandlePair*)anchor)->m_handleType);
    if (agent == 0)
        return 0;

    SItemHandleObject* checked =
        (agent->m_kindTag == kAgentTagPathable) ? agent : 0;
    if (checked == 0)
        return 0;
    if (checked->m_pendingFlag != 0)
        return 0;

    FVec3 eye;

    ((SSpriteHost*)anchor)->GetAttachmentOrigin(&eye.m_x);

    float radius = kInfinity;
    FVec3 delta;

    int inRange = QueryNearestObstruction(&eye.m_x, &x, &radius, 1);

    if (outDepth != 0)
        *outDepth = kInfinity;

    if (inRange != 0)
    {
        delta.m_x = x - eye.m_x;
        delta.m_y = y - eye.m_y;
        delta.m_z = z - eye.m_z;

        float limit = radius;

        if (delta.Length() * 0.95 > limit)
            return 0;
    }

    float vector[4];

    vector[0] = x;
    vector[1] = y;
    vector[2] = z;
    vector[3] = 1.0f;

    float* projected = TransformByMatrix(&delta.m_x, vector, matrix);

    vector[0] = projected[0];
    vector[1] = projected[1];
    vector[2] = projected[2];
    vector[3] = projected[3];

    float w = vector[3];

    vector[0] = vector[0] / w;
    vector[1] = vector[1] / w;
    vector[2] = vector[2] / w;
    vector[3] = w / w;

    if (outDepth != 0)
        *outDepth = vector[2];

    vector[0] = (float)((right + left) * 0.5 + (right - left) * vector[0] * 0.5);
    vector[1] = (float)((bottom + top) * 0.5 + (bottom - top) * vector[1] * 0.5);

    float screenX, screenY;

    ScaleToUiSpan(&screenX, &screenY, vector[0], vector[1]);

    out[0] = screenX;
    out[1] = screenY;

    return ((SScreenRect*)clip)->Contains(out);
}

//----------------------------------------------------------------------------
//  0x6F333FB0 - the world point an agent should be projected from.
//
//  Four sources, tried in order: a unit that is either visible or carries
//  the +0x248 bit 0x200 goes through its own +0x28 sub-object; a unit that
//  is neither uses the widget-offset position; anything that is not a unit
//  at all goes through its own vtable slot 0xE0; and an agent whose handle
//  no longer resolves is left at +infinity on all three axes.
//
//  The scoped SCheckedUnitSlot is what puts the `__except_handler4`-shaped
//  frame this toolchain cannot reproduce around the body.
//----------------------------------------------------------------------------
float* __fastcall GetAgentAnchorPosition(float* out, void* agent)
{
    float position[3];

    position[0] = kInfinity;
    position[1] = kInfinity;
    position[2] = kInfinity;

    if (agent != 0)
    {
        SItemHandleObject* handle = (SItemHandleObject*)
            LookupHandle(((SAgentHandlePair*)agent)->m_handle,
                         ((SAgentHandlePair*)agent)->m_handleType);

        SItemHandleObject* checked =
            (handle != 0 && handle->m_kindTag == kAgentTagPathable)
                ? handle : 0;

        if (checked != 0 && checked->m_pendingFlag == 0)
        {
            SCheckedUnitSlot unit((CAgent*)agent);

            float* source;

            if (unit.m_value != 0)
            {
                if (((CUnit*)unit.m_value)->QueryVisibleImpl(1) != 0
                    || (*(unsigned int*)((char*)unit.m_value + 0x248) & 0x200) != 0)
                {
                    void* sub = (void*)GetField0x28UnlessFlag_6F26BA80(unit.m_value);

                    if (sub != 0)
                        GetAgentWorldPosition(sub, position);

                    out[0] = position[0];
                    out[1] = position[1];
                    out[2] = position[2];
                    return out;
                }

                source = (float*)((CWidget*)unit.m_value)
                             ->GetOffsetPosition((FVec3*)position, 1, -1, 0);
            }
            else
            {
                source = ((AgentGetPositionFn)(*(void***)agent)[0xE0 / 4])(
                    agent, position, 1, -1, 0);
            }

            position[0] = source[0];
            position[1] = source[1];
            position[2] = source[2];

            out[0] = position[0];
            out[1] = position[1];
            out[2] = position[2];
            return out;
        }
    }

    out[0] = position[0];
    out[1] = position[1];
    out[2] = position[2];

    return out;
}
