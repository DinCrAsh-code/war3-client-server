//============================================================================
//  jassvm-batch-4 - pass-through definitions for the assigned addresses
//  this session did not reconstruct, so the tree still links.  NOT
//  reconstructions - each body jumps to the shipped function at its real
//  address; the real body still runs, unhooked.  Never offer any of these
//  as hookable.  See jassnodectors.cpp for the AST node constructors this
//  session did reconstruct, and the write-up for why each of these was
//  left for a follow-up session (mostly: an unfamiliar class this batch's
//  dumps show only a fragment of, a `Jass2/Nodes.h`-tagged jump-table
//  dispatcher whose exact case order needs more care than this session's
//  budget allowed, or - for the flagship 2274-instruction AST evaluator
//  and the 827-instruction lexer scan loop - real size).
//
//  Every declaration below states the exact calling convention and stack
//  byte count `build/*.asm` showed for its target, which is what
//  tools/thunk_abi_audit.py checks.
//============================================================================

#define JASS_THUNK(addr)         \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//----------------------------------------------------------------------------
//  Jass2/Nodes.h family - siblings of jassnodectors.cpp's constructors.
//----------------------------------------------------------------------------

//  0x6F455E50 - `retn 0`, `__thiscall`.  Wraps `this->+0x24` in a
//  NodeCast (kind 0x27) allocated from g_jassNodePool when a coercion is
//  needed; sets `this->+0x10`/+0x14 to a fixed (5,0) type pair either way.
//  Small but its own control flow needs the same close reading
//  jassnodectors.cpp's leaves got; left for a follow-up session.
__declspec(naked) void Node_CoerceCastOperand_6F455E50()      JASS_THUNK(0x6F455E50)

//  0x6F455ED0 - `retn 0`, `__thiscall`.  An 11-case jump table
//  (`jpt_6F455F13`) picking an operator/coercion result from two child
//  nodes' resolved types; calls sub_6F460760 (also unreconstructed this
//  session) and sub_6F4C1AB0/sub_6F455D00 (already reconstructed) along
//  one path.  jpt_6F455F13 is this function's own jump table, not a
//  separate address - `tools/ida_query.py func_at` confirms neither
//  jpt_6F455F13 nor jpt_6F46467F (0x6F464460's own table) is a function.
__declspec(naked) void Node_InferBinaryResultType_6F455ED0()  JASS_THUNK(0x6F455ED0)

//  A plain free-function declaration defaults to `__cdecl` in this
//  project (no /Gr/Gz/Gv on the command line) - caller-cleans, so its own
//  mangled name would demand a shipped `retn 0`.  Every thunk below that
//  takes a stack argument is `__thiscall` in the shipped image instead
//  (callee-cleans), so - same as jassnatives_thunks.cpp's own pattern -
//  it is declared as a member of a placeholder struct instead of a free
//  function, which is the only way this compiler spells `__thiscall`
//  explicitly.  A first pass here declared several of these as free
//  functions with stack arguments; `thunk_abi_audit.py` caught the whole
//  class of them as `__cdecl`/`retn 0` vs. the shipped `retn <n>` MISMATCH
//  before this was committed live.

//  0x6F462480 - `retn 4`, `__thiscall`.  A second, similar type-coercion
//  helper operating on `this->+0x20`/`+0x24` (insert a NodeCast on
//  whichever operand's type is 4-but-should-be-5), reusing
//  sub_6F4C1AB0/sub_6F455D00 the same way 0x6F455ED0 does.
struct Node_CoerceOperandTypes_Owner { void CoerceOperandTypes(void*); };
__declspec(naked) void Node_CoerceOperandTypes_Owner::CoerceOperandTypes(void*) JASS_THUNK(0x6F462480)

//  0x6F460760 - `retn 4`, `__thiscall`.  Walks a linked structure off
//  `this` (fields at +0x18/+0x14, not Node's own layout - the caller is
//  not a `Node*` here) testing membership of `[esp+arg_0]->+4`.  The
//  class this really belongs to has no name recovered yet.
struct Node_ContainsMember_Owner { int ContainsMember(void*); };
__declspec(naked) int Node_ContainsMember_Owner::ContainsMember(void*) JASS_THUNK(0x6F460760)

