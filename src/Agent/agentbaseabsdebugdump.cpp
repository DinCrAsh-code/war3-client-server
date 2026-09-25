//============================================================================
//  0x6F4A62D0 - CAgentRelation::DumpDebugState.  Prints this relation's own
//  presence/birth tag pair (offsets +0x14/+0x18 - within the still-flat
//  "base-class territory" agentrelation.h's own m_reserved00[0x1C] covers;
//  this is the first call tree to establish what two of those bytes are:
//  NTempest::CPresenceTagged's own tag pair, matching
//  checksum-provider-registry.md's dead debug print, which names the same
//  shape "next presence tag %05d next birth tag %05d"), then
//  m_field44/m_eventId/m_flags, then - reloading `this` as endpoint A -
//  the *same* endpoint's own presence/birth tag pair twice, once under the
//  "alph" label and once under "beta".
//
//  **The double "alph"/"beta" print of the same endpoint is real, not a
//  transcription slip.**  The shipped code reloads `esi` from `this+0x30`
//  (m_endpointA) exactly once and never again before the second dump - so
//  there is no separate "endpoint B" read here despite the two labels
//  suggesting one.  Kept exactly as written per CLAUDE.md's "say why when
//  the source looks odd" rule; this reads as the original debug dump
//  literally printing the same field twice under two names, not as a bug
//  this reconstruction should "fix".
//
//  Own translation unit: LogFormatted and FormatCFloat are both real calls,
//  redirected rather than reconstructed (Storm/textsink.h).
//
//  `ValueName` (0x6F87529C, `agent_worktrees` `resolve` confirms it as a
//  real 4-byte *data* symbol, all-zero - an empty C string) is passed by
//  address as the format string on both calls below. It is a genuinely
//  IDA-named symbol ("ValueName"), not one of the `dword_`/`unk_`/...-
//  prefixed auto-generated names `build_mix.py`'s own `DATA_ADDR` regex
//  can pull an address out of - CLAUDE.md's link_check.py section calls
//  this shape out by name ("a DATA row whose IDA name is not address-
//  shaped ... has nothing to bind to and is glue.cpp's to provide"), so
//  this is a real, local *definition* (not an `extern` funcmap.DATA
//  binding into the shipped image - a live LNK2019 caught that mismatch)
//  matching the shipped bytes exactly; nothing else in this repo reads or
//  writes this address, so a private copy cannot desync from anything.
//============================================================================
#include "agentrelation.h"
#include "textsink.h"

static char g_valueNameTag[4] = { 0, 0, 0, 0 };   // 0x6F87529C (IDA: ValueName)

void CAgentRelation::DumpDebugState(void* sink)
{
    LogFormatted(sink, g_valueNameTag);

    const unsigned int* presenceTag = (const unsigned int*)((char*)this + 0x14);
    LogFormatted(sink, "[PrTg] t[%u] b[%u]", presenceTag[0], presenceTag[1]);

    LogFormatted(sink, g_valueNameTag);
    LogFormatted(sink, "[PrRe]");

    LogFormatted(sink, "\tdlt[%s] uid[%u] opt[0x%X]", FormatCFloat(&m_field44, 1, -1), m_eventId, m_flags);

    const unsigned int* endpointTag = (const unsigned int*)m_endpointA;
    if (endpointTag != 0)
        LogFormatted(sink, "\talph t[%u] b[%u]", endpointTag[5], endpointTag[6]);
    if (endpointTag != 0)
        LogFormatted(sink, "\tbeta t[%u] b[%u]", endpointTag[5], endpointTag[6]);
}
