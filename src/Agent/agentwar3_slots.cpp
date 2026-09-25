//============================================================================
//  CAgentWar3's remaining default vtable slots in the 0x6F001xxx
//  neighbourhood - the ones that neither read nor write m_flags.  See
//  agentwar3.h for the class and agentwar3_flags.cpp for the m_flags half
//  of the same module.
//
//  None of these six calls another, so they share a translation unit; the
//  one out-of-line call any of them makes (GetRecordVersion's) goes to
//  agentdefaults.cpp, which is a different translation unit precisely so
//  that the call survives.
//============================================================================
#include "agentwar3.h"
#include "textsink.h"   // SIntMiniValue::DumpTo, slot 17 below

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

//  0x6F471C10 - see agentdefaults.cpp.

//  The RTTI-shaped tag the two teardown slots hand SMemFree, IDA's
//  `aAvcagentfieldm`.  A `.?AV<class>@@` type-descriptor name with -2 for
//  the line number is this codebase's "free one object of this class"
//  spelling (CDataAllocator::Alloc, dataallocator.cpp, builds the matching
//  allocation the same way); the class it names is reached by nothing else
//  in any dump in this repo, so only its shape is established, not its
//  contents.  String-literal operands canonicalise to an unresolved data
//  symbol on both streams either way, so the exact bytes are not
//  score-relevant.
static const char kAgentFieldMapTag[] = ".?AVCAgentFieldMap@@";

//----------------------------------------------------------------------------
//  0x6F0017A0 - vtable slot 3 (+0x0C).  One stack argument, ignored.
//  CItem overrides it with something that really does look at the argument
//  (0x6F2B7DE0, item_vtable.cpp), which is the only reason to believe the
//  argument means anything at this level.
//----------------------------------------------------------------------------
int CObserver::Method_0x0C(const SAgentMessage*)
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F001AE0 - vtable slot 10 (+0x28).
//----------------------------------------------------------------------------
int CAgentWar3::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 1;
}

//----------------------------------------------------------------------------
//  0x6F001A20 - vtable slot 25 (+0x64).  No arguments, always -1.
//  `or eax, 0FFFFFFFFh` rather than `mov eax, -1` is just MSVC's shorter
//  encoding for the constant; nothing in the source picks between them.
//----------------------------------------------------------------------------
unsigned int CAgentWar3::Method_0x64()
{
    return -1;
}

//----------------------------------------------------------------------------
//  0x6F001DE0 - vtable slot 26 (+0x68).  Five stack arguments, three of
//  them read: step `cursor` on by `advance` when `advance` is non-zero,
//  give `block` back to Storm if it is set, and return the stepped cursor.
//  The first and fifth arguments are never touched.
//
//  The pairing with Method_0x70 below (same tag, same conditional free) and
//  the "advance a cursor" half make this look like the tail of a
//  serialise-into-a-buffer walk, but nothing in these three vtables calls
//  either one, so that is a reading of the instructions and not evidence.
//----------------------------------------------------------------------------
void* CAgentWar3::Method_0x68(int, int advance, void* block, char* cursor, int)
{
    if (advance != 0)
        cursor += advance;
    if (block != 0)
        SMemFree(block, kAgentFieldMapTag, -2, 0);
    return cursor;
}

//----------------------------------------------------------------------------
//  0x6F001AC0 - vtable slot 27 (+0x6C).  Three stack arguments; the second
//  plus one, the other two ignored.
//----------------------------------------------------------------------------
int CAgentWar3::Method_0x6C(int, int count, int)
{
    return count + 1;
}

//----------------------------------------------------------------------------
//  0x6F001E10 - vtable slot 28 (+0x70).  Method_0x68's free half on its
//  own, with four arguments instead of five and no cursor to step.
//----------------------------------------------------------------------------
void CAgentWar3::Method_0x70(int, int, void* block, int)
{
    if (block != 0)
        SMemFree(block, kAgentFieldMapTag, -2, 0);
}


//============================================================================
//  The five slots that were __declspec(naked) redirects until now.  Four of
//  them are in this same 0x6F001xxx run; GetClassName and GetAgileTypeId
//  sit a little lower at 0x6F0015xx, which is still this module (the
//  0x6F0014xx-0x6F001Fxx block is one source file's worth of CAgent and
//  CAgentWar3 leaves).
//============================================================================

//----------------------------------------------------------------------------
//  0x6F001590 - slot 22.
//----------------------------------------------------------------------------
const char* CAgentWar3::GetClassName()
{
    return "CAgentWar3";
}

//----------------------------------------------------------------------------
//  0x6F0015E0 - slot 7.  '+','w','3','a' low byte first - CAgent's own
//  '+aga' (agentdefaults.cpp) with the family byte changed, the same way
//  CWar3Image spells '+w3i' and CWidget '+w3w'.
//----------------------------------------------------------------------------
unsigned int CAgentWar3::GetAgileTypeId() const
{
    return 0x2B773361;
}

//----------------------------------------------------------------------------
//  0x6F001AB0 - slot 9.  A bare `jmp sub_6F471BF0`: a tail jump straight
//  into CAgent::Method_0x24 (agentdefaults.cpp), which is what MSVC emits
//  for a body that is nothing but a qualified call to the base with the
//  same arguments.  The qualification is load-bearing - written unqualified
//  it would dispatch through the vtable back into this same slot and
//  recurse forever.  CWidget's slot 9 (agentwar3_thunks.cpp) is the same
//  shape one class down.
//----------------------------------------------------------------------------
int CAgentWar3::Method_0x24(int context)
{
    return CAgent::Method_0x24(context);
}

//----------------------------------------------------------------------------
//  0x6F001AD0 - slot 29.  `retn 8`: empty, but two stack dwords wide.  No
//  class in the image overrides it, so it keeps a slot-indexed name.
//----------------------------------------------------------------------------
void CAgentWar3::Method_0x74(int, int) {}

//----------------------------------------------------------------------------
//  0x6F001B20 - slot 17.  `add ecx, 20h` then a tail jump to 0x6F472EB0:
//  the dump routine is a member of whatever lives at +0x20, and +0x20 is
//  m_flags (agentwar3.h).  So a CAgentWar3 dumps exactly one thing - its
//  own flags word - and does it by handing the *field* to a dumper rather
//  than by formatting anything itself, which is why the whole body is two
//  instructions.
//----------------------------------------------------------------------------
void CAgentWar3::DumpState(void* sink)
{
    ((SIntMiniValue*)&m_flags)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F001B30 - slot 1, the scalar deleting destructor.
//
//  ~CAgentWar3 is inlined into it - the shipped body stamps
//  ??_7CAgentWar3@@6B@ and then makes a real `call` to ~CAgent - so this
//  function has to live where that definition is visible, which is
//  agentwar3_dtor.cpp, not here.  See that file.
//----------------------------------------------------------------------------
