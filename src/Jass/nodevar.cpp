//============================================================================
//  0x6F4637F0 - NodeVar::NodeVar.  See jassnodes.h.
//
//  SEH-framed in the shipped body (docs/msvc-vc8-idioms.md's known gap -
//  this build's fixed /GS- /EHs-c- reproduces none of it), so this is
//  DIFFERS regardless of how faithfully the field stores are reproduced.
//  `decl`'s own +0x14 (not `decl` itself) is what lands in this node's own
//  +0x20; its resolved type comes from JassDecl_TypeSlot(decl)
//  (jassdeclhelpers.cpp), not straight from `decl`'s own +0x10/+0x14 the
//  way NodeUnary's operand-type copy does.
//============================================================================
#include "jassnodes.h"

void* __fastcall JassDecl_TypeSlot(void* decl);

NodeVar::NodeVar(JassCompileContext* ctx, void* decl, Node* index)
    : Node(ctx, 0x25)
{
    m_decl = *(void**)((char*)decl + 0x14);

    int* typeSlot = (int*)JassDecl_TypeSlot(decl);
    m_typeKind = typeSlot[0];
    m_index = index;
    m_typeExtra = typeSlot[1];
}
