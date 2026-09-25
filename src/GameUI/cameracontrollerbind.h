//============================================================================
//  The controller side of the camera: the ten numbered slots every camera
//  property is bound to, the three registration entry points the binder
//  uses, and the nine callbacks it registers.
//
//  A slot number is a *camera field* id - the same numbering JASS's own
//  camera natives use - and which property each one drives is settled by
//  CCameraWar3::BindControllerProps, which hands each slot the address of
//  the property it feeds.
//
//  The three registration functions differ only in which list the callback
//  lands on; nothing in this tree says more than that, so they are named
//  for what distinguishes them at the call sites - two of the nine bindings
//  use the first, six the second and one the third.  All three are
//  __fastcall with the controller in ecx and the slot in edx, and all three
//  take a float blend time as their last stack argument (`retn 0Ch`).
//============================================================================
#ifndef CAMERACONTROLLERBIND_H
#define CAMERACONTROLLERBIND_H

//  0x6F4E86C0 / 0x6F4E8720 / 0x6F4E86E0.
void __fastcall BindControllerSlotDirect(void* controller, int slot,
                                         void* callback, void* context,
                                         float blend);
void __fastcall BindControllerSlotRamp(void* controller, int slot,
                                       void* callback, void* context,
                                       float blend);
void __fastcall BindControllerSlotEvent(void* controller, int slot,
                                        void* callback, void* context,
                                        float blend);

//----------------------------------------------------------------------------
//  The callbacks.  Every one of them is `retn 4` - one stack dword, which is
//  the context the binding registered alongside it - and none is called from
//  anywhere in this slice, only had its address taken.  So the declaration
//  below is exactly what the shipped `retn 4` fixes and nothing more; what
//  each does is in its own body, which is not in this dump.
//
//  Named for the property each drives, which the binder itself establishes:
//  PublishSlotToProp is the generic one, registered six times with a
//  different SCameraProp as its context, and the other four are registered
//  with the camera itself.
//----------------------------------------------------------------------------
void __stdcall PublishSlotToProp(void* context);          // 0x6F469410
void __stdcall CameraOnTransformSlot(void* context);      // 0x6F304AA0, slot 8
void __stdcall CameraOnTargetSlot(void* context);         // 0x6F302570, slot 2
void __stdcall CameraOnRotationSlotLocked(void* context); // 0x6F302530, slot 6, mode 2
void __stdcall CameraOnZoomSlot(void* context);           // 0x6F3025D0, slot 7

//  The two the unbind pass puts on the event sink at +0x5C rather than on
//  the controller.  Same `retn 4`.
void __stdcall CameraOnModeEnter(void* context);          // 0x6F305030
void __stdcall CameraOnModeLeave(void* context);          // 0x6F3051B0

//----------------------------------------------------------------------------
//  0x6F4D31B0 - CUnit batch-15.  CUnit::RestartPropertyRamp
//  (unitorderpaththunks.cpp, still a naked redirect itself) reaches this
//  when its own +0x1EC gate is clear: hand the controller (this+0x28) one
//  stack float (this+0x300) through its own vtable slot 0x28 rather than
//  through BindControllerSlotRamp.  The controller's own class is not
//  established anywhere in this tree (no committed vtable to check the
//  slot's real signature against - the exact case
//  vtable_dispatch_audit.py's "no committed vtable" skip bucket exists
//  for), so this is reproduced as the literal naked instruction sequence:
//  no C++ typed dispatch, no class name invented. `retn 4` - the wrapper
//  cleans only its own one stack float; the callee is presumed
//  callee-cleanup like everything else in this binary, __thiscall(this,
//  float) going by the register/stack split IDA's own listing shows
//  (ecx still holds the controller at `call edx`; `push ecx` is a
//  scratch-space reservation the immediately following `fstp` overwrites
//  with the float, not a second copy of `this`).
//----------------------------------------------------------------------------
void __fastcall InvokeControllerRampVTableSlot0x28(void* controller, float value);

#endif
