//============================================================================
//  Jass2/Nodes.h - the JASS compiler's AST node hierarchy.
//
//  `Node` is the common base every parse-tree node in this call tree shares:
//  a vtable, a "kind" tag at +0x04 (a literal the shipped code writes as an
//  ASCII-looking byte for many statement/operator kinds - '\'' for a cast,
//  '(' / ',' / '-' / '/' / '0' / '1' / '&' / '.' for others - and a small
//  integer for the rest; no full enum has been recovered, so every derived
//  constructor below just states the literal its own dump shows), a pool
//  pointer copied from the parse context, a parent-context pointer, and two
//  "resolved type" slots plus one more word that different node kinds reuse
//  for different things (a link, a flag, a literal count - each comment
//  says what the specific dump showed at that field).
//
//  Every one of these constructors is `retn <n>` = 4 * (stack-arg count),
//  `__thiscall`, and every one is reached only as `new(sizeof) Node...(...)`
//  from the flagship AST builder (`JassAstBuilder_Owner::Build`) at
//  0x6F464460 or its siblings - confirmed to be a builder, not an
//  evaluator, by jassvm-flagship-I reading ~60 of that function's own
//  opcode handlers case-by-case; see
//  docs/targets/jassvm-flagship-opcode-table.md - none of it is virtual
//  dispatch, so none of it falls under the
//  "never reconstruct CJassFunc's vtable methods" rule.  `Node` itself does
//  have a vtable (every derived ctor stamps `??_7Node@@6B@` before
//  overwriting it with its own), but nothing in this batch's dumps reaches
//  any of its virtual slots directly, so none are declared here - only the
//  constructors these dumps actually show.
//
//  jassvm-batch-5's own addition (NodeAnd through NodeFunction below, plus
//  JassCompileContext's own +0x00/+0x04/+0x2C fields): a disjoint set of
//  node kinds from jassvm-batch-4's own 20 (NodeGoal/File/Decl/... in
//  jassnodectors.cpp) - no address in common, confirmed by name - but the
//  identical `Node` base and parse-context object, so this header now
//  carries both closures' evidence about the one real class each of them
//  independently reconstructed.  `Node::Node` moved `inline` into this
//  header (it was out-of-line in jassnodectors.cpp) so that *every*
//  translation unit constructing a derived Node can fold the base part in
//  when its own dump shows no separate call to it - this batch's own dumps
//  never show one, unlike some of jassvm-batch-4's larger constructors -
//  without changing jassvm-batch-4's own generated code at all (an inline
//  definition is still eligible to stay out-of-line at any call site /Ob2's
//  own cost model declines to fold, which is what a `.cpp`-local
//  definition already gave their own same-TU derived constructors; moving
//  it here only adds the *other* TUs as candidates).
//============================================================================
#ifndef JASSNODES_H
#define JASSNODES_H

#include "storm.h"
#include "dataallocator.h"
#include "rcstring.h"
#include "jassscopetable.h"

//  The JASS parser/compiler context.  Only the fields the two batches'
//  dumps between them actually read or write are named.
//
//  jassvm-batch-5's own reading of +0x00/+0x04/+0x2C: a flag that is
//  non-zero while compiling inside a function body, and two embedded
//  `TAllocatorHashTable<JassSymbolNode,HASHKEY_STR,64>` symbol-scope
//  tables (jassscopetable.h) - local at +0x2C, global at +0x04, each
//  exactly `sizeof(TAllocatorHashTable<...>)` (0x28 bytes) wide, which is
//  what pins the local table's own offset.  See jassscopetable.h for the
//  node type and the table's own methods.
struct JassCompileContext
{
    void*          m_insideFunctionBody;  // +0x00 - non-zero: use the local scope
    JassScopeTable m_globalScope;         // +0x04
    JassScopeTable m_localScope;          // +0x2C
    char           m_reserved54[0xB8 - 0x54];
    void*          m_pool;      // +0xB8 - the arena every node's m_pool is copied from

    //  0x6F45FF00 - jassresolvesymbol.cpp.
    JassSymbolNode* Resolve(const char* key);

    //  0x6F464420 - jassdeclaresymbol.cpp.
    JassSymbolNode* Declare(const char* key);
};

//  0x6FAB72A8 - the pool every `new Node...()` in this batch's dumps
//  allocates from (`CDataAllocator::Alloc(0, "Jass2/Nodes.h", 0)` at every
//  call site).  Declared, not defined - see funcmap.py DATA.
extern CDataAllocator g_jassNodePool;

//  Storm.dll ordinal 403 - the same declaration escmenu_deleteself.cpp's
//  own `CEscMenu::DeleteSelf` already carries for this exact shape.
//  Needed ahead of `Node` itself now that `Node::DeleteSelf`'s own
//  fallback body (below) calls it directly.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