//  0x6F461260 - `retn 4`, `__thiscall`.  Propagates a child's own
//  +0x18/+0x10/+0x14 onto `this->+0x18`/+0x10/+0x14 and stamps
//  `this->+0x2C` - offsets past Node's own 0x1C footprint, so `this` here
//  is some derived node this batch's dumps do not show the rest of.
struct Node_PropagateChildType_Owner { void PropagateChildType(void*); };
__declspec(naked) void Node_PropagateChildType_Owner::PropagateChildType(void*) JASS_THUNK(0x6F461260)

//  0x6F464400 - `retn 0`, `__thiscall`, no stack args - a free function's
//  __cdecl default already matches this one.
__declspec(naked) void Node_DestroySubobjectAt2C_6F464400()   JASS_THUNK(0x6F464400)

//  0x6F4617C0 - `retn 0x10`, `__thiscall`, 4 stack args.  NodeIf::NodeIf -
//  294 instructions of real then/else branch type-unification logic
//  (mirrors 0x6F455ED0/0x6F462480's coercion shape at greater length).
//  Left for a follow-up session given this session's remaining budget.
struct NodeIf_Owner { void ctor(void*, void*, void*, void*); };
__declspec(naked) void NodeIf_Owner::ctor(void*, void*, void*, void*) JASS_THUNK(0x6F4617C0)

//  0x6F4614D0 - `retn 0xC`, `__thiscall`, 3 stack args.  NodeCode::NodeCode
//  (kind 4) - 167 instructions of branch-type-unification logic between
//  two statement lists, in the same family as NodeIf's.
struct NodeCode_Owner { void ctor(void*, void*, void*); };
__declspec(naked) void NodeCode_Owner::ctor(void*, void*, void*) JASS_THUNK(0x6F4614D0)

//  0x6F463630 - `retn 0x10`, `__thiscall`, 4 stack args.  NodeCall::NodeCall
//  (kind 0xE) - SEH-guarded (a `__except_handler4` frame), and calls
//  sub_6F460750/sub_6F460670 (neither in this batch).
struct NodeCall_Owner { void ctor(void*, void*, void*, void*); };
__declspec(naked) void NodeCall_Owner::ctor(void*, void*, void*, void*) JASS_THUNK(0x6F463630)

//----------------------------------------------------------------------------
//  The flagship AST builder and its own lexer/scanner sibling.
//----------------------------------------------------------------------------

//  0x6F464460 - `retn 4`, `__thiscall`, 1 stack arg.  THE FLAGSHIP: 2274
//  instructions, tagged `Jass2/Nodes.h` throughout, an SEH-guarded
//  jump-table dispatch over an opcode/token stream (NOT the node `m_kind`
//  tags - see below).  batch-4's own read described this as "the AST
//  evaluator's own execution core"; jassvm-flagship-I re-read ~60 of its
//  ~116 distinct main-jump-table (0x6F46467F) opcode handlers
//  case-by-case (docs/targets/jassvm-flagship-opcode-table.md has the full
//  table) and found that framing backwards - nearly every handler that
//  calls anything besides the recursive sub_6F45FF30 ends in
//  `CDataAllocator::Alloc` + a `NodeXxx::NodeXxx` constructor this repo
//  has already reconstructed (NodeSet, NodeAnd, NodeCompare, NodeMath,
//  NodeVar, NodeCall, ... - see the table doc for the full list).  That is
//  the signature of a function that BUILDS the AST from a linear
//  opcode/token stream (`this`'s own +0x0C/+0x14 fields are a read cursor
//  advancing 2 bytes at a time through a 16-bit-opcode stream, +0x18 a
//  write cursor advancing 4 bytes at a time through what looks like a
//  node-pointer stack), not a function that walks an already-built tree
//  and evaluates it - consistent with `sub_6F45FF30` below being the
//  character-level lexer this function's own recursive calls almost
//  certainly drive.  Renamed from `JassAstEvaluator_Owner::Evaluate`
//  accordingly; nothing about the verdict or scope changed (still
//  `THUNK`, still SEH-capped below EXACT/IDENTICAL regardless of how much
//  of the body is eventually transcribed - see the opcode-table doc's own
//  section on why).  `jpt_6F46467F` is this function's own jump table
//  (confirmed via `ida_query.py func_at`, back when that tool existed),
//  not a separate address.
struct JassAstBuilder_Owner { void Build(void*); };
__declspec(naked) void JassAstBuilder_Owner::Build(void*) JASS_THUNK(0x6F464460)

