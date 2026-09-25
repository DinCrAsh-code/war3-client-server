//============================================================================
//  CTaskParam's four serialisation-family vtable overrides -
//  GetRecordVersion, SaveFlags, LoadFlags, DumpState - all clustered at
//  0x6F287Cxx-0x6F287Dxx, one module, one TU.  Save/Load (slots 14/15) are
//  *not* here: they are byte-identical to CTask's own (see taskparam.h's
//  own header comment), so this file only has the four CTaskParam genuinely
//  overrides.
//
//  GetRecordVersion is a real point of divergence from the naive "each
//  class calls its own immediate base and adds one" shape: the shipped
//  body calls straight into sub_6F471C10 (CAgent::GetRecordVersion) again -
//  the exact same callee CTask::GetRecordVersion itself calls, not a chain
//  through CTask::GetRecordVersion - and adds 4 instead of CTask's own 3.
//============================================================================
#include "taskparam.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Agent/handletriple.h"
#include "../Storm/textsink.h"

//----------------------------------------------------------------------------
//  0x6F287CD0 - slot 10, GetRecordVersion.  `retn 4`.  A direct call to
//  CAgent's own answer (sub_6F471C10), the same callee CTask::GetRecordVersion
//  calls, not CTask::GetRecordVersion itself - confirmed by the disassembly
//  making a real `call sub_6F471C10`, not a virtual dispatch through
//  CTask's own slot 10.
//----------------------------------------------------------------------------
int CTaskParam::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 4;
}

//----------------------------------------------------------------------------
//  0x6F287D30 - slot 17, DumpState.  `retn 4`.  Four fields, four
//  self-describing dump calls: m_flags (CAgentWar3's own, +0x20) as a plain
//  dword, the m_field24/m_field28 pair (CUnitRefList's own, +0x24/+0x28) as
//  a "ref" pair, m_field30 (CTask's own, +0x30) as a plain dword, and
//  m_field34 (this class's own, +0x34) as a plain dword again.  Note +0x2C
//  is *not* dumped here, same as CTask's own DumpState.
//----------------------------------------------------------------------------
void CTaskParam::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field34)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F287CE0 - slot 18, SaveFlags.  `retn 4`.  All six fields from +0x20
//  to +0x34, in offset order; +0x2C is the one write that goes through
//  CDataStoreScratch's *other* four-byte writer (WriteDwordAlt2, 0x6F4C2310)
//  rather than WriteDword (0x6F4C2360) - same "not interchangeable, this
//  image was linked without /OPT:ICF" note as CTask's own SaveFlags
//  (cdatastorescratch.h).
//----------------------------------------------------------------------------
void CTaskParam::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);
    store->WriteDword(m_field24);
    store->WriteDword(m_field28);
    store->WriteDwordAlt2(m_field2C);
    store->WriteDword(m_field30);
    store->WriteDword(m_field34);
}

//----------------------------------------------------------------------------
//  0x6F287D70 - slot 19, LoadFlags.  `retn 4`.  Mirrors SaveFlags: m_flags
//  read as a plain dword, then m_field24/m_field28/m_field2C read in one
//  shot as an SHandleTriple (ReadHandleTriple, handletriple.h - same
//  reasoning as CTask's own LoadFlags), then m_field30 and m_field34 each
//  read as their own plain dword.
//----------------------------------------------------------------------------
void CTaskParam::LoadFlags(CDataStore* store)
{
    unsigned int value;

    store->ReadDword(&value);
    m_flags = value;

    ReadHandleTriple(store, reinterpret_cast<SHandleTriple*>(&m_field24));

    store->ReadDword(&value);
    m_field30 = value;

    store->ReadDword(&value);
    m_field34 = value;
}