//  0x6F45xxxx/0x6F46xxxx family - abstract JASS AST node base.  sizeof == 0x20.
//
//  `virtual` here is not a guess at a specific slot - nothing in either
//  batch's dumps calls through Node's vtable - it is what makes the
//  compiler emit a vtable at all, so that its mangled name
//  (`??_7Node@@6B@`, IDA's own name for it) comes out of the compiler
//  rather than being hand-written as a hex literal.  Per
//  docs/msvc-vc8-idioms.md's "A vtable stamp is one instruction" note, a
//  hex literal is the fallback for a vtable this repo cannot declare a
//  class for; here the class is ours to declare, so the real spelling
//  reproduces the dump's own `mov [reg], offset ??_7...@@6B@` for free.
struct Node
{
    int                  m_kind;       // +0x04 - node/operator tag literal
    void*                m_pool;       // +0x08 - parent->m_pool
    JassCompileContext*  m_parent;     // +0x0C
    int                  m_typeKind;   // +0x10 - resolved type slot #1, default 0
    int                  m_typeExtra;  // +0x14 - resolved type slot #2, default 0
    int                  m_flags18;    // +0x18 - reused per-kind (link/flag/count), default 0
    int                  m_reserved1C; // +0x1C - no dump in either batch ever writes it (every
                                        // derived field the dumps DO show starts at +0x20, one
                                        // word past m_flags18), so it is presumed to rely on the
                                        // allocator's own zero-fill rather than being set here.

    //  jassvm-sweep-K5's own addition: Node's *actual* single virtual slot.
    //  Every one of the 0x6F460960-0x6F464140 family's own destructor pairs
    //  this sweep reconstructs (jassnodectors.cpp/jassnodedtors.cpp's own
    //  `~NodeXxx()`, jassnodedeleteself.cpp's own `NodeXxx::DeleteSelf`)
    //  confirms this directly: every recursive child release goes
    //  `mov eax,[ecx]; mov edx,[eax]; push 0; call edx` - vtable slot 0,
    //  one `int` argument - then the freshly-reloaded field is handed to
    //  `CDataAllocator::Free`.  That is the MSVC "scalar deleting
    //  destructor" shape (run the real dtor, and only when `flags&1` also
    //  release the object's own storage - here via `SMemFree`, not the
    //  pool `Free` the *children* go through), spelled out exactly the way
    //  `PositionTrack::DeleteSelf` already is in this repo
    //  (Pathfinding/positiontrack.h) rather than left for the compiler to
    //  synthesize invisibly.  Node is replacing the older placeholder
    //  `virtual ~Node() {}` this header had - nothing before this sweep
    //  ever called through Node's vtable (this header's own top note said
    //  so), so no already-scored instruction stream depended on which
    //  virtual occupied the slot, only on the slot existing at all.  Node
    //  itself is never directly `new`'d in any dump either batch has
    //  shown.  Not pure virtual: every class in this header that this
    //  sweep did *not* reconstruct a destructor for (NodeAnd, NodeType,
    //  NodeVarBase's own leaves, ...) still needs to be instantiable from
    //  the two `JassNodeCtors_Batch{4,5}_Reference` forcing functions, so
    //  Node gives the generic "run ~Node(), then SMemFree `this` when
    //  flags&1" shape as its own fallback body - the same shape every
    //  leaf's own override below has, minus any field release, rather
    //  than making the whole hierarchy abstract over one class this sweep
    //  had no dump for.
    virtual Node* DeleteSelf(int flags)
    {
        this->~Node();
        if ((flags & 1) && this != 0)
            SMemFree(this, "delete", -1, 0);
        return this;
    }

    //  0x6F455C80 - jscript-bin1's own addition: kept out of line (declared,
    //  not defined inline) because that is what the shipped code does too -
    //  NodeDebug::~NodeDebug (0x6F460960) and every other derived
    //  destructor's own base part end with a genuine `jmp` here rather than
    //  repeating the one vtable-reset store inline.  An empty body still
    //  compiles to exactly that store: MSVC always re-stamps a polymorphic
    //  class's own vtable pointer at the top of every destructor in its
    //  chain, precisely so a virtual call made from inside a base
    //  destructor dispatches to that base's own override rather than a
    //  derived one already torn down - here there is nothing else to do
    //  because Node itself owns no members of its own to release (every
    //  derived leaf releases its own fields before this runs).  Defined in
    //  its own translation unit (jassnodebase.cpp), not next to any derived
    //  destructor, to match the address neighbourhood - 0x6F455C80 sits
    //  well outside jassnodectors.cpp/jassnodedtors.cpp's own
    //  0x6F45FD60-0x6F466F50 range, so this is genuinely a different
    //  original module.
    ~Node();

    //  Every derived constructor's own base part - see this header's own
    //  top note on why this is `inline` rather than out-of-line.
    Node(JassCompileContext* parent, int kind)
        : m_kind(kind), m_pool(parent->m_pool), m_parent(parent),
          m_typeKind(0), m_typeExtra(0), m_flags18(0)
    {
    }

    //  0x6F462480 - jassvm-batch-4's own naked thunk
    //  (`Node_CoerceOperandTypes_Owner::CoerceOperandTypes`,
    //  jassvmbatch4_thunks.cpp) to the real, unhooked body - not
    //  reconstructed by either batch.  Reached through that exact class
    //  and member name (`Node_CoerceOperandTypes_Owner` is forward-declared
    //  again below, `this` reinterpreted rather than a second function),
    //  because MSVC mangles the class name into the symbol and a
    //  differently-named re-declaration would link to nothing -
    //  docs/msvc-vc8-idioms.md's "Declaring imports so the diff can see
    //  them" note on why one spelling has to be everywhere.
    void TryFold(JassCompileContext* ctx);
};

struct Node_CoerceOperandTypes_Owner { void CoerceOperandTypes(void*); };

inline void Node::TryFold(JassCompileContext* ctx)
{
    ((Node_CoerceOperandTypes_Owner*)this)->CoerceOperandTypes(ctx);
}

//----------------------------------------------------------------------------
//  jassvm-sweep-K5's own addition: the release helpers every `~NodeXxx()`
//  in jassnodectors.cpp/jassnodedtors.cpp shares, plus the one out-of-range
//  callee two of them need.
//----------------------------------------------------------------------------

