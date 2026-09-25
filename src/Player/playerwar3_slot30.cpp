//============================================================================
//  0x6F415AA0 - CPlayerWar3 vtable slot 30 (+0x78).  Initialises several
//  fields, notifies the two embedded FloatMini-shaped objects at +0x298/
//  +0x2A0 of the constant 1.0 (g_slopeThreshold1) through their own
//  vtable slot 0 - a two-argument notify for these two fields specifically
//  rather than a destructor call, matching floatmini.h's own note on this
//  exact shape - then builds a fresh IntegerListener bound into the sixth
//  AbilityEntry (index 5, playerwar3.h) and smart-pointer-assigns it into
//  +0x27C: AddRef the new value, release the old one, store, then release
//  the local temporary's own extra reference - the same
//  assign-then-drop-the-temporary shape CAgentPtr's own copy-assignment
//  uses elsewhere in this codebase, just spelled out over a class this
//  session could not name (CLAUDE.md's own house style for an
//  unidentified refcounted field).
//
//  Reaches CPlayerWar3::BuildIntegerListenerLevel0/Level1
//  (playerwar3_buildintegerlistenerlevel0.cpp/
//  playerwar3_buildintegerlistenerlevel1.cpp, 0x6F02D800/0x6F40FAF0,
//  each its own TU so /Ob2 cannot merge them - measured) and, through
//  those, CPlayerWar3::AbilityEntry::BuildIntegerListener
//  (playerwar3_ability5integerlistener.cpp, 0x6F473180) - all three real,
//  depth-1/2/3 reconstructions - which itself reaches the out-of-scope
//  bind helper at 0x6F480D50 (integerlistenerbindthunk.cpp, depth 4,
//  thunked).
//============================================================================
#include "playerwar3.h"
#include "refcnt.h"
#include "CFloat.h"

extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4 - encoded 1.0

typedef void (__thiscall *FloatMiniNotifyFn)(void*, const CFloat*, int);

static void ReleaseRef(void* field)
{
    TRefCnt* p = *(TRefCnt**)field;
    if (p != 0 && --p->m_refcount == 0)
        p->ReleaseSelf();
}

void CPlayerWar3::Method_0x78Slot30()
{
    *(unsigned int*)((char*)this + 0x25C) = 0x60;
    *(unsigned int*)((char*)this + 0x260) = 0;
    *(unsigned int*)((char*)this + 0x268) = 5;
    *(unsigned int*)((char*)this + 0x270) = 0;
    *(int*)((char*)this + 0x274) = -1;
    *(int*)((char*)this + 0x278) = -1;

    ((FloatMiniNotifyFn)(*(void***)&m_reserved298)[0])(
        &m_reserved298, &g_slopeThreshold1, 1);
    ((FloatMiniNotifyFn)(*(void***)&m_reserved2A0)[0])(
        &m_reserved2A0, &g_slopeThreshold1, 1);

    *(unsigned int*)((char*)this + 0x26C) = 1;
    *(unsigned int*)((char*)this + 0x32C) = 0;
    *(unsigned int*)((char*)this + 0x330) = 0;

    void* newListener = 0;
    void** listenerSlot = BuildIntegerListenerLevel0(&newListener, 6, this, 0);
    newListener = *listenerSlot;

    if (newListener)
        ++*(int*)((char*)newListener + 4);

    void* old = *(void**)((char*)this + 0x27C);
    ReleaseRef(&old);

    *(void**)((char*)this + 0x27C) = newListener;

    if (newListener)
        ReleaseRef(&newListener);
}
