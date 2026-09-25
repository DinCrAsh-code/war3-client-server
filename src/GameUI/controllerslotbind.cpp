//============================================================================
//  0x6F4E86C0 / 0x6F4E86E0 / 0x6F4E8720 - the three slot binders.
//
//  They are byte-identical: each takes the controller in ecx, the slot in
//  edx and a callback, a context and a blend time on the stack, and each
//  hands all five straight to the same 0x6F4E85F0.  The names
//  cameracontrollerbind.h gave the three addresses while they were
//  redirects said they "differ only in which list the callback lands on";
//  the bodies say they do not differ at all - three identical functions at
//  three addresses, which is normal in this image (docs/msvc-vc8-idioms.md,
//  "Four identical functions at four addresses is normal here").  The names
//  stay as they are, because one shipped address carries one C++ name and
//  renaming them would only move the question.
//
//  Own translation unit: all three calls are real, and the callee is not in
//  this dump.
//============================================================================
#include "cameracontrollerbind.h"

//  0x6F4E85F0 - the registration proper: the controller in ecx, the slot in
//  edx and three stack dwords it cleans itself (`retn 0Ch`).  Redirected in
//  GameUI/controllerslot_thunks.cpp.
void __fastcall RegisterControllerSlotBinding(void* controller, int slot,
                                              void* callback, void* context,
                                              float blend);

void __fastcall BindControllerSlotDirect(void* controller, int slot,
                                         void* callback, void* context,
                                         float blend)
{
    RegisterControllerSlotBinding(controller, slot, callback, context, blend);
}

void __fastcall BindControllerSlotEvent(void* controller, int slot,
                                        void* callback, void* context,
                                        float blend)
{
    RegisterControllerSlotBinding(controller, slot, callback, context, blend);
}

void __fastcall BindControllerSlotRamp(void* controller, int slot,
                                       void* callback, void* context,
                                       float blend)
{
    RegisterControllerSlotBinding(controller, slot, callback, context, blend);
}

//  0x6F4E8700 - CUnit batch-14.  A fourth, byte-identical instance of the
//  same three-address family above (docs/msvc-vc8-idioms.md, "Four
//  identical functions at four addresses is normal here") - same
//  ecx/edx/stack-callback/stack-context/stack-blend forward straight into
//  RegisterControllerSlotBinding, `retn 0Ch`.  Not renamed to match the
//  other three (no evidence ties it to "direct"/"event"/"ramp" specifically);
//  named for what it does, not guessed at beyond that.
void __fastcall BindControllerSlotColor(void* controller, int slot,
                                        void* callback, void* context,
                                        float blend)
{
    RegisterControllerSlotBinding(controller, slot, callback, context, blend);
}
