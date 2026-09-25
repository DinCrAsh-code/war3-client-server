//============================================================================
//  0x6F307140 - CCameraWar3::PanTo: the whole of what
//  JASS_PanCameraToTimedWithZ asks for, and the root of this target under
//  the native itself.
//
//  Four stages:
//
//   1. drop the previous pan job, zero the pan offset and put the
//      controller back into mode 0;
//   2. project the requested (x, y) onto the ground, which is what gives
//      the point its z, and cancel every modifier still running;
//   3. if the requested duration is (near) zero, or if the distance still
//      to travel is (near) zero, snap: publish the projected point on
//      m_target and return.  Both tests are `fabs(v - 0.0) < 0.001`, and
//      both are spelled with the subtraction of the shipped `dbl_6F876620`,
//      which is a *double* zero - so the subtraction is there in the
//      instruction stream and is not something to simplify away;
//   4. otherwise make one MovementModifier for the ground travel and, when
//      the caller asked for it, one FloatModifier for the height, and park
//      each in its own counted-reference slot.
//
//  Things in the shipped code that a tidier spelling loses:
//
//   * the two deltas are written back into the *incoming parameter slots*
//     (`fstp [esp+40h+arg_0]`), so the source reassigns its own parameters
//     rather than naming two new locals;
//   * the height modifier's counted reference goes through the out-of-line
//     SCountedRef::Assign (0x6F029580) while the movement one is written
//     out inline - addref the new, release the old, store.  Two spellings
//     of one operation, and both are in the shipped stream;
//   * `1.0f / duration` is computed once, into a named local, and both
//     ground components and the height rate are multiplied by it.  The
//     shipped `fld1` / `fdivrp` is the reciprocal, not two divisions.
//
//  What it cannot reproduce is the frame.  The shipped body carries the
//  __except_handler4-shaped, cookie-XORed, frame-pointer-omitted frame that
//  docs/msvc-vc8-idioms.md records as unreachable at this toolchain's fixed
//  /GS- /EHs-c-, together with the two `mov [esp+40h+var_4], <state>` stores
//  that drive it - the two counted-reference temporaries below are what
//  needs unwinding.
//
//  Own translation unit: nine real calls.
//============================================================================
#include "gameui.h"
#include "fvec3.h"
#include "floatpropmodifier.h"

//  The compiler's own intrinsics; see fvec3normalize.cpp for why they are
//  declared here rather than included, and sqrtfallback.cpp/fabsfallback.cpp
//  for why each also has a real definition somewhere in the tree.
double sqrt(double value);
#pragma intrinsic(sqrt)
//  `fabs` needs C linkage for MSVC to substitute the bare x87 instruction
//  for it (C4162 otherwise, and then it is a real call); `sqrt` does not,
//  because what the shipped code has there is a call too - the compiler's
//  own `_CIsqrt` helper, which funcmap maps onto this name.
extern "C" double __cdecl fabs(double value);
#pragma intrinsic(fabs)

//  dbl_6F876620 - a *double* zero, and flt_6F92E7A4 - one thousandth, the
//  camera's own "near enough" tolerance.  Declared by address.
extern const double g_dblZero;
extern const float  g_cameraTolerance;

//----------------------------------------------------------------------------
//  The float -> CFloat reinterpretation, *by value*.  The by-value parameter
//  is the point: the shipped code reads each component with `fld`, stores it
//  to a scratch slot with `fstp`, and only then reads that slot back as an
//  integer - which is what passing a float by value and reinterpreting the
//  parameter gives, and what casting the component's own address
//  (`*(const CFloat*)&point.m_x`) does not: that is one dword move.
//----------------------------------------------------------------------------
static CFloat AsCFloatValue(float v) { return *(const CFloat*)&v; }

