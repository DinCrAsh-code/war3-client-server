//============================================================================
//  CCameraWar3's own real vtable overrides (slots 1, 7, 8, 9, 13, 14, 15,
//  22 - user_knowledge.json's own CCameraWar3 vtable_members table,
//  vtable @ 0x6F9365AC).  Slot 3 (Method_0x0C / Update, the per-frame
//  dispatch root) is a real reconstruction now too -
//  GameUI/cameraupdate.cpp - session-vtable-w647wp-camera's own closing
//  pass; every slot in this class's vtable is reconstructed.  The
//  destructor (0x6F308340, called by address rather than through the
//  vtable) stays thunked below for the reason given at its own definition.
//
//  Every signature here is a virtual *override* of a CAgent/CObserver slot
//  already declared and reconstructed elsewhere (agent.h/observer.h), so its
//  stack-argument shape is fixed by the base declaration - the ABI a C++
//  override must keep - and not something this file is free to guess at.
//
//  NOT reconstructions.  Never offer them as hookable (they aren't - see
//  tools/hookset.py: a member of a class with no externally-visible symbol
//  the way a naked jmp-forwarder never is; check with
//  tools/thunk_inventory.py, which will list all nine as NEED_DUMP).
//============================================================================
#include "gameui.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  slot 1 / +0x04 (0x6F3085C0) - real now, GameUI/cameradeleteself.cpp.
//  slot 3 / +0x0C (0x6F308E70) - real now, GameUI/cameraupdate.cpp.
//  slot 7 / +0x1C (0x6F301E40) - reconstructed now, GameUI/cameraagiletypeid.cpp.
//  slot 8 / +0x20 (0x6F301EE0) - reconstructed now, GameUI/cameraregisterproperties.cpp.
//  slot 9 / +0x24 (0x6F3022B0) - reconstructed now, GameUI/cameraunregisterproperties.cpp.
//  slot 13 / +0x34 (0x6F308B60) - reconstructed now, GameUI/cameradeactivate.cpp.
//  slot 14 / +0x38 (0x6F303790) - reconstructed now, GameUI/camerasaveload.cpp.
//  slot 15 / +0x3C (0x6F308810) - reconstructed now, GameUI/camerasaveload.cpp.
//  slot 22 / +0x58 (0x6F301DF0) - reconstructed now, GameUI/cameragetclassname.cpp.

//  0x6F308340, the real scalar destructor DeleteSelf (GameUI/cameradeleteself.cpp)
//  calls by address, is a real reconstruction now too - GameUI/cameradestructor.cpp.
//  Not a vtable slot, so it never lived in this file's own ADDR_THUNK list; the
//  naked two-instruction redirect that used to stand in for it here is gone.