//  Release one owned child `Node*` living at `*slot`: run its own
//  `DeleteSelf(0)` through its own vtable (so the *dynamic* type's real
//  destructor runs, not a fixed one), then hand the raw storage back to
//  the shared node pool.  Every `~NodeXxx()` this sweep reconstructs
//  re-reads `*slot` a second time for the `Free` call rather than reusing
//  the value already in a register - that is in every one of these dumps,
//  because MSVC treats the intervening virtual call as capable of writing
//  through `self` and reloads rather than trusts the stale copy.
__forceinline void ReleaseChildNode(Node** slot)
{
    Node* child = *slot;
    if (child)
    {
        child->DeleteSelf(0);
        g_jassNodePool.Free(*slot, 0, 0);
    }
}

//  0x6F4563C0 - `retn 0`, `__thiscall`.  Release one Node, or - when its
//  own `m_kind` is 4 (`NodeCode`, a statement list) - walk its own +0x20
//  chain releasing every element the same way `ReleaseChildNode` above
//  does for a single scalar child.  Does its own null check internally
//  (unlike `ReleaseChildNode`, every call site below passes the field
//  straight through with no `test`/`jz` of its own).  Well outside this
//  sweep's own address range (0x6F45FD60-0x6F466F50); declared and called
//  exactly as shipped, the same way jassvmbatch4_thunks.cpp's own
//  Node_ContainsMember_Owner/Node_PropagateChildType_Owner declare their
//  own out-of-batch callees, not reconstructed here.
//  Declaration only - defined once, out-of-line, in jassnodectors.cpp
//  (this header is included by several translation units, and a naked
//  body inline here would be a multiply-defined symbol at link time).
struct Node_ReleaseOrList_Owner { void ReleaseOrList(); };

__forceinline void ReleaseChildOrList(Node* node)
{
    ((Node_ReleaseOrList_Owner*)node)->ReleaseOrList();
}

//============================================================================
//  jassvm-batch-5's own closure: NodeAnd through NodeFunction below.  Each
//  constructor's own address comment gives its target; field roles common
//  to more than one are noted once, at the first class that needs them.
//============================================================================

//----------------------------------------------------------------------------
//  Binary boolean connectives.  Kind 0x11 = AND, 0x12 = OR; both
//  known-BOOLEAN (8) at parse time.
//----------------------------------------------------------------------------
struct NodeAnd : Node
{
    Node* m_left;    // +0x20
    Node* m_right;   // +0x24

    //  0x6F462670
    NodeAnd(JassCompileContext* ctx, Node* left, Node* right)
        : Node(ctx, 0x11), m_left(left), m_right(right)
    {
        m_typeKind = 8;
    }
};

struct NodeOr : Node
{
    Node* m_left;
    Node* m_right;

    //  0x6F4626F0
    NodeOr(JassCompileContext* ctx, Node* left, Node* right)
        : Node(ctx, 0x12), m_left(left), m_right(right)
    {
        m_typeKind = 8;
    }
};

//----------------------------------------------------------------------------
//  0x6F462770 - a relational compare.  `op` is the class's own kind
//  (comparison token), read out of `ctx`'s own +0xB8 the same as every
//  other constructor - `ctx` is the *second* argument here, not the first.
//  SEH-framed in the shipped body (docs/msvc-vc8-idioms.md's
//  "__except_handler4-shaped frame this toolchain cannot reproduce" - the
//  fold helper below can throw), so this is DIFFERS regardless of how
//  faithfully the field stores are reproduced.  Ends with a call into the
//  constant-folder, `Node::TryFold` below.
//----------------------------------------------------------------------------
struct NodeCompare : Node
{
    Node* m_left;    // +0x20
    Node* m_right;   // +0x24

    NodeCompare(int op, JassCompileContext* ctx, Node* left, Node* right)
        : Node(ctx, op), m_left(left), m_right(right)
    {
        TryFold(ctx);
    }
};

//----------------------------------------------------------------------------
//  0x6F462A50 - a binary arithmetic operator.  Same (op, ctx, left, right)
//  shape, the same SEH frame gap and the same fold-helper call as
//  NodeCompare above.
//----------------------------------------------------------------------------
struct NodeMath : Node
{
    Node* m_left;
    Node* m_right;

    NodeMath(int op, JassCompileContext* ctx, Node* left, Node* right)
        : Node(ctx, op), m_left(left), m_right(right)
    {
        TryFold(ctx);
    }
};

//----------------------------------------------------------------------------
//  0x6F462C50 - `%`.  Its own class rather than a NodeMath kind (0x17),
//  and its type slot is left at zero - unlike NodeMath/NodeCompare it never
//  calls the fold helper, so nothing here resolves its type early.  Not
//  SEH-framed (no callee that could throw through it), unlike its two
//  siblings above.
//----------------------------------------------------------------------------
struct NodeMod : Node
{
    Node* m_left;
    Node* m_right;

    //  0x6F462C50
    NodeMod(JassCompileContext* ctx, Node* left, Node* right)
        : Node(ctx, 0x17), m_left(left), m_right(right)
    {
    }
};

//----------------------------------------------------------------------------
//  Leaf literals.  Each is (ctx[, value]); NodeNull has no value slot.
//----------------------------------------------------------------------------
struct NodeBool : Node
{
    int m_value;   // +0x20

