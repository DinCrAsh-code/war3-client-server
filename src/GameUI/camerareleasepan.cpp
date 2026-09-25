//============================================================================
//  0x6F305960 - CCameraWar3::ReleasePanModifiers: drop the two modifiers a
//  timed pan installs, and mark the pan as no longer running.
//
//  The movement modifier goes first: if its handle still resolves, its
//  destination is set back to the origin and its own slot 2 (+0x08) is run,
//  which is what stops it.  The height modifier follows and only gets the
//  slot-2 call - a FloatModifier has no two-axis destination to reset.
//
//  The height modifier is taken into a *scoped counted reference* first
//  (addref, work, release on scope exit).  That local is the whole reason
//  this function has an SEH frame at all, and the reason it cannot score
//  1.000 here: the unwind funclet the shipped code emits
//  (`lea ecx,[ebp-10h]` / `jmp 0x6F029560`) is the same
//  __except_handler4-shaped, cookie-XORed, frame-pointer-omitted frame
//  docs/msvc-vc8-idioms.md records as unreproducible at this toolchain's
//  fixed /GS- /EHs-c-.  The body inside it is transcribed line for line.
//
//  Own translation unit: CCameraWar3::CancelModifiers reaches it with a
//  real call.
//============================================================================
#include "gameui.h"
#include "positionmodifier.h"
#include "refcnt.h"

//  Slot 2 (+0x08) on a modifier: "stop driving whatever you are driving".
//  A hand-written dispatch - this repo has no committed vtable for either
//  modifier class - and `retn 0`, which is what the call sites show
//  (`mov eax,[esi]` / `mov edx,[eax+8]` / `mov ecx,esi` / `call edx` with
//  nothing pushed).
typedef void (__thiscall *ModifierStopFn)(void*);

//----------------------------------------------------------------------------
//  The scoped counted reference the height half is held in.  One word, a
//  constructor that takes a reference and a destructor that drops it -
//  which is exactly SCountedRef plus lifetime, and exactly what the shipped
//  frame unwinds.
//----------------------------------------------------------------------------
struct SScopedRef
{
    SScopedRef(TRefCnt* ref) : m_ref(ref)
    {
        if (m_ref != 0)
            m_ref->m_refcount++;
    }
    ~SScopedRef()
    {
        if (m_ref != 0)
        {
            if (--m_ref->m_refcount == 0)
                m_ref->ReleaseSelf();
        }
    }
    TRefCnt* m_ref;
};

void CCameraWar3::ReleasePanModifiers()
{
    MovementModifier* move = (MovementModifier*)m_pPanMovement;
    if (move != 0 && LookupHandle(move->m_handle, move->m_typeTag) != 0)
    {
        move->SetTarget(&g_CFloatZero, &g_CFloatZero);
        ((ModifierStopFn)(*(void***)move)[0x08 / 4])(move);
    }
    TRefCnt* heldMove = m_pPanMovement;
    if (heldMove != 0)
    {
        if (--heldMove->m_refcount == 0)
            heldMove->ReleaseSelf();
    }
    m_pPanMovement = 0;

    SScopedRef held(m_panZ.m_pModifier);
    MovementModifier* height = (MovementModifier*)held.m_ref;
    if (height != 0 && LookupHandle(height->m_handle, height->m_typeTag) != 0)
        ((ModifierStopFn)(*(void***)height)[0x08 / 4])(height);

    TRefCnt* heldHeight = m_panZ.m_pModifier;
    if (heldHeight != 0)
    {
        if (--heldHeight->m_refcount == 0)
            heldHeight->ReleaseSelf();
    }
    m_panZ.m_pModifier = 0;
    m_panActive = 0;
}
