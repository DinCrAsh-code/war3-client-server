//============================================================================
//  0x6F3069F0 - CCameraWar3::CancelModifiers: stop and drop every modifier
//  currently driving the camera.
//
//  Two arrays of two, walked *interleaved* - A[0], B[0], A[1], B[1] - which
//  is one loop over a pair of arrays and not one over four consecutive
//  slots; the shipped code keeps a single pointer at &B[i] and reaches A[i]
//  as `[esi-8]`, counting down from two.  Then the pan's own two
//  (ReleasePanModifiers), then the local angle property's second modifier
//  slot, which is the one that gets no destination reset.
//
//  Own translation unit: it calls ReleasePanModifiers for real.
//============================================================================
#include "gameui.h"
#include "positionmodifier.h"
#include "refcnt.h"

typedef void (__thiscall *ModifierStopFn)(void*);

//  Stop one modifier reference and clear the slot.  A helper rather than
//  four copies: the shipped code has the body twice inside the loop and
//  once more after it, which is what an inline function looks like once the
//  loop has been unrolled by two.
static __forceinline void StopAndClear(TRefCnt*& slot)
{
    MovementModifier* mod = (MovementModifier*)slot;
    if (mod != 0 && LookupHandle(mod->m_handle, mod->m_typeTag) != 0)
    {
        mod->SetTarget(&g_CFloatZero, &g_CFloatZero);
        ((ModifierStopFn)(*(void***)mod)[0x08 / 4])(mod);
    }
    TRefCnt* held = slot;
    if (held != 0)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
    slot = 0;
}

void CCameraWar3::CancelModifiers()
{
    //  Two variables and no more: an index into both arrays and a
    //  countdown.  The shipped loop keeps exactly those - one pointer
    //  walking the second array and one counter decremented into its own
    //  branch - and a `for (i = 0; i < 2; i++)` gives MSVC a third live
    //  value here and spills the counter to the stack.
    int i = 0;
    int remaining = 2;
    do
    {
        StopAndClear(m_pRampModifiers[i]);
        StopAndClear(m_pOffsetModifiers[i]);
        i++;
    }
    while (--remaining != 0);

    ReleasePanModifiers();

    //  The last one is not StopAndClear: it gets no destination reset, and
    //  its release and clear happen *only* when the handle resolved.  The
    //  shipped code's two early exits both jump past the store, so a slot
    //  whose object has already gone keeps its stale pointer.  That is what
    //  is there, not a tidier reading of it.
    TRefCnt* local = m_localAngle.m_pModifier2;
    if (local != 0 &&
        LookupHandle(((MovementModifier*)local)->m_handle,
                     ((MovementModifier*)local)->m_typeTag) != 0)
    {
        ((ModifierStopFn)(*(void***)local)[0x08 / 4])(local);

        TRefCnt* held = m_localAngle.m_pModifier2;
        if (held != 0)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
        m_localAngle.m_pModifier2 = 0;
    }
}
