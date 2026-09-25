//============================================================================
//  CTask's four serialisation-family vtable overrides - GetRecordVersion,
//  SaveFlags, LoadFlags, DumpState - all clustered at 0x6F287Bxx-0x6F287Cxx,
//  one module, one TU.  Save/Load (slots 14/15) are *not* here: they are
//  byte-identical to CUnitRefList's own (see task.h's own header comment),
//  so this file only has the four CTask genuinely overrides.
//============================================================================
#include "task.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Agent/handletriple.h"
#include "../Storm/textsink.h"

//----------------------------------------------------------------------------
//  0x6F287BD0 - slot 10, GetRecordVersion.  `retn 4`.  CAgent's own answer
//  (a plain base-class call, not a virtual dispatch - the shipped body
//  calls straight into sub_6F471C10, CAgent::GetRecordVersion, not through
//  the vtable) plus 3.
//----------------------------------------------------------------------------
int CTask::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 3;
}

//----------------------------------------------------------------------------
//  0x6F287C30 - slot 17, DumpState.  `retn 4`.  Three fields, three
//  self-describing dump calls: m_flags (CAgentWar3's own, +0x20) as a plain
//  dword, the m_field24/m_field28 pair (CUnitRefList's own, +0x24/+0x28) as
//  a "ref" pair, and m_field30 (+0x30, this class's own) as a plain dword
//  again.  Note +0x2C is *not* dumped here, unlike Save/LoadFlags below.
//----------------------------------------------------------------------------
void CTask::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F287BE0 - slot 18, SaveFlags.  `retn 4`.  All five fields from
//  +0x20 to +0x30, in offset order; +0x2C is the one write that goes
//  through CDataStoreScratch's *other* four-byte writer
//  (WriteDwordAlt2, 0x6F4C2310) rather than WriteDword (0x6F4C2360) -
//  this image was linked without /OPT:ICF, so the two identical-bodied
//  writers stay distinct addresses and the shipped call sites are not
//  interchangeable (cdatastorescratch.h).
//----------------------------------------------------------------------------
void CTask::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);
    store->WriteDword(m_field24);
    store->WriteDword(m_field28);
    store->WriteDwordAlt2(m_field2C);
    store->WriteDword(m_field30);
}

//----------------------------------------------------------------------------
//  0x6F287C60 - slot 19, LoadFlags.  `retn 4`.  Mirrors SaveFlags: m_flags
//  read as a plain dword, then m_field24/m_field28/m_field2C read in one
//  shot as an SHandleTriple (ReadHandleTriple, handletriple.h - the same
//  three-dword record item_loadhandletriple.cpp already established, and
//  the reason m_field2C's default (-1, -1, 0) matches SHandleTriple's own
//  documented construction default), then m_field30 read as a plain dword
//  again.
//----------------------------------------------------------------------------
void CTask::LoadFlags(CDataStore* store)
{
    unsigned int value;

    store->ReadDword(&value);
    m_flags = value;

    ReadHandleTriple(store, reinterpret_cast<SHandleTriple*>(&m_field24));

    store->ReadDword(&value);
    m_field30 = value;
}
