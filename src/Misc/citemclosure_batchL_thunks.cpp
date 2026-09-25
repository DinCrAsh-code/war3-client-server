//============================================================================
//  citem-batch-L: the closing batch of CItem's own vtable-closure sweep
//  (citem_vtable_closure_worklist.json) - 33 addresses at BFS depth 12-25+
//  off slots 32/104, all reached through CUnit::Reposition's own callee
//  tree (asm/sub_6F2A5D50_0x6F2A5D50_calltree_asm.md; the CItem-side dump
//  named in this batch's own brief carries none of them - CUnit's own
//  Reposition override is the real root).  Six prior sibling batches
//  already found this closure to be almost entirely Storm/NTempest
//  terrain-rendering internals and a WEDB subsystem the dispatcher merely
//  passes through; this batch's own depth confirms the same pattern one
//  layer further down, now inside CUnit's own selection-circle and visual-
//  attachment bookkeeping specifically:
//
//    * every function below either touches only fields selectable.h/
//      widget.h already document as visual-attachment state
//      (m_attachmentSlot1/2, the -1-sentinel handle at CWidget's own +0x38,
//      the two attachment slot indices CItem__Method_0x1A0's own writeup
//      already called "visual-attachment bookkeeping, not item-placement
//      logic" and declined to reconstruct for exactly that reason), or
//    * calls straight into an unnamed NTempest/Storm render-primitive
//      singleton chain (sub_6F01F5A0 GetStormSingletonA, sub_6F74E920,
//      sub_6F300710 GetGameUI, sub_6F32C880 GetOrCreateFootprintTypeData,
//      Storm_401/403/405) with no CItem/CUnit-owned state of its own left
//      to name, or
//    * is a second, unidentified TSExplicitList<T> list/table
//      instantiation (the 0x6F7Cxxxx cluster) whose element type this call
//      tree never dereferences at all - see
//      Containers/citemclosure_unknownlistarray.cpp for the two members of
//      that cluster (ComputeChunk/SetAlloc) that *are* reconstructed for
//      real, matching generics of the exact template already established
//      throughout Containers/.
//
//  Declared signatures below exist only to give thunk_abi_audit.py the
//  right `retn <n>` - the byte count read straight off each dump's own
//  trailing `retn` line - not to claim a real calling convention: nothing
//  in src/ calls any of these (grep says zero references outside this
//  file - every one of this batch's callers within CUnit::Reposition's own
//  tree is itself still a thunk, either pre-existing or one of the pair
//  added by this batch just below), so the declaration only has to
//  compile and clean the right number of stack bytes.  `__stdcall` with N
//  plain int parameters is the simplest spelling that cleans exactly 4*N
//  bytes regardless of what the shipped body's real arguments turn out to
//  be, the same convention citemclosure_terrain_gates2.cpp already uses.
//
//  Two of this batch's addresses (0x6F4E81C0/0x6F4E8360) end in a tail
//  `jmp` through a vtable slot rather than a static `retn`, so their real
//  cleanup count is not recoverable from this dump at all; both are
//  zero-argument `__stdcall` placeholders for the same "nothing calls it"
//  reason the rest of this file is.
//============================================================================

#define STORM_THUNK(sig, addr)                    \
    __declspec(naked) sig                         \
    {                                              \
        __asm { mov eax, addr }                    \
        __asm { jmp eax }                           \
    }

//----------------------------------------------------------------------------
//  0x6F010100 / 0x6F0100C0 - the terrain-side selection-circle primitive's
//  two release paths, called from CUnit::TearDownSelectionVisual
//  (Unit/unit_selectionvisual_notify.cpp).  `retn 0`, `ecx` the primitive
//  handle - real callers, real `__fastcall` signature.
//----------------------------------------------------------------------------
STORM_THUNK(void __fastcall ReleaseTerrainSelectionPrimitive(int handle),
            0x6F010100)
STORM_THUNK(void __fastcall ReleaseTerrainSelectionPrimitiveAndReset(int handle),
            0x6F0100C0)

