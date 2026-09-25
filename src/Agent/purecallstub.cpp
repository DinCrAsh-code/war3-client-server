//============================================================================
//  0x6F7E10C2 - `_purecall`: `jmp ds:__imp__purecall` (agent_worktrees'
//  own `ida_name` for it, and `tools/worktree_store.py resolve
//  __imp__purecall` confirms 0x6F86D3F0 is a genuine four-byte IAT slot).
//  This is not a per-class body: it is the one, shared trampoline the whole
//  binary's pure-virtual dispatch uses - the same address a "= 0" virtual
//  with no override compiles down to project-wide, MSVC's own generated
//  glue for an abstract slot no derived class has reached yet.
//
//  Reconstructed here because NIpse::CPrBehavior's own vtable slot 17
//  (+0x44, agent_worktrees/classes/0x6F951D44.json) names this address
//  directly - CPrBehavior is abstract at that slot, and
//  Agent/prbehavior_dispatch.cpp's own DispatchIfEnabled dispatches through
//  it on every live object, so a real behaviour only exists once a
//  concrete derived class (this cluster's own task notes name NIpse::
//  CBhPoBehavior as the likely candidate) overrides it. Net/netconn.h's own
//  precedent for a `_purecall` vtable slot gives it a trivial `{}` body
//  instead of trying to match this exact jump, on the reasoning that
//  nothing in this repo's own code ever executes one through our own
//  vtable at run time (a hook patches a function, never a vtable). This
//  address is different: it is itself one of this session's ten assigned
//  TODO targets, so it gets a real, byte-matching body rather than being
//  left unreconstructed.
//
//  IMPORTANT CORRECTION (link_check.py caught this - see its own "UNEXPECTED
//  unresolved externals" bucket): the shipped image imports `_purecall`
//  dynamically (`jmp ds:__imp__purecall`, an IAT slot at 0x6F86D3F0)
//  because the real Game.dll links against the CRT as a DLL. THIS
//  toolchain's own link line (CLAUDE.md's own link_check.py section)
//  statically links `libcmt.lib` instead, exactly like both injection
//  repos' real `build_mix.py` does - so there is no `__imp__purecall`
//  import slot to reference at all in a build shaped this way; `_purecall`
//  itself is a plain, statically-linked libcmt symbol here, not an import.
//  Declaring the raw `__imp__purecall` pointer (this file's own first
//  attempt) compiles clean - nothing catches an invented `extern` at
//  compile time - but leaves a live unresolved external in the real link,
//  invisible to `verify.py` and only caught by `link_check.py`'s actual
//  link step (its own header comment: "compiles clean, scores clean, and
//  fails only in the operator's build_mix.py").
//
//  So this reconstruction is a genuine, unavoidable direct-vs-indirect
//  addressing difference between the shipped (dynamically-linked CRT) and
//  this pipeline's (statically-linked libcmt) build shapes - `call/jmp
//  _purecall` direct here where the shipped image has to bounce through
//  its own IAT, DIFFERS rather than EXACT even though the C++ says exactly
//  what the shipped intent was. `_purecall` itself is declared to match
//  the compiler's own internal auto-declaration for a "= 0" virtual's glue
//  (no `dllimport` - that attribute is what produced this file's own
//  earlier `error C2375: redefinition; different linkage`, confirming the
//  internal one already carries plain, non-imported `__cdecl` linkage).
//============================================================================
extern "C" int __cdecl _purecall(void);

extern "C" __declspec(naked) void PureVirtualCallStub()
{
    __asm { jmp _purecall }
}
