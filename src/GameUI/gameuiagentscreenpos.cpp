//============================================================================
//  0x6F334180 - ProjectAgentToScreen.
//
//  Where on the screen an agent's overhead marker belongs, and whether it is
//  visible at all.  Both outputs are pre-loaded with +infinity before
//  anything can fail, the same way ProjectWorldPointToScreen does it.
//
//  The body projects twice.  The first projection is for its *side effects*
//  only - it is what fills `out` and `outDepth` in from the agent's own
//  anchor position - and its answer is thrown away.  Then the agent's own
//  height (vtable slot 0xBC) is added and, if the camera anchor can see the
//  agent at all, the point is projected again half a height higher; if it
//  cannot, the whole thing answers 0 and leaves the first projection's
//  output in place.
//
//  The scoped SCheckedUnitSlot is what puts the `__except_handler4`-shaped
//  frame this toolchain cannot reproduce around the body
//  (docs/msvc-vc8-idioms.md).
//
//  Own translation unit: four real calls out of it, two of them
//  (0x6F333CB0, 0x6F333FB0) in this same shipped module.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "gameuiagenthandle.h"
#include "agenttypedslots.h"
#include "fvec3.h"

//  0x6F302950 - gameuispriteorigin.cpp.
struct SSpriteHost { float* __thiscall GetAttachmentOrigin(float* out); };

//: 0x7F800000 - positive infinity.
static const unsigned int kInfinityBits = 0x7F800000u;
#define kInfinity (*(const float*)&kInfinityBits)

//  slot 0xBC of an agent: how tall it is, in st0.
typedef float (__thiscall *AgentHeightFn)(void* self);

int __fastcall ProjectAgentToScreen(void* agent, float* out, float* outDepth)
{
    CGameUI* ui = (CGameUI*)GetGameUI(0, 0);

    out[0] = kInfinity;
    out[1] = kInfinity;

    if (outDepth != 0)
        *outDepth = kInfinity;

    if (agent == 0)
        return 0;

    SItemHandleObject* handle = (SItemHandleObject*)
        LookupHandle(((SAgentHandlePair*)agent)->m_handle,
                     ((SAgentHandlePair*)agent)->m_handleType);
    if (handle == 0)
        return 0;

    SItemHandleObject* checked =
        (handle->m_kindTag == kAgentTagPathable) ? handle : 0;
    if (checked == 0)
        return 0;
    if (checked->m_pendingFlag != 0)
        return 0;

    void* anchor = ui->GetCamera();

    if (anchor == 0)
        return 0;
    if (IsAgentHandleLive(anchor) == 0)
        return 0;

    FVec3 position;

    GetAgentAnchorPosition(&position.m_x, agent);

    ProjectWorldPointToScreen(out, outDepth,
                              position.m_x, position.m_y, position.m_z);

    float height = ((AgentHeightFn)(*(void***)agent)[0xBC / 4])(agent);

    SCheckedUnitSlot unit((CAgent*)agent);

    if (unit.m_value != 0)
    {
        FVec3 eye;

        ((SSpriteHost*)anchor)->GetAttachmentOrigin(&eye.m_x);

        float radius = kInfinity;

        if (QueryNearestObstruction(&eye.m_x, &position.m_x, &radius, 1) != 0)
        {
            FVec3 top;

            top.m_x = position.m_x;
            top.m_y = position.m_y;
            top.m_z = (float)(height * 0.5 + position.m_z);

            FVec3 delta;

            delta.m_x = top.m_x - eye.m_x;
            delta.m_y = top.m_y - eye.m_y;
            delta.m_z = top.m_z - eye.m_z;

            if (delta.Length() * 0.95 > radius)
                return 0;
        }
    }

    return ProjectWorldPointToScreen(out, outDepth,
                                     position.m_x, position.m_y,
                                     position.m_z + height);
}