//  0x6F45FF30 - `retn 0`, `__thiscall`.  827 instructions: a
//  character-by-character lexer/scanner main loop (UTF/codepage
//  classification tables at 0x6FA8xxxx, a virtual `getchar()`-shaped call
//  through `this`'s own vtable slots 0 and 1, keyword/identifier state
//  tables) - almost certainly `JassLexer::NextToken` or its equivalent.
//  Left for a follow-up session for the same reason as the flagship.
__declspec(naked) void JassLexer_NextToken_6F45FF30()         JASS_THUNK(0x6F45FF30)

//----------------------------------------------------------------------------
//  A small buffer/array class in the 0x6F455xxx-0x457xxx neighbourhood
//  (SMemAlloc-tagged "e:\drive1\temp\buildwar3x\engine\s..." - a
//  truncated shipped path, not `.\Jass.cpp`, so this is very likely a
//  different original module reused by the JASS front end rather than
//  Jass.cpp/JassWar3.cpp themselves) - no class name recovered.
//----------------------------------------------------------------------------
struct JassBuffer_Owner
{
    void* Clone(void*);
    void  Append(void*);
    void  Destroy();
    void  ctor(void*);
    void  Reset(void*);
};
__declspec(naked) void* JassBuffer_Owner::Clone(void*)  JASS_THUNK(0x6F4560D0)
__declspec(naked) void  JassBuffer_Owner::Append(void*) JASS_THUNK(0x6F4561C0)
__declspec(naked) void  JassBuffer_Owner::Destroy()     JASS_THUNK(0x6F456650)
__declspec(naked) void  JassBuffer_Owner::ctor(void*)   JASS_THUNK(0x6F4574B0)
__declspec(naked) void  JassBuffer_Owner::Reset(void*)  JASS_THUNK(0x6F456090)
//  0x6F455760 - `retn 0`, `__fastcall(scriptIndex in ecx)`.  Signature
//  corrected this round (jass-instance-targeting-D) from the parameterless
//  declaration this thunk carried since jassvm-batch-4: it is the root-
//  JassInstance constructor a compiled script's own slot index drives -
//  `JassThreadGetScript(scriptIndex)` resolves the JassThreadLocal::m_scripts
//  record `LoadAndCompileCommonScripts`'s own compile pass just populated,
//  `JassInstance::Construct(that record, 1)` allocates+builds a *brand-new*
//  root instance from it (flag `1`, as opposed to the `0` every
//  SpawnChildInstance/CreateChildInstance call site in this closure passes -
//  see jassinstance.h's own note on Construct's `flag`, and
//  docs/notes/jass-runtime-instance-targeting.md for why that flag is what
//  gates JassInstance::Construct's own call into ExecuteDeclStream), then
//  auto-assigns it a fresh slot in the calling thread's own JassThreadLocal
//  instance-handle array (JassThreadRegisterInstance/AssignInstanceSlot,
//  already reconstructed) and **returns that assigned slot index in eax**
//  (not the instance pointer - the epilogue never reloads eax after the
//  AssignInstanceSlot call that sets it, so whatever `mov [esi+2850h], eax`
//  just consumed is exactly what `retn` hands back to the caller too).
//  jassgetorrunscriptinstance.cpp's own GetOrRunJassInstanceForScript is
//  this function's one caller found so far.
__declspec(naked) unsigned int __fastcall Jass_ctor_SEH_6F455760(unsigned int scriptIndex) JASS_THUNK(0x6F455760)
__declspec(naked) void  Jass_dtor_SEH_6F457530()               JASS_THUNK(0x6F457530)

//  0x6F45A0B0 - `retn 0`, `__thiscall`.  A JassArray::JassArray variant:
//  stamps the vtable, then frees whatever was already at `this+0xC`
//  *before* zeroing it - odd for a plain constructor (freeing
//  uninitialised/garbage memory), which reads as this ctor sharing a
//  compiled body with a "Reset()"-shaped member rather than being a pure
//  constructor.  Left thunked rather than risk a live double-free on a
//  guess at which case this really is.
__declspec(naked) void JassArray_ctor_6F45A0B0()               JASS_THUNK(0x6F45A0B0)
