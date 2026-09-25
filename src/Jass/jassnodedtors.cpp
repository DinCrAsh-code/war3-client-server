//============================================================================
//  Jass2/Nodes.h - AST node destructors, jassvm-sweep-K5's own closure.
//
//  Six classes: three whose constructors jassvm-batch-5 already
//  reconstructed in jassnodes.h (NodeDebug/NodeFunction/NodeArgList - a
//  separate translation unit here purely so `~NodeXxx()` cannot inline
//  into `DeleteSelf` in jassnodedeleteself.cpp, the same reason
//  escmenu_deleteself.cpp's own header comment gives), and three whose
//  real constructors remain thunked (NodeIf/NodeCode/NodeCall,
//  jassvmbatch4_thunks.cpp) but whose real *destructors* this sweep does
//  reconstruct - jassnodes.h declares each as a bare `Node`-derived struct
//  with no constructor at all, which is not an error unless something
//  tries to default-construct one, and nothing here does.
//
//  Every one of these is SEH-framed in the shipped body (the
//  `__except_handler4`-shaped frame docs/msvc-vc8-idioms.md already
//  documents as unreproducible with this toolchain's fixed /GS- /EHs-c-),
//  so every one is `DIFFERS` regardless of how faithfully the release
//  logic matches - not a transcription defect to keep chasing.
//============================================================================
#include "jassnodes.h"

//  0x6F460960
NodeDebug::~NodeDebug()
{
    ReleaseChildNode((Node**)&m_body);
}

//  0x6F461110.  Releases `m_decl`, `m_args`, `m_extra` each conditionally
//  through its own vtable slot 0 (the ctor's own field-role comments in
//  jassnodes.h notwithstanding - a declaration object living in this same
//  Node pool is itself release-able the same way any child is), then
//  `m_reserved2C` unconditionally through the out-of-range
//  `ReleaseChildOrList` helper - the one field the ctor's own comment
//  says is "always zeroed here", so this branch is very likely dead in
//  every dump either batch has actually seen rather than evidence the
//  field is ever non-null in practice.  `m_returnType` (+0x30) is left
//  untouched, matching the dump exactly.
NodeFunction::~NodeFunction()
{
    ReleaseChildNode((Node**)&m_decl);
    ReleaseChildNode((Node**)&m_args);
    ReleaseChildNode((Node**)&m_extra);
    ReleaseChildOrList((Node*)m_reserved2C);
}

//  0x6F4622C0.  Releases `m_value` then `m_next` (the tail of the
//  argument-list chain, itself a `NodeArgList*` - released here as a
//  plain `Node*`, matching the dump's own generic vtable-slot-0 call).
NodeArgList::~NodeArgList()
{
    ReleaseChildNode((Node**)&m_value);
    ReleaseChildNode((Node**)&m_next);
}

//  0x6F461B00.  Releases the condition (+0x20) conditionally through its
//  own vtable slot 0, then both branches (+0x24 "then", +0x28 "else")
//  unconditionally through the out-of-range `ReleaseChildOrList` helper -
//  either branch is routinely a `NodeCode` statement list rather than a
//  bare expression/statement.
NodeIf::~NodeIf()
{
    ReleaseChildNode((Node**)&m_reserved20);
    ReleaseChildOrList((Node*)m_reserved24);
    ReleaseChildOrList((Node*)m_reserved28);
}

//  0x6F4616B0.  Releases only +0x24 (the statement list's own head/tail),
//  conditionally through its own vtable slot 0 - a plain release, not the
//  `ReleaseChildOrList` walk (this class *is* the list `ReleaseChildOrList`
//  itself detects via `m_kind == 4` and walks into from elsewhere; its
//  own destructor releases its own held node the ordinary way).
NodeCode::~NodeCode()
{
    ReleaseChildNode((Node**)&m_reserved24);
}

//  0x6F463710.  Releases only +0x30 (the resolved argument list, most
//  likely - `NodeCall`'s own real constructor, still thunked at
//  0x6F463630, is out of this sweep's scope), conditionally through its
//  own vtable slot 0.
NodeCall::~NodeCall()
{
    ReleaseChildNode((Node**)&m_reserved30);
}
