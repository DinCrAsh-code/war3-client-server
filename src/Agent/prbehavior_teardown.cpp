//============================================================================
//  0x6F4AA550 - NIpse::CPrBehavior::Teardown, vtable slot 4. See
//  prbehavior.h's own comment: a bare tail sequence with no register saved
//  across it, unlike CAgentRelation::Teardown's own otherwise-identical
//  shape (Agent/agentrelationteardown.cpp) which does need to cache `this`
//  in esi across its own extra work. Release both pending sync
//  registrations, then tail-jump into
//  SHandleTableEntry::UnregisterAndNotify(arg).
//
//  `this` is reinterpreted as CAgentTickRelay* purely for
//  ReleasePendingA/B's own generic "retire the registration and clear the
//  reference" shape against the +0x1C/+0x20 pair (Agent/agenttickrelay.h) -
//  this call tree never establishes CPrBehavior as inheriting from
//  CAgentTickRelay, only that the two classes happen to share the same
//  field layout at that offset, the same "reinterpret an existing object as
//  a raw interface" idiom Agent/agentbaseabseventbinding.cpp's own header
//  already names. SHandleTableEntry is declared locally rather than pulled
//  in via handletable.h, same reason as prbehavior_attachhandle.cpp.
//============================================================================
#include "prbehavior.h"
#include "agenttickrelay.h"

struct SHandleTableEntry
{
    void UnregisterAndNotify(int arg);
};

namespace NIpse {

//  This compiler spills `this` to esi and reloads it into ecx before each
//  of the three calls (push esi/mov esi,ecx up front, mov ecx,esi before
//  ReleasePendingB and again before UnregisterAndNotify) where the shipped
//  body keeps `this` in ecx across all three with no spill and no reload
//  at all, ending in a genuine tail `jmp` into UnregisterAndNotify instead
//  of a `call`+`pop`+`retn`. A single local interface type spanning all
//  three calls (rather than this file's own CAgentTickRelay/
//  SHandleTableEntry split) was tried to see whether crossing between two
//  reinterpreted types was forcing the reload - it was not: the same
//  extra `mov ecx, esi` still appeared, and a single fictitious type also
//  cannot link to the three real, already-named symbols these calls have
//  to resolve to (CAgentTickRelay::ReleasePendingA/B,
//  SHandleTableEntry::UnregisterAndNotify - each tied by mangled name to
//  its own real class). Cross-referenced against
//  Agent/agentrelationteardown.cpp's own CAgentRelation::Teardown, which
//  has the *identical* three-call shape and genuinely DOES spill to esi in
//  its own shipped body (0x6F4A6920) - so this compiler's spill-happy
//  behaviour here is not a mistake, it is what MSVC does for this shape
//  without whole-program optimization; only 0x6F4AA550 itself is the one
//  address in this family where the real, LTCG-built image proved `this`
//  survives all three calls unspilled and folded the last one into a tail
//  jmp, which is exactly the kind of cross-function register-preservation
//  fact this toolchain has no way to know without seeing every callee's
//  own body (see Pathfinding/agenttickrelay.h's own CAgentTickRelay::Arm
//  header comment for the same "two things unreachable from source, both
//  the shipped image's own whole-program register conventions" shape).
void CPrBehavior::Teardown(int arg)
{
    ((CAgentTickRelay*)this)->ReleasePendingA();
    ((CAgentTickRelay*)this)->ReleasePendingB();
    ((SHandleTableEntry*)this)->UnregisterAndNotify(arg);
}

}  // namespace NIpse