void CCameraWar3::PanTo(float x, float y, float duration, float height,
                        int animateHeight)
{
    TRefCnt* job = m_pPanJob;
    if (job != 0)
    {
        if (--job->m_refcount == 0)
            job->ReleaseSelf();
    }
    m_pPanJob = 0;

    //  One zeroed three-float value copied over the offset, which is what
    //  the shipped three `fst`/`mov`/`mov` triples are - a struct
    //  assignment, not three independent stores.
    FVec3 zero;
    zero.m_x = 0.0f;
    zero.m_y = 0.0f;
    zero.m_z = 0.0f;
    m_offset = zero;

    SetControllerMode(0);

    FVec3 point;
    point.m_x = x;
    point.m_y = y;
    point.m_z = 0.0f;

    //  The copy is from the *returned* pointer, not from the named buffer:
    //  the shipped code reads the three words back through eax
    //  (`mov edx,[eax]` / `mov ecx,[eax+4]` / `mov edx,[eax+8]`), which is
    //  what using the return value gives and what `point = projected;`
    //  does not.
    FVec3 projected;
    point = *(const FVec3*)ProjectGroundPoint(&projected.m_x, &point.m_x);

    CancelModifiers();

    //  Two named float locals, not one expression: the shipped stream has
    //  the value stored and reloaded twice (once for the subtraction's
    //  result and once for `fabs`'s).
    float slack = (float)(duration - g_dblZero);
    slack = (float)fabs(slack);
    if (slack < g_cameraTolerance)
    {
        CWorldVec3 world(*(const CFloat*)&point.m_x,
                         *(const CFloat*)&point.m_y,
                         *(const CFloat*)&point.m_z);
        m_target.SetWorldPosition(&world);
        return;
    }

    //  **The parameters are the scratch space from here on.**  Every one of
    //  the values below lands in an incoming argument slot in the shipped
    //  frame (`fstp [esp+40h+arg_0]` and friends), which is why they are
    //  written back into the parameters rather than into fresh locals: a
    //  named local is a new stack slot and displaces every `[esp+N]` after
    //  it.
    CWorldVec3 current = m_target.SmartPtrToWorldVec3();
    float currentY = *(const float*)&current.m_y;
    float currentX = *(const float*)&current.m_x;
    x = x - currentX;
    y = y - currentY;

    slack = (float)sqrt(x * x + y * y);
    slack = (float)(slack - g_dblZero);
    slack = (float)fabs(slack);
    if (slack < g_cameraTolerance)
    {
        CWorldVec3 world(*(const CFloat*)&point.m_x,
                         *(const CFloat*)&point.m_y,
                         *(const CFloat*)&point.m_z);
        m_target.SetWorldPosition(&world);
        return;
    }

    //  One reciprocal, not two divisions: the shipped `fld1` / `fdivrp`.
    float rate = 1.0f / duration;
    float vx = x * rate;
    float vy = y * rate;

    if (animateHeight != 0)
    {
        FVec3 controller;
        QueryControllerVec3(&controller.m_x);

        y = point.m_z + height;
        x = controller.m_z;
        m_panZ.m_value.SetValue(AsCFloat(x));

        //  `controller.m_z` is read a second time rather than `x` being
        //  reused: the shipped `fsub [esp+40h+var_10]` goes back to the
        //  query's own buffer.
        x = y - controller.m_z;
        x = x * rate;
        y = duration;

        SFloatModifierRef ref =
            m_panZ.m_value.MakeFloatModifier(AsCFloat(x), AsCFloat(y));
        ((SCountedRef*)&m_panZ.m_pModifier)->Assign((SCountedRef*)&ref);

        m_panActive = 1;
    }

    height = duration;
    SMovementModifierRef move = m_target.MakeMoveModifier(AsCFloat(vx),
                                                          AsCFloat(vy),
                                                          AsCFloat(height));

    //  Written out rather than routed through SCountedRef::Assign: the
    //  shipped code has the three steps inline here and the call for the
    //  height slot above.
    if (move.m_ptr != 0)
        move.m_ptr->m_refcount++;
    TRefCnt* held = m_pPanMovement;
    if (held != 0)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
    m_pPanMovement = (TRefCnt*)move.m_ptr;
}
