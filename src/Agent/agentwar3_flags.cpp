//============================================================================
//  CAgentWar3's m_flags accessors - the three vtable slots that read or
//  write the whole +0x20 word rather than one bit of it.  See agentwar3.h
//  for why the field lives in this class and not in CWidget/CSelectable.
//
//  All three sit in the low 0x6F001xxx neighbourhood with CAgent::CAgent()
//  (0x6F001730), CAgent::~CAgent() (0x6F0017F0) and CAgent::ReleaseSelf()
//  (0x6F001F70), so CAgent and CAgentWar3 shipped in one module and this
//  file is a slice of it; none of the three calls another, so grouping
//  them costs no inlining that the shipped code does not already have.
//============================================================================
#include "agentwar3.h"
#include "cdatastore.h"
#include "cdatastorescratch.h"

//----------------------------------------------------------------------------
//  0x6F001AF0 - vtable slot 18 (+0x48).  Push the flags word into the
//  stream.  A `call` and a `retn 4` rather than a tail `jmp`: this function
//  has its own stack argument to clean, so the callee's own cleanup cannot
//  stand in for it.
//----------------------------------------------------------------------------
void CAgentWar3::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);
}

//----------------------------------------------------------------------------
//  0x6F001B00 - vtable slot 19 (+0x4C).  Read it back.
//
//  The shipped code reads straight into its own incoming argument slot
//  (`lea eax,[esp+arg_0]` for the out-pointer, then `mov ecx,[esp+arg_0]`
//  again afterwards to pick the value back up) - that is MSVC reusing the
//  now-dead parameter's home for the local, not a second variable, so the
//  source is the ordinary two-step it looks like.
//----------------------------------------------------------------------------
void CAgentWar3::LoadFlags(CDataStore* store)
{
    unsigned int value;
    store->ReadDword(&value);
    m_flags = value;
}

//----------------------------------------------------------------------------
//  0x6F001BE0 - vtable slot 24 (+0x60).  The whole word, no masking: this
//  is the setter LoadFlags would have used if it went through the vtable
//  instead of storing directly.
//----------------------------------------------------------------------------
void CAgentWar3::SetFlags(unsigned int flags)
{
    m_flags = flags;
}
