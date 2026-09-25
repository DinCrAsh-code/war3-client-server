//============================================================================
//  CAgentWar3: a trivial intermediate base between CAgent and CWidget the
//  CItem__Constructor dump (0x6F2B7520) exposes that CWidget__Constructor
//  (0x6F2ACAE0, docs/targets/CWidget__Constructor.md) never did.
//
//  CAgentWar3 adds no fields and no observable constructor body of its
//  own: sub_6F266C30 (CSelectable's own base-construction helper, see
//  selectable.h) calls CAgent::CAgent() exactly once and then, with no
//  second call in between, stamps CWidget's vtable directly - the same
//  "single-call-site trivial constructor gets fully inlined" shape
//  CWidget__Constructor's own root already relies on for CWidget's own
//  constructor (widget.h).  CWidget__Constructor's dump could not show
//  this layer existed at all: nothing in that call tree could ever throw
//  between CAgent::CAgent() returning and CWidget's own vtable store, so
//  no exception-unwind funclet ever needed CAgentWar3's own address there,
//  and /O2 inlined its trivial default constructor away completely.
//
//  CItem__Constructor's own root *can* throw after this whole chain
//  finishes (two more base-class constructions, CAgentTimer at +0xD0 and
//  +0xE4, follow it) - which is why its unwind table needs a real,
//  out-of-line address to destruct "a fully-built CSelectable" with, and
//  the compiler picked CAgentWar3's own vtable-bearing level to synthesize
//  it at (agentwar3_dtor.cpp): CWidget's and CSelectable's own trivial
//  destructors have nothing to do beyond resetting a vtable pointer that
//  nothing then reads before the whole object goes away, so their vtable
//  resets are dead stores /O2 elides, leaving one real reset (to
//  CAgentWar3's own vtable) followed by a tail call into CAgent::~CAgent().
//
//  Inserting this class does not move any field CWidget or CSelectable
//  already reads/writes - CAgentWar3 is layout-neutral by construction
//  (zero-size base optimisation does not apply here since it is not
//  actually empty of virtuals, only of data, but it still contributes no
//  bytes of its own) - so it costs CWidget__Constructor's already-matched
//  instructions nothing; verify.py confirms the report is unchanged after
//  this file lands.
//============================================================================
#ifndef AGENTWAR3_H
#define AGENTWAR3_H

#include "agent.h"

//  Only ever pointed to from here - the two serialisation slots below take
//  one each and hand it straight on.  Including cdatastore.h/
//  cdatastorescratch.h instead would drag the whole streaming layer into
//  every widget/item translation unit for two parameter types.
class CDataStore;
class CDataStoreScratch;