    //  0x6F462DE0, kind 0x20, type 8
    NodeBool(JassCompileContext* ctx, int value)
        : Node(ctx, 0x20), m_value(value)
    {
        m_typeKind = 8;
    }
};

struct NodeNull : Node
{
    //  0x6F462E50, kind 0x21, type 2
    explicit NodeNull(JassCompileContext* ctx)
        : Node(ctx, 0x21)
    {
        m_typeKind = 2;
    }
};

struct NodeInt : Node
{
    int m_value;   // +0x20

    //  0x6F462EB0, kind 0x23, type 4
    NodeInt(JassCompileContext* ctx, int value)
        : Node(ctx, 0x23), m_value(value)
    {
        m_typeKind = 4;
    }
};

struct NodeReal : Node
{
    float m_value;   // +0x20

    //  0x6F462F20, kind 0x24, type 5.  `value` never touches the FPU here -
    //  the shipped body is a plain 4-byte register move, the same as every
    //  other leaf's literal slot.
    NodeReal(JassCompileContext* ctx, float value)
        : Node(ctx, 0x24), m_value(value)
    {
        m_typeKind = 5;
    }
};

//----------------------------------------------------------------------------
//  0x6F463DB0 - a string literal.  Unescapes `\\`, `\'`, `\"`, `\r`, `\n`,
//  `\b`, `\f`, `\t` in place into a stack buffer and hands that to an
//  embedded RCString at +0x20 (RCString::Construct, rcstring.h) - not a
//  pointer, a real member.  SEH-framed for the same reason NodeCompare is
//  (a 0x400-byte local plus the placement-constructed RCString both earn
//  the frame this build's fixed /GS- /EHs-c- does not emit).
//----------------------------------------------------------------------------
struct NodeString : Node
{
    RCString m_text;   // +0x20

    NodeString(JassCompileContext* ctx, const char* text);   // kind 0x22
};

//----------------------------------------------------------------------------
//  0x6F462D60/0x6F462CD0 - the two unary operators, and a real three-level
//  chain: both streams stamp `Node`, then `NodeUnary`, then their own leaf
//  vtable, in that order - `NodeUnary` is not a guess, it is a symbol the
//  dump names directly.  `NodeUnary`'s own part sets the operand slot and
//  copies the operand's *own* already-resolved type into this node's - a
//  unary op's type is its operand's type, known immediately if the
//  operand's was - which both NodeNot and NodeUMinus inherit unchanged, so
//  each of their own constructors is empty.
//----------------------------------------------------------------------------
struct NodeUnary : Node
{
    Node* m_operand;   // +0x20

    NodeUnary(JassCompileContext* ctx, int kind, Node* operand)
        : Node(ctx, kind), m_operand(operand)
    {
        m_typeKind = operand->m_typeKind;
        m_typeExtra = operand->m_typeExtra;
    }
};

struct NodeNot : NodeUnary
{
    //  0x6F462D60
    NodeNot(JassCompileContext* ctx, Node* operand)
        : NodeUnary(ctx, 0x1F, operand) {}
};

struct NodeUMinus : NodeUnary
{
    //  0x6F462CD0
    NodeUMinus(JassCompileContext* ctx, Node* operand)
        : NodeUnary(ctx, 0x1E, operand) {}
};

//----------------------------------------------------------------------------
//  0x6F4637F0 - a variable reference.  `decl` is the resolved symbol (the
//  declaration jassdeclhelpers.cpp's JassDecl_TypeSlot reaches through);
//  its own resolved type slot is copied into this node's, the same as
//  NodeUnary does for its operand.  `index` is the array-subscript
//  expression, or null.  SEH-framed like NodeCompare above.
//----------------------------------------------------------------------------
struct NodeVar : Node
{
    Node* m_index;   // +0x20
    void* m_decl;    // +0x24

    NodeVar(JassCompileContext* ctx, void* decl, Node* index);   // kind 0x25
};

//----------------------------------------------------------------------------
//  0x6F462250 - one link of a call's argument list.  `next` chains toward
//  the call's own outer argument list (or is null for the last one); the
//  count at +0x20 is `next ? next->m_count + 1 : 1` - the list's own length
//  counted from the tail, which is why it has to walk `next` to compute it
//  rather than being handed a count.
//----------------------------------------------------------------------------
struct NodeArgList : Node
{
    unsigned int  m_count;   // +0x20
    Node*         m_value;   // +0x24
    NodeArgList*  m_next;    // +0x28

    //  kind 0xF
    NodeArgList(JassCompileContext* ctx, Node* value, NodeArgList* next)
        : Node(ctx, 0xF),
          m_count(next ? next->m_count + 1 : 1),
          m_value(value), m_next(next)
    {
    }

    //  0x6F4622C0 - jassnodedeleteself.cpp.  0x6F462360 (this class's own
    //  DeleteSelf duplicate) - jassnodedeleteself.cpp.  Releases `m_value`
    //  then `m_next`, each conditionally through its own vtable slot 0 -
    //  see jassnodedtors.cpp's own `~NodeArgList()`.
    ~NodeArgList();
    virtual Node* DeleteSelf(int flags);
};

//----------------------------------------------------------------------------
//  0x6F460920 - a `debug` statement wrapper.  `body` is at +0x20.  Kind 0,
//  unlike every other constructor here - the shipped body stores a literal
//  zero rather than a per-class tag.
//----------------------------------------------------------------------------
struct NodeDebug : Node
{
    Node* m_body;   // +0x20

    NodeDebug(JassCompileContext* ctx, Node* body)
        : Node(ctx, 0), m_body(body)
    {
    }

