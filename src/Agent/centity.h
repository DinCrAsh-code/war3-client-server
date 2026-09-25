//============================================================================
//  NTempest::CEntity (`??_7CEntity@NTempest@@6B@`, 0x6F951784) - the root of
//  the single-inheritance chain crlagent.h documents in full:
//
//      CAgentBaseAbs : public NIpse::CRlAgent : public NIpse::CPrRelation :
//        public NTempest::CPresenceTagged : public NTempest::CPresence :
//        public NTempest::CMemBlock : public NTempest::CEntity
//
//  One vtable slot only (index 0, confirmed the same way crlagent.h's own
//  slot-0 note is - a direct read of the vtable's first dword in
//  agent_worktrees/classes/0x6F951784.json; no further slot has its own
//  symbol to confirm past that).  A scalar deleting destructor with nothing
//  to tear down: CEntity has no base of its own, so the body is nothing but
//  the standard MSVC "restamp this class's own vtable, then free when bit 0
//  of `flags` is set" shape - no `this != 0` guard on the free either,
//  unlike the sibling shapes further up the chain that call a real base
//  destructor first (Agent/cpresence_dtor.cpp's `CPresenceRealDtor`-backed
//  version, Agent/cmemblock.h's own, CAgentBaseAbs's own in
//  Agent/cagentbaseabs_module_batch2.cpp) - see centity_dtor.cpp for why
//  that distinction is read straight off the two shapes' raw bytes, not
//  guessed.
//
//  No real C++ layout is modelled here beyond what this one function
//  touches (nothing besides `this` itself) - see crlagent.h's own file
//  header for why this whole chain stays a set of flat, address-only
//  classes rather than a real inheritance tree.
//============================================================================
#ifndef CENTITY_H
#define CENTITY_H

namespace NTempest {

class CEntity
{
public:
    //  0x6F467810 - vtable slot 0.
    void* ScalarDeletingDestructor(unsigned int flags);
};

}  // namespace NTempest

#endif  // CENTITY_H
