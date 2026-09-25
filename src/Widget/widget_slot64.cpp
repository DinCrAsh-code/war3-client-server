//============================================================================
//  0x6F2AD710 - CWidget's vtable slot 64 (+0x100).  See widget.h.
//
//  The same "ask the handle-bearing sub-object where it is" opening as
//  GetWorldPositionWithGround (widget_worldpos.cpp), but the position goes
//  to the world-object singleton instead of to a caller's buffer.  Its own
//  translation unit for the usual reason: two real calls to reproduce.
//============================================================================
#include "widget.h"
#include "playercolor.h"    // g_unk6FAB65F4 - the world-object singleton

typedef CPathRef* (__thiscall *GetHandleObjectFn)(void* self);

//  0x6F39F280 - hand a position and two caller arguments to the world-object
//  singleton, which rewrites its own first two stack slots and tail-jumps
//  into SPlayerTableSubmit::SubmitMasked.  Reconstructed in
//  publishwidgetposition.cpp; declared, not defined, here.  A real thiscall
//  member of that singleton: `this` in ecx, four stack arguments, so it is
//  declared through a struct rather than as a __fastcall free function -
//  otherwise the first argument would go in edx and the `ret` counts would
//  disagree.
struct SWorldObjectSingleton
{
    int PublishWidgetPosition(CWorldVec3* pos, int a1, int a2, int* out);
};

void CWidget::PublishPosition(int a1, int a2)
{
    //  The singleton pointer is read into a callee-saved register *before*
    //  the two calls that follow, exactly as the shipped
    //  `mov esi, dword_6FAB65F4` at the top does - written after them it
    //  would be reloaded, the same point agent_releaseself.cpp makes about
    //  g_pGameData.
    SWorldObjectSingleton* world = (SWorldObjectSingleton*)g_unk6FAB65F4;

    //  Two locals, not one: the shipped frame is 0x18 bytes - two whole
    //  CWorldVec3s - and it copies the returned one into the second before
    //  taking its address.  That is exactly the "assigning a call's result
    //  to a named local costs a real copy" shape docs/msvc-vc8-idioms.md
    //  records; written as one local MSVC constructs straight into it and
    //  the frame comes out 0xC bytes and three moves shorter.
    CWorldVec3 raw =
        ((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this)
            ->SmartPtrToWorldVec3();
    CWorldVec3 pos = raw;

    world->PublishWidgetPosition(&pos, a1, a2, 0);

}
