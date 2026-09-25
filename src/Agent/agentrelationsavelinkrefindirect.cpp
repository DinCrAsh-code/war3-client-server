//============================================================================
//  0x6F4A5E80 - a two-instruction forwarder: `handle` is one field into
//  whatever wraps the real link pointer (the same `+4` a caller elsewhere
//  in this module reaches through - Agent/agentbaseabssave.cpp's own
//  `NIpse::CRlAgent::Save`, four times, over its own four
//  `SRelationLink` list heads), so this just dereferences past it before
//  handing off to SaveLinkRef proper.  A tail call in the shipped code
//  (`jmp sub_6F4A5DF0`, no `call`/`retn` of its own).
//
//  Own translation unit, deliberately: declaring SaveLinkRef only (not
//  including its definition, agentrelationsyncsave.cpp) is what lets MSVC
//  emit a genuine tail-call `jmp` here instead of inlining the whole body -
//  the same reasoning CDataStoreScratch::TailCallWriteRaw's own comment
//  gives for the identical shape.
//============================================================================
#include "agentrelation.h"
#include "cdatastorescratch.h"

void __fastcall SaveLinkRef(CDataStoreScratch* store, const SRelationLink* link);

void __fastcall SaveLinkRefIndirect(CDataStoreScratch* store, void* handle)
{
    SaveLinkRef(store, *(SRelationLink* const*)((char*)handle + 4));
}
