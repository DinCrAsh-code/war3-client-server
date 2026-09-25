//============================================================================
//  NIpse::CPmRegion::DumpDebugState, 0x6F48AD50, vtable slot 16 (+0x40).
//  See pmregion.h for the class-level header comment.
//
//  This class's OWN override (same "not the shared
//  CPresenceTagged::DumpDebugState" story ComputeChecksum's own file has
//  for slot 15). The dump shows SIX calls into LogFormatted, not the four
//  distinct format strings a quick read suggests: the first two are
//  BYTE-FOR-BYTE NTempest::CPresenceTagged::DumpDebugState's own two lines
//  (Agent/presencetagged_dumpdebugstate.cpp - the empty-line-then-"[PrTg]
//  t[%u] b[%u]" pair, reading `this->m_tagT`/`this->m_tagB` at +0x14/+0x18,
//  which this flat/offset-addressed class shares the same layout for),
//  duplicated INLINE here rather than reached through a real call to that
//  address - the dump has no `call sub_6F479C30` anywhere in this
//  function's own body, only direct `call sub_6F4B73F0` (LogFormatted)
//  six times over, so the base class's own two-line body was genuinely
//  duplicated in the original source rather than invoked - the same kind
//  of real, transcription-worthy duplication
//  Agent/agentbaseabscomputechecksum.cpp's own header comment documents for
//  its own class's ComputeChecksum. Two more lines follow - a second empty
//  line, then "[CPmRg]" - and finally this class's own two field lines.
//
//  `retn 4` - one stack argument only (unlike NTempest::CSyncData's own
//  DumpDebugState, csyncdata_dumpdebugstate.cpp, which genuinely does take
//  a second, unread padding dword and cleans `retn 8`).
//============================================================================
#include "pmregion.h"
#include "presence.h"
#include "textsink.h"

namespace NIpse {

//  0x6F48AD50 - vtable slot 16 (+0x40).
void CPmRegion::DumpDebugState(void* sink)
{
    //  Genuinely duplicated NTempest::CPresenceTagged::DumpDebugState body
    //  (presencetagged_dumpdebugstate.cpp) - see this file's own header
    //  comment for why this is transcribed rather than a real base call.
    NTempest::CPresenceTagged* base = (NTempest::CPresenceTagged*)this;
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrTg] t[%u] b[%u]", base->m_tagT, base->m_tagB);

    LogFormatted(sink, "");
    LogFormatted(sink, "[CPmRg]");
    LogFormatted(sink, "\tt[%d] l[%d] b[%d] r[%d]", m_top, m_left, m_bottom, m_right);
    LogFormatted(sink, "\tmsk[0x%X]", m_mask);
}

}  // namespace NIpse
