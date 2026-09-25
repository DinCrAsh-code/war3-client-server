//============================================================================
//  Jass2/Nodes.h - AST node constructors.  jassvm-batch-4.
//
//  Every constructor here is `__thiscall`, non-virtual (`ecx` = the raw
//  memory `CDataAllocator::Alloc()` just handed back, not yet a live
//  object), and inlines its own `Node` base-class construction: the dump
//  stamps `??_7Node@@6B@`, fills in the base fields, then overwrites the
//  vtable pointer a second time with the derived class's own
//  `??_7<Derived>@@6B@` before writing the derived-only fields.  None of
//  this is reached through a vtable slot anywhere in this batch's dumps -
//  every call site is `new(pool) NodeXxx(...)` - so none of it falls under
//  the "CJassFunc vtable, do not touch" rule.
//
//  jassvm-sweep-K5's own note: every struct this file used to define
//  locally now lives in jassnodes.h instead (moved verbatim, ctor bodies
//  byte-for-byte unchanged) - sweep-K5's own destructor reconstructions
//  needed the *type* visible from a second translation unit
//  (jassnodedeleteself.cpp) so `DeleteSelf` could be defined out-of-line
//  there without /Ob2 folding it into `~NodeXxx()` the way
//  escmenu_deleteself.cpp's own header comment warns a same-TU definition
//  would.  This file still *compiles* every one of these constructors
//  (unchanged) and now also the destructors sweep-K5 added for the
//  classes it reconstructed - same TU, since a ctor calling nothing here
//  never risks inlining into its own class's dtor.
//============================================================================
#include "jassnodes.h"

//  0x6F45xxxx/0x6F46xxxx family - every ctor above (jassnodes.h) runs this
//  first.  `Node::Node` itself is `inline` in jassnodes.h (jassvm-batch-5's
//  own note there says why).

//  0x6F4563C0 - out-of-range callee (jassnodes.h's own note on
//  `Node_ReleaseOrList_Owner` explains it); the one out-of-line definition
//  for the whole header's declaration, placed here since NodeFile/NodeLoop
//  below are this sweep's first two callers of it.
__declspec(naked) void Node_ReleaseOrList_Owner::ReleaseOrList()
{
    __asm { mov eax, 06F4563C0h }
    __asm { jmp eax }
}

//============================================================================
//  jassvm-sweep-K5's own additions: the real destructors for thirteen of
//  jassvm-batch-4's classes above (NodeGoal/File/Decl/Locals/GlobalList/
//  StartFunc/Native/Globals/Loop/ExitWhen/Return/Params/Param).  Each
//  releases exactly the fields its own dump reads,
//  each conditionally through the child's own vtable slot 0
//  (`ReleaseChildNode`, jassnodes.h) unless noted otherwise, and every one
//  is SEH-framed in the shipped body - the `__except_handler4`-shaped
//  frame docs/msvc-vc8-idioms.md's own "An `__except_handler4`-shaped
//  frame this toolchain cannot reproduce" note already documents for this
//  exact `push -1 / push offset SEH_<addr> / mov eax,fs:0 / ...` shape, a
//  scope-table tail included - so every one of these is `DIFFERS`
//  regardless of how faithfully the release logic itself matches, not a
//  transcription defect to keep chasing.
//============================================================================

//  0x6F460A60
NodeGoal::~NodeGoal()
{
    ReleaseChildNode((Node**)&m_20);
    ReleaseChildNode((Node**)&m_24);
}

//  0x6F460BB0.  `m_24` is released through the out-of-range
//  `ReleaseChildOrList` helper (jassnodes.h), not the per-field vtable
//  check `m_20` gets - it can itself be a `NodeCode` statement list (a
//  parsed module's own decl/func list).
NodeFile::~NodeFile()
{
    ReleaseChildNode((Node**)&m_20);
    ReleaseChildOrList((Node*)m_24);
}

//  0x6F460CF0
NodeDecl::~NodeDecl()
{
    ReleaseChildNode((Node**)&m_20);
    ReleaseChildNode((Node**)&m_24);
}

//  0x6F462FE0
NodeLocals::~NodeLocals()
{
    ReleaseChildNode((Node**)&m_20);
    ReleaseChildNode((Node**)&m_24);
}

//  0x6F463250
NodeGlobalList::~NodeGlobalList()
{
    ReleaseChildNode((Node**)&m_20);
    ReleaseChildNode((Node**)&m_24);
}

//  0x6F460FD0.  Releases only `m_28` - `m_20`/`m_24` are the parser's own
//  cached name/param-list pointers, never touched by this destructor's
//  own dump.
NodeStartFunc::~NodeStartFunc()
{
    ReleaseChildNode((Node**)&m_28);
}

//  0x6F460E90.  Releases `m_28` then `m_2C`; `m_20`/`m_24` are untouched
//  the same way NodeStartFunc's own name/param fields are.
NodeNative::~NodeNative()
{
    ReleaseChildNode((Node**)&m_28);
    ReleaseChildNode((Node**)&m_2C);
}

//  0x6F463150
NodeGlobals::~NodeGlobals()
{
    ReleaseChildNode((Node**)&m_20);
}

//  0x6F461C90.  Released through the out-of-range `ReleaseChildOrList`
//  helper, not the usual per-field vtable check - a loop body is
//  routinely a `NodeCode` statement list.
NodeLoop::~NodeLoop()
{
    ReleaseChildOrList(m_20);
}

//  0x6F461D70
NodeExitWhen::~NodeExitWhen()
{
    ReleaseChildNode((Node**)&m_20);
}

//  0x6F461E90
NodeReturn::~NodeReturn()
{
    ReleaseChildNode((Node**)&m_20);
}

//  0x6F461FD0.  Shipped order is `m_2C` then `m_20` - the tail-of-list
//  field before the head - reproduced in that order rather than
//  declaration order.
NodeParams::~NodeParams()
{
    ReleaseChildNode((Node**)&m_2C);
    ReleaseChildNode((Node**)&m_20);
}

//  0x6F463580.  Releases only `m_24` - `m_20`/`m_28` are a raw type-extra
//  word and a fixed `-1` sentinel, neither a Node.
NodeParam::~NodeParam()
{
    ReleaseChildNode((Node**)&m_24);
}

//  Force one instance of each so a non-hookable-but-referenced ctor is not
//  optimised away and so its own listing exists to score against; the real
//  callers are the flagship evaluator's own `new` sites elsewhere in this
//  worklist.
extern "C" void JassNodeCtors_Batch4_Reference()
{
    JassCompileContext* p = 0;
    void* a = 0;
    void* b = 0;
    Node* n = 0;
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeGoal(p, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeFile(p, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeDecl(p, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeFuncs(p, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeLocals(p, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeGlobalList(p, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeType(p, 0, a);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeCast(p, a);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeStartFunc(p, a, b, 0);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeNative(p, a, b, 0, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeHandle(p, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeGlobals(p, a);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeConst(p, n, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeSet(p, n, a, b);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeLoop(p, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeExitWhen(p, a);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeReturn(p, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeParams(p, n, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeParam(p, n, a);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeAddr(p, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeNonConst(0, p, n, n, a);
}