    //  0x6F460960 / 0x6F4609E0 (DeleteSelf) - jassnodedtors.cpp /
    //  jassnodedeleteself.cpp.  Releases `m_body` conditionally through
    //  its own vtable slot 0.
    ~NodeDebug();
    virtual Node* DeleteSelf(int flags);
};

//----------------------------------------------------------------------------
//  0x6F461090 - a function-reference/call node, kind 7.  `ctx` here
//  (+0x80 of it) is read unconditionally into +0x30 - almost certainly the
//  enclosing function-declaration's own cached return-type field, though
//  nothing later in the AST is in this batch's closure to confirm it.
//  When an argument list was actually parsed (`args != 0`), its own
//  already-resolved type slot (+0x10/+0x14) is copied not only into this
//  node's own type slot but *back into* the callee declaration object
//  (`decl`) as well - a real double write the shipped code makes, not a
//  transcription accident, and there is nothing in scope here to say why.
//----------------------------------------------------------------------------
struct NodeFunction : Node
{
    void*         m_decl;       // +0x20
    NodeArgList*  m_args;       // +0x24
    void*         m_extra;      // +0x28
    void*         m_reserved2C; // +0x2C, always zeroed here
    void*         m_returnType; // +0x30 - copied from ctx->+0x80

    NodeFunction(JassCompileContext* ctx, void* decl, NodeArgList* args,
                 void* extra)
        : Node(ctx, 7), m_decl(decl), m_args(args), m_extra(extra),
          m_reserved2C(0)
    {
        m_returnType = *(void**)((char*)ctx + 0x80);

        if (args != 0)
        {
            m_typeKind = args->m_typeKind;
            m_typeExtra = args->m_typeExtra;

            *(int*)((char*)decl + 0x10) = args->m_typeKind;
            *(int*)((char*)decl + 0x14) = args->m_typeExtra;
        }
    }

    //  0x6F461110 / 0x6F4612B0 (DeleteSelf) - jassnodedtors.cpp /
    //  jassnodedeleteself.cpp.  Releases `m_decl`, `m_args`, `m_extra`
    //  (each conditionally through its own vtable slot 0, the same as
    //  every sibling in this header), then `m_reserved2C` unconditionally
    //  through the out-of-range `ReleaseChildOrList` helper above (it does
    //  its own null check) - the one field this class's own ctor above
    //  says is "always zeroed here", so this is very likely a dead
    //  release in every dump either batch has actually seen, not evidence
    //  the field is ever non-null in practice.
    ~NodeFunction();
    virtual Node* DeleteSelf(int flags);
};

//----------------------------------------------------------------------------
//  jassvm-sweep-K5's own addition: three classes whose real constructors
//  remain thunked (jassvmbatch4_thunks.cpp's own `NodeIf_Owner::ctor`/
//  `NodeCode_Owner::ctor`/`NodeCall_Owner::ctor`, none of which give a
//  full field layout - each is declared as a bare placeholder struct
//  purely to spell the thunk's `__thiscall`), so no constructor is
//  declared here at all - nothing in this sweep's own dumps ever
//  constructs one, and an implicitly-deleted default constructor is not
//  an error unless something tries to call it.  Every field below is
//  named only where a real destructor dump in jassnodedtors.cpp actually
//  reads it; the rest is reserved padding.  Real fields established
//  elsewhere (NodeCall's own args, NodeIf's own condition) are left for
//  whichever session reconstructs the real constructor.
//----------------------------------------------------------------------------

//  0x6F461B00 / 0x6F461C00 (DeleteSelf).  NodeIf (kind unconfirmed - the
//  real ctor is still `NodeIf_Owner::ctor`, 0x6F4617C0, thunked).
//  Releases +0x20 conditionally through its own vtable slot 0, then +0x24
//  and +0x28 unconditionally through the out-of-range
//  `ReleaseChildOrList` helper (each branch of the `if` can itself be a
//  `NodeCode` statement list, hence the list-aware release) - see
//  jassnodedtors.cpp's own `~NodeIf()`.
struct NodeIf : Node
{
    Node* m_reserved20;   // +0x20 - condition
    void* m_reserved24;   // +0x24 - then-branch (Node or NodeCode list)
    void* m_reserved28;   // +0x28 - else-branch (Node or NodeCode list, or null)

    ~NodeIf();
    virtual Node* DeleteSelf(int flags);
};

//  0x6F4616B0 / 0x6F461790 (DeleteSelf).  NodeCode (kind 4, per
//  jassvmbatch4_thunks.cpp's own note on its still-thunked ctor,
//  0x6F4614D0) - the statement-list class `ReleaseChildOrList` above
//  walks into whenever some other node's own branch/body field turns out
//  to hold one.  Releases only +0x24 conditionally through its own vtable
//  slot 0 - see jassnodedtors.cpp's own `~NodeCode()`.
struct NodeCode : Node
{
    char  m_reserved20[0x24 - 0x20];
    void* m_reserved24;   // +0x24 - the statement list's own head/tail

    ~NodeCode();
    virtual Node* DeleteSelf(int flags);
};

//  0x6F463710 / 0x6F463790 (DeleteSelf).  NodeCall (kind 0xE, per
//  jassvmbatch4_thunks.cpp's own note on its still-thunked ctor,
//  0x6F463630).  Releases only +0x30 conditionally through its own
//  vtable slot 0 - see jassnodedtors.cpp's own `~NodeCall()`.
struct NodeCall : Node
{
    char  m_reserved20[0x30 - 0x20];
    void* m_reserved30;   // +0x30 - the resolved argument list, most likely

