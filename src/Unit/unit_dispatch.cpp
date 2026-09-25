//============================================================================
//  0x6F2A7E60 - CUnit's vtable slot 3 override (+0x0C), replacing
//  CObserver's own flat `return 0` (agentwar3_slots.cpp) with a real
//  message dispatcher: switch on the dispatched SAgentMessage's own
//  `m_id` (observer.h) and fan out to one of ~30 per-id case handlers.
//
//  The switch lowers to three separate compiler-generated dispatches, not
//  one - which the source has to reproduce structurally to get the same
//  instruction shapes, since a single flat `switch` over all ~30 values
//  would not necessarily compile back to the same three-way split:
//
//    1. a short subtract-and-compare chain for five ids that sit well
//       below the big table (0xD0146, 0xD0147, 0xD0148, 0xD014C, 0x80264,
//       0x80265, 0xD0004) plus 0xD0005/0xD0006/0xD000E/0xD0145 just above
//       it and 0xD01B3 further above still - IDA shows these as a run of
//       `sub`/`jz` tests against the *dispatched* id, i.e. real
//       comparisons the compiler chose not to fold into the jump table
//       below because they are not contiguous with it;
//    2. a dense jump table (`jpt_6F2A7F33`) over ids 0xD0146..0xD01D4
//       (143 contiguous cases), most of which fall to the same default
//       (return 0) and the ~21 real ones below;
//    3. the shared `return 0` default both dispatches fall through to.
//
//  Writing this as one `switch (msg->m_id) { case 0x80264: ...; case
//  0xD0146: ...; ... }` over the full mixed set lets the compiler choose
//  its own split, and it reproduces the shipped body's own two-dispatch
//  shape (a handful of `sub`/`jz` compares ahead of one dense jump table)
//  rather than emitting a single giant table or a single compare chain -
//  the range gap between the low cluster and 0xD0146 is exactly what
//  drives that choice on both sides.
//
//  Two cases push nothing but `this`; four push the dispatched message
//  pointer too (`push ecx` *before* `mov ecx, esi` overwrites it, since
//  `ecx` still holds `msg` from the function's own prologue up to that
//  point) - `Method_0x0C_DropAllAbilities` and the `StoreMessageField...`/
//  `ClearMotionRefOnHitGate` family (unit.h) already established which
//  shape each of *their* addresses takes; every new leaf here matches its
//  own call site's push count exactly, per thunk_abi_audit.py's own rule.
//
//  Three cases have no callee at all - a single flag bit read directly
//  off `this` (CAgentWar3::m_flags at +0x20, agentwar3.h; CUnit's own
//  m_flags5C at +0x5C) - so they compile to no call whatsoever, matching
//  the dump's own bare `or`/`and dword ptr [esi+N], imm`.
//
//  The ~21 still-unnamed leaves this dispatcher reaches are naked
//  redirects with no dump of their own anywhere in this tree (only ever
//  referenced as this switch's own call targets) - per this session's own
//  "do not go deep" scope, none is chased past its own call site's push
//  count.  Several tail-jump into a real __except_handler4/__CxxFrameHandler3
//  funclet frame at their own real bodies (0x6F294920, 0x6F29DA40,
//  0x6F29ED60, 0x6F2A0330 all end their dump in `jmp __CxxFrameHandler3`) -
//  the same SEH-frame ceiling this file's docs/targets/CUnit__vtable.md
//  already documents at length - which would cap their own score even if
//  reconstructed, so thunking them costs nothing past what a from-scratch
//  attempt would keep anyway.
//============================================================================
#include "unit.h"

//  the two SEH-blocked leaves cunitclosure_batch14_thunks.cpp already
//  redirects, widened there to take `this` since this switch is their
//  real caller (see that file's own updated comment).
void __fastcall CUnitClosure_SEH_6F2A0420(void* self);
void __fastcall CUnitClosure_SEH_6F2A4880(void* self);

#define ADDR_THUNK(name, addr)                 \
    __declspec(naked) void CUnit::name()       \
    {                                           \
        __asm { mov eax, addr }                 \
        __asm { jmp eax }                        \
    }
#define ADDR_THUNK_ARG(name, addr)             \
    __declspec(naked) void CUnit::name(void*)  \
    {                                           \
        __asm { mov eax, addr }                 \
        __asm { jmp eax }                        \
    }