class CAgentWar3 : public CAgent
{
public:
    //====================================================================
    //  CAgentWar3's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has
    //  to stay that way - inserting one line in the wrong place renames
    //  every slot after it.  Slots 1-29; see
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md.
    //====================================================================
    //  slot 1 / +0x04 (0x6F001B30, sub_6F001B30)
    virtual CAgentWar3* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F0015E0, sub_6F0015E0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 9 / +0x24 (0x6F001AB0, sub_6F001AB0)
    virtual int Method_0x24(int context);
    //  shared "always 0" leaf and return its result plus one, i.e. always
    //  1.  The +1-over-a-base-class-answer shape is what a "record format
    //  version" accessor looks like when the base contributes 0 and each
    //  derived layer adds its own one, so it is named for that; nothing
    //  else in these three vtables reaches it.
    //  slot 10 / +0x28 (0x6F001AE0)
    virtual int GetRecordVersion(int context);
    //  slot 14 / +0x38 (0x6F001A30)
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F001A70)
    virtual void Load(class CDataStore* store);
    //  slot 17 / +0x44 (0x6F001B20, sub_6F001B20)
    virtual void DumpState(void* sink);
    //  write m_flags to a CDataStoreScratch and read it back out of a
    //  CDataStore.  The pair is what pins m_flags down as one whole
    //  serialisable dword rather than a set of separate bytes.
    //  slot 18 / +0x48 (0x6F001AF0)
    virtual void SaveFlags(CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F001B00)
    virtual void LoadFlags(CDataStore* store);
    //  slot 22 / +0x58 (0x6F001590, sub_6F001590)
    virtual const char* GetClassName();
    //  slot 24 / +0x60 (0x6F001BE0)
    virtual void SetFlags(unsigned int flags);
    //  slot 25 / +0x64 (0x6F001A20)
    virtual unsigned int Method_0x64();
    //  fifth by the second when the second is non-zero, free the third
    //  through Storm if it is set, and return the advanced fifth.  The
    //  ".?AVCAgentFieldMap@@" tag on the free is the only clue to what any
    //  of them are, and it names a class no dump in this repo reaches.
    //  slot 26 / +0x68 (0x6F001DE0)
    virtual void* Method_0x68(int, int advance, void* block, char* cursor, int);
    //  *second* plus one and ignores the other two.
    //  slot 27 / +0x6C (0x6F001AC0)
    virtual int Method_0x6C(int, int count, int);
    //  third through Storm with the same ".?AVCAgentFieldMap@@" tag if it
    //  is set, and returns nothing.
    //  slot 28 / +0x70 (0x6F001E10)
    virtual void Method_0x70(int, int, void* block, int);
    //  slot 29 / +0x74 (0x6F001AD0, nullsub_52).  Empty with `retn 8` -
    //  two stack dwords, not none.  See CAgent::Method_0x2C (agent.h) for
    //  why an empty slot's argument count still has to be right.
    virtual void Method_0x74(int a, int b);

    //  Fully inlined at every call site seen so far (see the header
    //  comment above) - never itself reached as a real out-of-line
    //  address, only its destructor is (agentwar3_dtor.cpp).  Declared
    //  anyway so CWidget's own real constructor has a base to call.
    //
    //  It is not empty any more: m_flags below is a base-class field, so
    //  zeroing it is this constructor's own job, and that is exactly where
    //  both shipped constructors put the store - `mov [esi+20h], 0`
    //  immediately after `call sub_6F001730` (CAgent::CAgent) and before
    //  the derived class's own vtable store, in CWidget::CWidget
    //  (0x6F2ACAE0) and CSelectable::CSelectable (0x6F266C30) alike.
    CAgentWar3() : m_flags(0) {}

    //  0x6F266D00 - see agentwar3_dtor.cpp.  Reached only from an
    //  exception-unwind funclet in this call tree (CItem__Constructor,
    //  0x6F2B7520), never from a live `delete` - same footing as
    //  CAgent::~CAgent() above.  Not virtual for the same reason.
    ~CAgentWar3();

    //====================================================================
    //  The vtable slots CWidget, CSelectable and CItem all three inherit
    //  unchanged from here - user_knowledge.json's three vtable dumps name
    //  the same address at the same index in all three, and every one of
    //  them touches m_flags below or nothing at all.  See
    //  agentwar3_flags.cpp / agentwar3_slots.cpp for the bodies and
    //  docs/targets/CWidget__vtable_full.md for the survey.
    //
    //  None of them is spelled `virtual`, for this repo's usual reason
    //  (widget.h): the class model here only ever declares the eight
    //  virtuals CAgent contributes, and adding more would renumber them.
    //  They are ordinary members that happen to sit in a vtable slot.
    //====================================================================

















    //====================================================================
    //  +0x00-+0x1F: CAgent (base class above).
    //====================================================================

    //  +0x20.  Not CWidget's and not CSelectable's, though both used to
    //  declare their own copy of it at this offset (widget.h's m_flags,
    //  selectable.h's m_reserved20) - the two are the *same* field, and
    //  this is where it lives.  The proof is the vtable slots above:
    //  0x6F001AF0/0x6F001B00/0x6F001BE0/0x6F0520B0/0x6F267120/0x6F267140/
    //  0x6F077E20 all read or write `[ecx+20h]` and all sit at the same
    //  index in CWidget's, CSelectable's *and* CItem's vtables, so the
    //  field they share has to belong to a base all three have - and
    //  CAgent ends at exactly +0x20 (agent.h), which leaves this class.
    //
    //  Moving it here is layout-neutral: CWidget's and CSelectable's own
    //  fields keep every absolute offset they had, and both constructors
    //  keep the store in the same place (see the constructor above).
    //
    //  Known bits: 0x01 "pending" (SetPending above, cleared by
    //  CSelectable::ClearPendingAndNotify), 0x02/0x04/0x08 (SetFlagBit1-3
    //  above), 0x10 and 0x20 (CWidget::InitSprite sets 0x10 from the art
    //  descriptor's +0x54; CWidget::IsVisibilityMaskLive tests 0x30),
    //  0x80000000 (CSyncChannel picks the local or remote channel on it).
    unsigned int m_flags;    // +0x20
};

#endif