    ~NodeCall();
    virtual Node* DeleteSelf(int flags);
};

//============================================================================
//  jassvm-batch-4's own closure, moved here from jassnodectors.cpp (that
//  file still defines every one of these constructors, out-of-class -
//  jassvm-sweep-K5 only needed the *type* visible from a second
//  translation unit so `~NodeXxx()`/`DeleteSelf` could be defined out of
//  line there without risking /Ob2 folding them into each other the way
//  escmenu_deleteself.cpp's own header comment warns about).  Nothing
//  about any already-scored constructor's own instructions changes: same
//  file compiles them (jassnodectors.cpp), same inline bodies, only the
//  type declaration's address moved.
//============================================================================
struct NodeGoal : Node
{
    void* m_20;
    void* m_24;
    //  0x6F460A10
    NodeGoal(JassCompileContext* parent, void* a, void* b)
        : Node(parent, 1), m_20(a), m_24(b) {}

    //  0x6F460A60 / 0x6F460B30 (DeleteSelf) - jassnodectors.cpp /
    //  jassnodedeleteself.cpp.
    ~NodeGoal();
    virtual Node* DeleteSelf(int flags);
};

struct NodeFile : Node
{
    void* m_20;
    void* m_24;
    //  0x6F460B60
    NodeFile(JassCompileContext* parent, void* a, void* b)
        : Node(parent, 2), m_20(a), m_24(b) {}

    //  0x6F460BB0 / 0x6F460C70 (DeleteSelf).  `m_24` releases through the
    //  out-of-range `ReleaseChildOrList` helper (jassnodes.h), not the
    //  usual per-field vtable check - it can itself be a `NodeCode`
    //  statement list.
    ~NodeFile();
    virtual Node* DeleteSelf(int flags);
};

struct NodeDecl : Node
{
    void* m_20;
    void* m_24;
    //  0x6F460CA0
    NodeDecl(JassCompileContext* parent, void* a, void* b)
        : Node(parent, 3), m_20(a), m_24(b) {}

    //  0x6F460CF0 / 0x6F460DF0 (DeleteSelf).
    ~NodeDecl();
    virtual Node* DeleteSelf(int flags);
};

struct NodeFuncs : Node
{
    void* m_20;
    void* m_24;
    //  0x6F461350
    NodeFuncs(JassCompileContext* parent, void* a, void* b)
        : Node(parent, 5), m_20(a), m_24(b) {}
};

struct NodeLocals : Node
{
    void* m_20;
    void* m_24;
    //  0x6F462F90
    NodeLocals(JassCompileContext* parent, void* a, void* b)
        : Node(parent, 0x28), m_20(a), m_24(b) {}

    //  0x6F462FE0 / 0x6F4630E0 (DeleteSelf).
    ~NodeLocals();
    virtual Node* DeleteSelf(int flags);
};

struct NodeGlobalList : Node
{
    void* m_20;
    void* m_24;
    //  0x6F463200
    NodeGlobalList(JassCompileContext* parent, void* a, void* b)
        : Node(parent, 0x2D), m_20(a), m_24(b) {}

    //  0x6F463250 / 0x6F463350 (DeleteSelf).
    ~NodeGlobalList();
    virtual Node* DeleteSelf(int flags);
};

//----------------------------------------------------------------------------
//  NodeType (0x6F4621E0) - kind 0x10.  Both trailing args are stored a
//  second time into the base's own type slots (`arg_8` lands at both
//  `m_own` and `m_typeExtra`) - the shipped body genuinely writes it twice.
//----------------------------------------------------------------------------
struct NodeType : Node
{
    void* m_own;   // +0x20
    //  0x6F4621E0
    NodeType(JassCompileContext* parent, int typeKind, void* typeExtra)
        : Node(parent, 0x10), m_own(typeExtra)
    {
        m_typeKind = typeKind;
        m_typeExtra = (int)typeExtra;
    }
};

//----------------------------------------------------------------------------
//  NodeCast (0x6F455D00) - kind 0x27 ('\'').  Fixed type: every cast node
//  resolves to type-tag 5, never inferred from its operand.
//----------------------------------------------------------------------------
struct NodeCast : Node
{
    void* m_value;   // +0x20
    //  0x6F455D00
    NodeCast(JassCompileContext* parent, void* value)
        : Node(parent, 0x27), m_value(value)
    {
        m_typeKind = 5;
    }
};

//----------------------------------------------------------------------------
//  NodeStartFunc (0x6F460F70) - kind 6.  Stashes its own second argument
//  into the *parent* context's own +0x94 field as well as its own +0x20 -
//  the parser's "current function" slot, most likely.
//----------------------------------------------------------------------------
struct NodeStartFunc : Node
{
    void* m_20;
    void* m_24;
    void* m_28;
    //  0x6F460F70
    NodeStartFunc(JassCompileContext* parent, void* a4, void* a8, void* aC)
        : Node(parent, 6), m_20(a4), m_24(a8), m_28(aC)
    {
        *(void**)((char*)parent + 0x94) = a4;
    }

    //  0x6F460FD0 / 0x6F461060 (DeleteSelf).  Releases only `m_28`.
    ~NodeStartFunc();
    virtual Node* DeleteSelf(int flags);
};