//----------------------------------------------------------------------------
//  The 0x6F7Cxxxx list-group cluster's own remaining members (the element
//  type these never dereference, matching Containers/
//  citemclosure_unknownlistarray.cpp's own ComputeChunk/SetAlloc pair) -
//  release/insert/unlink/reset operations over a small doubly-linked
//  group this call tree never names an owner for.  0x6F7CB760/0x6F7E0FA0
//  are not here: a concurrent FACEDATA batch reconstructed them for real
//  (TeardownFaceRecord, Misc/citemclosure_facedata_teardown_thunk.cpp;
//  OpenFaceFileStream, Misc/citemclosure_facedata_fileload.cpp, 0.882 -
//  the CRT fopen/fseek/ftell wrapper this file's own header comment for
//  OpenTerrainStreamReader guessed at from its shape alone) - keeping
//  both would be two funcmap names for one address.
//----------------------------------------------------------------------------
STORM_THUNK(void __stdcall InsertListEntry(void), 0x6F7C9730)            // retn 0
STORM_THUNK(void __stdcall ReleaseListChain(void), 0x6F7C97D0)           // retn 0
STORM_THUNK(void __stdcall UnlinkListNode(void), 0x6F7C9AC0)             // retn 0
STORM_THUNK(void __stdcall RunAndClearCallbackList(int, int), 0x6F7CA980) // retn 8
STORM_THUNK(void __stdcall ResetListGroupFields(void), 0x6F7C9780)       // tail-jmp -> retn 0
STORM_THUNK(void __stdcall ResetListGroupHeader(void), 0x6F7C9760)       // retn 0

//----------------------------------------------------------------------------
//  CUnit::Reposition's own visual-attachment/terrain-splat bookkeeping -
//  the register-argument trampoline, the two footprint-splat installers,
//  the position-notify state machine (0x6F283xxx, m_notifyHandle25C's own
//  small state machine per unit.h's existing comment on that field), the
//  facing-vector publisher and the two attachment-position installers
//  selectable.h's own CItem__Method_0x1A0 writeup already declined to
//  reconstruct for the identical reason.
//
//  0x6F283BC0 (was declared here as `DispatchPositionNotifyState`) moved
//  out - CUnit vtable slot-finalize pass: it is a real `__thiscall`
//  member on `this` (its own `mov ecx, esi` right before the call proves
//  it, not the untyped free `__stdcall` placeholder every other entry
//  here still is), and it now has a real caller - CUnit::ClearPendingAndNotify
//  (slot 32, unit_clearpendingnotify.cpp). Declared there, not here. Same
//  address, same `retn 4`.
//----------------------------------------------------------------------------
STORM_THUNK(void __stdcall SwapRegisterArgsAndDispatch(int), 0x6F01F440)  // retn 4 (one stack arg at its one call site)
STORM_THUNK(int  __stdcall InitializeFromTemplateRecord(int), 0x6F12C490) // retn 4
STORM_THUNK(int  __stdcall CheckDualTerrainFlagState(int, int), 0x6F24F3A0) // retn 8
STORM_THUNK(void __stdcall RefreshPositionDeltaState(int, int), 0x6F2781F0) // retn 8
STORM_THUNK(void __stdcall NotifyPositionStateChange(int, int), 0x6F283830) // retn 8
STORM_THUNK(void __stdcall RefreshPositionNotifyState(void), 0x6F283A50)   // retn 0
STORM_THUNK(void __stdcall RefreshUberSplatAttachment(void), 0x6F28E170)   // retn 0
STORM_THUNK(void __stdcall PublishFacingVectorState(int, int), 0x6F2A8700) // retn 8
STORM_THUNK(void __stdcall RefreshFootprintOrientationIfChanged(void), 0x6F2AD980) // retn 0
STORM_THUNK(void __stdcall ReleaseAndRefreshFootprintOrientation(void), 0x6F2ADA60) // retn 0
STORM_THUNK(void __stdcall InstallVisualAttachmentPosition(int, int, int, int, int),
            0x6F303350)                                                    // retn 0x14
STORM_THUNK(void __stdcall RefreshUnitTypeSplatIfPresent(void), 0x6F3334C0) // retn 0
STORM_THUNK(void __stdcall RegisterFootprintSplatMask(int), 0x6F39A220)    // retn 4
STORM_THUNK(void __stdcall ApplyVisualAttachmentIfEligible(void), 0x6F3CD7B0) // retn 0
//  0x6F4E81C0 is NOT thunked here: it is already GameUI/controllerslotblend.cpp's
//  own ApplyControllerSlotVec3 (reached via controllerslot_thunks.cpp's
//  push-then-ret private-ABI trampoline, host in eax) - a second, naked
//  definition at the identical address under a different name compiled
//  fine and linked fine (distinct symbols, no collision there) but was a
//  live regression: MergeControllerSlotVec3 (GameUI/controllerslotblend.cpp)
//  calls ApplyControllerSlotVec3 by name, and once this file's own address
//  literal outbid it for canonlib's SYM resolution at that call site, the
//  target's named `call ResetFloatTargetSlot` stopped matching the
//  generated `call SYM` at all (0.979 -> 0.957).  One shipped address, one
//  C++ name - see CLAUDE.md's own rule.
STORM_THUNK(void __stdcall ResetFloatTargetSlotForRecord(void), 0x6F4E8360) // tail-jmp, no static retn

#undef STORM_THUNK
