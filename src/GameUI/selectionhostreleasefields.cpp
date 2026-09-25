//============================================================================
//  0x6F36D130 - release seven TRefCnt-derived fields (+0x1C8 down to
//  +0x1B0)
//  in that order and tail into the base's own destructor
//  (0x6F3677D0, GameUI/selectionhostdtor.cpp).  `retn 0`.
//
//  Each release is the inline `if (--field->m_refcount == 0)
//  field->ReleaseSelf();` idiom this codebase already uses throughout
//  Agent/ (agent.cpp, agentptrarray.cpp, agentwar3_dtor.cpp, ...) rather
//  than a call to the out-of-line `TRefCnt::Release()` - that member's own
//  body goes through `Self()` first and tail-calls vtable slot 1
//  (DeleteSelf), where this shipped stream decrements in place and calls
//  slot 0 (ReleaseSelf) directly.
//
//  The receiver's real class is not established by anything this dump
//  reaches (its one caller in this batch, 0x6F36D970, is itself a bare
//  `jmp` alias IDA gives its own heading), so it stays a minimal local
//  view sized to the highest offset touched, the same convention
//  GameUI/gameuinotifyleaf.cpp already uses for the same reason.
//============================================================================
#include "refcnt.h"

struct SSelectionHostSixRefs
{
    char     m_reserved00[0x1B0];
    TRefCnt* m_field1B0;    // +0x1B0
    TRefCnt* m_field1B4;    // +0x1B4
    TRefCnt* m_field1B8;    // +0x1B8
    TRefCnt* m_field1BC;    // +0x1BC
    TRefCnt* m_field1C0;    // +0x1C0
    TRefCnt* m_field1C4;    // +0x1C4
    TRefCnt* m_field1C8;    // +0x1C8
};

//  0x6F3677D0 - GameUI/selectionhostdtor.cpp.  Declared on `void*` here to
//  match that translation unit's own parameter type exactly - a mismatched
//  pointee type mangles to a different symbol and the two never link
//  (tools/link_check.py's first defect class).
void __fastcall SelectionHostBaseDtor(void* self);

void __fastcall ReleaseSelectionHostSixRefsAndBaseDtor(
    SSelectionHostSixRefs* self)
{
    if (self->m_field1C8 != 0 && --self->m_field1C8->m_refcount == 0)
        self->m_field1C8->ReleaseSelf();

    if (self->m_field1C4 != 0 && --self->m_field1C4->m_refcount == 0)
        self->m_field1C4->ReleaseSelf();

    if (self->m_field1C0 != 0 && --self->m_field1C0->m_refcount == 0)
        self->m_field1C0->ReleaseSelf();

    if (self->m_field1BC != 0 && --self->m_field1BC->m_refcount == 0)
        self->m_field1BC->ReleaseSelf();

    if (self->m_field1B8 != 0 && --self->m_field1B8->m_refcount == 0)
        self->m_field1B8->ReleaseSelf();

    if (self->m_field1B4 != 0 && --self->m_field1B4->m_refcount == 0)
        self->m_field1B4->ReleaseSelf();

    if (self->m_field1B0 != 0 && --self->m_field1B0->m_refcount == 0)
        self->m_field1B0->ReleaseSelf();

    SelectionHostBaseDtor(self);
}