//----------------------------------------------------------------------------
//  NodeNative (0x6F460E20) - kind 9.  `a8`'s own +0x18 becomes a
//  back-pointer to this node (the same "owner" idiom NodeHandle/NodeParam
//  below share), and the type slot is copied from `a10` only when it is
//  non-null.
//----------------------------------------------------------------------------
struct NodeNative : Node
{
    void* m_20;
    void* m_24;
    void* m_28;
    void* m_2C;
    //  0x6F460E20
    NodeNative(JassCompileContext* parent, void* a4, void* a8, void* aC, Node* a10)
        : Node(parent, 9), m_20(a4), m_24(a8), m_28(aC), m_2C(a10)
    {
        *(void**)((char*)a8 + 0x18) = this;
        if (a10)
        {
            m_typeKind = a10->m_typeKind;
            m_typeExtra = a10->m_typeExtra;
        }
    }

    //  0x6F460E90 / 0x6F460F40 (DeleteSelf).  Releases `m_28` then `m_2C`.
    ~NodeNative();
    virtual Node* DeleteSelf(int flags);
};

//----------------------------------------------------------------------------
//  NodeHandle (0x6F4612E0) - kind 0xA.  Fixed type-kind 7 (a handle type
//  tag); the type-extra slot doubles as the raw +0x24 payload.
//----------------------------------------------------------------------------
struct NodeHandle : Node
{
    void* m_20;
    void* m_24;
    //  0x6F4612E0
    NodeHandle(JassCompileContext* parent, void* a4, void* a8)
        : Node(parent, 0xA), m_20(a4), m_24(a8)
    {
        *(void**)((char*)a4 + 0x18) = this;
        m_typeKind = 7;
        m_typeExtra = (int)a8;
    }
};

//----------------------------------------------------------------------------
//  NodeGlobals (0x6F463110) - kind 0x2C.  Plain: one payload word, no type
//  override.
//----------------------------------------------------------------------------
struct NodeGlobals : Node
{
    void* m_20;
    //  0x6F463110
    NodeGlobals(JassCompileContext* parent, void* a4)
        : Node(parent, 0x2C), m_20(a4) {}

    //  0x6F463150 / 0x6F4631D0 (DeleteSelf).
    ~NodeGlobals();
    virtual Node* DeleteSelf(int flags);
};

//----------------------------------------------------------------------------
//  NodeVarBase - an intermediate class between Node and {NodeConst,
//  NodeSet, NodeNonConst} that this batch's dumps only ever see fully
//  inlined into its own leaf ctor.  No data member of its own has been
//  recovered (each leaf below stores its "VarBase-level" fields at
//  different literal offsets with different source expressions, so they
//  are declared on the leaf, not here) - this exists purely so the
//  intermediate `??_7NodeVarBase@@6B@` stamp the dumps show has a class to
//  come from.
//----------------------------------------------------------------------------
struct NodeVarBase : Node
{
    NodeVarBase(JassCompileContext* parent, int kind) : Node(parent, kind) {}
};

//----------------------------------------------------------------------------
//  NodeConst (0x6F4640D0) - kind 0x2B ('+').  `a8`'s own +0x18 becomes a
//  back-pointer to this node; the resolved type is copied from `a4`.
//----------------------------------------------------------------------------
struct NodeConst : NodeVarBase
{
    void* m_20;
    int   m_24;
    void* m_28;
    void* m_2C;
    //  0x6F4640D0
    NodeConst(JassCompileContext* parent, Node* a4, void* a8, void* aC)
        : NodeVarBase(parent, 0x2B), m_20(a4), m_24(*(int*)((char*)a8 + 0x14)),
          m_28(0), m_2C(aC)
    {
        m_typeKind = a4->m_typeKind;
        m_typeExtra = a4->m_typeExtra;
        *(void**)((char*)a8 + 0x18) = this;
    }
};

//----------------------------------------------------------------------------
//  NodeSet (0x6F464340) - kind 0x31 ('1').  Its own "target" field is
//  `a4->+0x18` when `a4` is non-null, else null - the resolved type then
//  comes from *that* pointer, not from `a4` directly.
//----------------------------------------------------------------------------
struct NodeSet : NodeVarBase
{
    void* m_20;
    int   m_24;
    void* m_28;
    void* m_2C;
    //  0x6F464340
    NodeSet(JassCompileContext* parent, Node* a4, void* a8, void* aC)
        : NodeVarBase(parent, 0x31),
          m_20(a4 ? *(void**)((char*)a4 + 0x18) : (void*)0),
          m_24(a4 ? *(int*)((char*)a4 + 0x14) : 0),
          m_28(a8), m_2C(aC)
    {
        Node* target = (Node*)m_20;
        m_typeKind = target->m_typeKind;
        m_typeExtra = target->m_typeExtra;
    }
};

//----------------------------------------------------------------------------
//  NodeNonConst (0x6F4641F0) - the fourth (and last, in this batch)
//  NodeVarBase leaf.  Unlike NodeConst/NodeSet, its *kind* literal is not
//  baked into this ctor at all - it arrives as this constructor's own
//  first stack argument, so one shared body serves whatever set of kinds
//  the flagship evaluator's call sites pass it (their literals are not in
//  this batch's dumps).  `a8`'s type is copied onto this node directly (not
//  through a conditional the way NodeSet's is), and `aC`'s own +0x18
//  becomes a back-pointer to this node.
//----------------------------------------------------------------------------
struct NodeNonConst : NodeVarBase
{
    Node* m_20;
    int   m_24;
    void* m_28;
    void* m_2C;
    //  0x6F4641F0
    NodeNonConst(int kind, JassCompileContext* parent, Node* a8, Node* aC, void* a10)
        : NodeVarBase(parent, kind), m_20(a8), m_24(aC->m_typeExtra),
          m_28(0), m_2C(a10)
    {
        m_typeKind = a8->m_typeKind;
        m_typeExtra = a8->m_typeExtra;
        *(void**)((char*)aC + 0x18) = this;
    }
};

