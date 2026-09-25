//============================================================================
//  NTempest::CSyncData (`??_7CSyncData@NTempest@@6B@`, 0x6F9581A4, 5 slots) -
//  its ONLY base is the already-fully-reconstructed, 1-slot
//  NTempest::CEntity (Agent/centity.h). Much smaller and shallower than the
//  rest of this session's own NIpse::CPmRegion work (pmregion.h) - every
//  slot here is genuinely this class's own, nothing shared/inherited.
//
//  No constructor found anywhere in this session's own search of
//  agent_worktrees/funcs/'s bulk dump for cross-references to 0x6F9581A4 -
//  matches the established pattern for this whole family (Agent/presence.h,
//  Agent/pmregion.h, etc all note the same absence). Not invented.
//============================================================================
#ifndef CSYNCDATA_H
#define CSYNCDATA_H

namespace NTempest {

class CSyncData
{
public:
    //  0x6F53D690 - vtable slot 0, scalar deleting destructor (NOT slot 2,
    //  unlike the rest of this base chain - this class's own vtable is
    //  short enough that the destructor sits first). Restamps straight to
    //  the base NTempest::CEntity's own vftable (Agent/centity.h,
    //  `??_7CEntity@NTempest@@6B@`) with no real ~CSyncData() call - same
    //  no-teardown-restamp shape as CEntity's own trivial destructor
    //  (centity.h), consistent with CEntity having nothing of its own to
    //  tear down and this class introducing nothing that needs it either.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F536BE0 - vtable slot 1. Plain zero-argument accessor: returns
    //  m_field4 as-is. Called virtually (through `this`'s own vtable, not
    //  directly) from this class's own DumpDebugState below with the
    //  literal name "csum" - most likely overridden with a real checksum
    //  computation by some other class sharing this same vtable slot
    //  further down an inheritance chain this session's own scope does not
    //  reach; this class's own base implementation just returns the raw
    //  field.
    unsigned int GetChecksumField();

    //  0x6F536C10 - vtable slot 2. Zeroes m_field4 and m_field8. `eax` is
    //  used only as the zero source register (`xor eax,eax`) - no
    //  meaningful return value.
    void ClearFields();

    //  0x6F536C20 - vtable slot 3 (nullsub_1539). A genuinely distinct
    //  one-instruction no-op specific to THIS class - NOT a member of the
    //  shared SVtableNoOp/SVtableNoOpArg family Agent/
    //  cagentbaseabs_noop_slots.cpp documents (that family is all
    //  NTempest::CPresence-chain addresses; this one shares none of them).
    //  Zero stack arguments, matching the raw `retn` with no operand.
    void Slot3();

    //  0x6F536C30 - vtable slot 4, DumpDebugState. See
    //  csyncdata_dumpdebugstate.cpp.  `retn 8`: the second stack dword
    //  (never read) is genuine unread padding in this function's own
    //  declared signature, the same shape CDataStoreScratch::
    //  WriteRecordHeader's own header comment documents for an unrelated,
    //  always-pushed-but-unread trailing parameter.
    void DumpDebugState(void* sink, unsigned int unused4);

    void*        m_vfptr;    // +0x00 - own vfptr (CEntity's base slot 0 is
                              //   this class's own inherited-but-overridden
                              //   scalar deleting destructor).
    unsigned int m_field4;    // +0x04 - the "csum" GetChecksumField/
                              //   ClearFields both touch.
    unsigned int m_count;     // +0x08 - "count[%u]" in DumpDebugState; also
                              //   zeroed by ClearFields.
    const char*  m_name;      // +0x0C - "(%s)" in "[CSyncData] (%s)".
};

}  // namespace NTempest

#endif  // CSYNCDATA_H