ADDR_THUNK(MsgHandler_6F2A4990, 0x6F2A4990)
ADDR_THUNK_ARG(MsgHandler_6F29DB50, 0x6F29DB50)
ADDR_THUNK(MsgHandler_6F2A0330, 0x6F2A0330)
ADDR_THUNK_ARG(MsgHandler_6F29DA40, 0x6F29DA40)
ADDR_THUNK(MsgHandler_6F2DCC70, 0x6F2DCC70)
ADDR_THUNK(MsgHandler_6F2E5C20, 0x6F2E5C20)
ADDR_THUNK(MsgHandler_6F2A4950, 0x6F2A4950)
ADDR_THUNK(MsgHandler_6F28BAA0, 0x6F28BAA0)
ADDR_THUNK(MsgHandler_6F2A0140, 0x6F2A0140)
ADDR_THUNK(MsgHandler_6F2828C0, 0x6F2828C0)
ADDR_THUNK(MsgHandler_6F29D570, 0x6F29D570)
ADDR_THUNK(MsgHandler_6F28BF00, 0x6F28BF00)
ADDR_THUNK(MsgHandler_6F294920, 0x6F294920)
ADDR_THUNK(MsgHandler_6F28C030, 0x6F28C030)
ADDR_THUNK(MsgHandler_6F28BFF0, 0x6F28BFF0)
ADDR_THUNK(MsgHandler_6F2A01C0, 0x6F2A01C0)
ADDR_THUNK(MsgHandler_6F2A4810, 0x6F2A4810)
ADDR_THUNK(MsgHandler_6F29ED60, 0x6F29ED60)
ADDR_THUNK(MsgHandler_6F2A7D80, 0x6F2A7D80)
ADDR_THUNK_ARG(MsgHandler_6F29DBC0, 0x6F29DBC0)
ADDR_THUNK(MsgHandler_6F284950, 0x6F284950)

#undef ADDR_THUNK
#undef ADDR_THUNK_ARG

int CUnit::Method_0x0C(const SAgentMessage* msg)
{
    switch (msg->m_id)
    {
    case 0x80264: MsgHandler_6F2A0330(); return 1;
    case 0x80265: CUnitClosure_SEH_6F2A0420(this); return 1;
    case 0xD0004: CancelPendingEventGroup(); return 1;
    case 0xD0005: MsgHandler_6F29DA40((void*)msg); return 1;
    case 0xD0006: MsgHandler_6F2DCC70(); return 1;
    case 0xD000E: MsgHandler_6F2E5C20(); return 1;
    case 0xD0145: MsgHandler_6F2A4950(); return 1;
    case 0xD0146: MsgHandler_6F2A4990(); return 1;
    case 0xD0147: MsgHandler_6F29DB50((void*)msg); return 1;
    case 0xD0148: m_flags |= 0x200; return 1;
    case 0xD014C: m_flags &= ~0x200u; return 1;
    case 0xD0153: CUnitClosure_SEH_6F2A4880(this); return 1;
    case 0xD0154: MsgHandler_6F28BAA0(); return 1;
    case 0xD0155: MsgHandler_6F2A0140(); return 1;
    case 0xD0156: CancelPropertyRampProperty0x5C(); return 1;
    case 0xD0157: MsgHandler_6F2828C0(); return 1;
    case 0xD0158: MsgHandler_6F29D570(); return 1;
    case 0xD0159: MsgHandler_6F28BF00(); return 1;
    case 0xD015A: Method_0x0C_DropAllAbilities(); return 1;
    case 0xD015B: MsgHandler_6F294920(); return 1;
    case 0xD015C: MsgHandler_6F28C030(); return 1;
    case 0xD015D: MsgHandler_6F28BFF0(); return 1;
    case 0xD015E: MsgHandler_6F2A01C0(); return 1;
    case 0xD0162: StoreMessageField0x34ToInt194((void*)msg); return 1;
    case 0xD0164: MsgHandler_6F2A4810(); return 1;
    case 0xD019D: PopPendingTask(); MsgHandler_6F29ED60(); return 1;
    case 0xD019F: MsgHandler_6F2A7D80(); return 1;
    case 0xD01A1: MsgHandler_6F29DBC0((void*)msg); return 1;
    case 0xD01A4: ClearMotionRefOnHitGate((void*)msg); return 1;
    case 0xD01B3: m_flags5C &= ~0x40u; return 1;
    case 0xD01D4: MsgHandler_6F284950(); return 1;
    }
    return 0;
}