//----------------------------------------------------------------------------
//  Six single-child statement kinds, all `Node(parent, kind)` plus a
//  `m_20` child pointer; some additionally propagate the child's own
//  resolved type/flag word onto themselves.
//----------------------------------------------------------------------------

//  NodeLoop (0x6F461C30) - kind 0x2F ('/').  Adopts the body's own
//  type/flag triple when the body is present.
struct NodeLoop : Node
{
    Node* m_20;
    //  0x6F461C30
    NodeLoop(JassCompileContext* parent, Node* body)
        : Node(parent, 0x2F), m_20(body)
    {
        if (body)
        {
            m_flags18 = body->m_flags18;
            m_typeKind = body->m_typeKind;
            m_typeExtra = body->m_typeExtra;
        }
    }

    //  0x6F461C90 / 0x6F461D00 (DeleteSelf).  Releases `m_20` through the
    //  out-of-range `ReleaseChildOrList` helper (jassnodes.h), not the
    //  usual per-field vtable check - a loop body is routinely a
    //  `NodeCode` statement list.
    ~NodeLoop();
    virtual Node* DeleteSelf(int flags);
};

//  NodeExitWhen (0x6F461D30) - kind 0x30 ('0').  Plain.
struct NodeExitWhen : Node
{
    void* m_20;
    //  0x6F461D30
    NodeExitWhen(JassCompileContext* parent, void* cond)
        : Node(parent, 0x30), m_20(cond) {}

    //  0x6F461D70 / 0x6F461E00 (DeleteSelf).
    ~NodeExitWhen();
    virtual Node* DeleteSelf(int flags);
};

//  NodeReturn (0x6F461E30) - kind 8.  `m_flags18` is 1 for a bare
//  `return`, 2 for `return <expr>` (which also adopts the expression's
//  resolved type).
struct NodeReturn : Node
{
    Node* m_20;
    //  0x6F461E30
    NodeReturn(JassCompileContext* parent, Node* expr)
        : Node(parent, 8), m_20(expr)
    {
        if (expr)
        {
            m_flags18 = 2;
            m_typeKind = expr->m_typeKind;
            m_typeExtra = expr->m_typeExtra;
        }
        else
        {
            m_flags18 = 1;
        }
    }

    //  0x6F461E90 / 0x6F461F10 (DeleteSelf).
    ~NodeReturn();
    virtual Node* DeleteSelf(int flags);
};

//  NodeParams (0x6F461F50) - kind 0xB.  A parameter-list cons cell: `a4`
//  is the parameter, `a8` the rest of the list (or null at the tail).
//  Both this node and `a4` itself get told the 1-based position from the
//  tail; the resolved type is copied from `a4`.
struct NodeParams : Node
{
    Node* m_20;
    int   m_24;
    int   m_28;
    Node* m_2C;
    //  0x6F461F50
    NodeParams(JassCompileContext* parent, Node* a4, Node* a8)
        : Node(parent, 0xB),
          m_20(a4),
          m_24(a8 ? *(int*)((char*)a8 + 0x24) + 1 : 1),
          m_28(0), m_2C(a8)
    {
        m_28 = m_24;
        *(int*)((char*)a4 + 0x28) = m_24;
        m_typeKind = a4->m_typeKind;
        m_typeExtra = a4->m_typeExtra;
    }

    //  0x6F461FD0 / 0x6F4621B0 (DeleteSelf).  Releases `m_2C` then `m_20`
    //  (that order - the shipped body releases the tail-of-list field
    //  before the head), each conditionally through its own vtable slot 0.
    ~NodeParams();
    virtual Node* DeleteSelf(int flags);
};

//  NodeParam (0x6F463520) - kind 0xC.  `a8`'s own +0x18 becomes a
//  back-pointer to this node; `m_20` is `a8`'s resolved type-extra word,
//  the resolved type itself is copied from `a4`, and `m_28` is a fixed
//  sentinel (-1, "no default value index" most likely).
struct NodeParam : Node
{
    void* m_20;
    Node* m_24;
    int   m_28;
    //  0x6F463520
    NodeParam(JassCompileContext* parent, Node* a4, void* a8)
        : Node(parent, 0xC), m_20((void*)(long)*(int*)((char*)a8 + 0x14)),
          m_24(a4), m_28(-1)
    {
        *(void**)((char*)a8 + 0x18) = this;
        m_typeKind = a4->m_typeKind;
        m_typeExtra = a4->m_typeExtra;
    }

    //  0x6F463580 / 0x6F463600 (DeleteSelf).  Releases only `m_24`.
    ~NodeParam();
    virtual Node* DeleteSelf(int flags);
};

//  NodeAddr (0x6F463980) - kind 0x26 ('&').  Fixed type-kind 3 (an
//  address/pointer type tag); the payload word is the operand's own
//  type-extra, not the operand itself.
struct NodeAddr : Node
{
    int m_20;
    //  0x6F463980
    NodeAddr(JassCompileContext* parent, Node* operand)
        : Node(parent, 0x26), m_20(operand->m_typeExtra)
    {
        m_typeKind = 3;
    }
};

#endif
