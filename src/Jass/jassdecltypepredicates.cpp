//============================================================================
//  Three predicates on a JASS declaration's resolved type slot, all
//  reached through the same `decl+0x18` indirection jassdeclhelpers.cpp's
//  own JassDecl_TypeSlot/JassDecl_Field28 already read - `*(decl+0x18)` is
//  a `Node*` (jassnodes.h), and each of these reads its own `m_kind` field
//  (+0x04) rather than dereferencing further, matching
//  JassDecl_TypeSlot's own `+0x10` and JassDecl_Field28's own `+0x28`
//  offsets past the same pointer. Neither dump names the declaration
//  object's own class, so `decl` stays a raw `void*` here too, the same
//  choice jassdeclhelpers.cpp's own header comment explains.
//
//  Which four/two *type-declaration* kind values 0xC/0x29/0x2A/0x2B and
//  9/6 pick out is not established from this closure - they are read off
//  the same `Node::m_kind` field jassnodes.h's own AST-node table uses,
//  but a type-declaration node's own kind numbering is not shown to be the
//  same numbering as an AST expression node's (e.g. 0x2B is `NodeConst`'s
//  own AST kind there; nothing here confirms a type node ever reuses that
//  same literal for the same reason). Named for the structural role
//  instead of a guessed semantic one.
//============================================================================
#include "jassnodes.h"

//----------------------------------------------------------------------------
//  0x6F4606A0 - `retn 0`.  True when the declaration has a resolved type
//  node whose kind is one of four literals (0xC, 0x29, 0x2A, 0x2B); false
//  when there is no resolved type node at all (no fallback string compare,
//  unlike JassDeclIsHandleType below).
//----------------------------------------------------------------------------
int __fastcall JassDeclTypeKindInGroupA(void* decl)
{
    Node* typeNode = *(Node**)((char*)decl + 0x18);
    if (typeNode != 0)
    {
        int kind = typeNode->m_kind;
        if (kind == 0xC || kind == 0x29 || kind == 0x2A || kind == 0x2B)
            return 1;
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4606D0 - `retn 0`.  Same shape, two literals (9, 6).
//----------------------------------------------------------------------------
int __fastcall JassDeclTypeKindInGroupB(void* decl)
{
    Node* typeNode = *(Node**)((char*)decl + 0x18);
    if (typeNode != 0)
    {
        int kind = typeNode->m_kind;
        if (kind == 9 || kind == 6)
            return 1;
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F460720 - `retn 0`.  A resolved type node answers directly (kind ==
//  0xA); with none resolved yet, fall back to comparing the declaration's
//  own name (`decl+0x14`, the same offset JassSymbolNode's own `m_key`
//  sits at) against the literal string "handle" - the base `handle` type
//  is presumably declared under that exact name, with no resolved-type
//  node of its own to point back at.  `Storm_508` is a bounded compare
//  (declared in Containers/tshash.inl, already visible through
//  storm.h/allochashtable.h's own include chain); the
//  `neg/sbb/add` idiom below turns its "0 means equal" result into a
//  plain boolean without a branch.
//----------------------------------------------------------------------------
int __stdcall Storm_508(const char* a, const char* b, unsigned int count);

int __fastcall JassDeclIsHandleType(void* decl)
{
    Node* typeNode = *(Node**)((char*)decl + 0x18);
    if (typeNode != 0)
        return typeNode->m_kind == 0xA;

    const char* name = *(const char**)((char*)decl + 0x14);
    return Storm_508(name, "handle", 0x7FFFFFFF) == 0;
}
