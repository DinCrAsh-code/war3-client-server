//============================================================================
//  NTempest::CSyncData - a generic "one cached checksum dword plus a debug
//  name string" wrapper, RTTI-confirmed (`.?AVCSyncData@NTempest@@`) as the
//  real class embedded at CNetData's own +0x2278 (this repo's
//  `SNetSessionInfo::m_reserved2278`, Net/netdata.h) - see
//  docs/notes/checksum-provider-registry.md, "LIVE FINDING + RTTI
//  confirmation: CNetData+0x2278's real class is NTempest::CSyncData".
//
//  A live breakpoint at CNetData's own ComputeChecksum thunk
//  (0x6F537E10/0x6F537E16, right after it loads the embedded object's own
//  vtable slot 1) reported `edx == 0x6F536BE0` - `??_7CSyncData@NTempest@@6B@`
//  (0x6F9581A4)'s own slot 1, whose containing vtable's slot-0 RTTI
//  `type_info` string (0x6FA89850) reads `.?AVCSyncData@NTempest@@`.  That
//  settled the class identity without ever finding a static promotion site
//  at the time - but see the cnd-ctor-agent session
//  (docs/notes/checksum-provider-registry.md) for the correction: the real
//  constructor is `CNetData::CNetData` at 0x6F550460 (not 0x6F54FC50, which
//  is actually `~CNetData()`), and it stamps `??_7CSyncData@NTempest@@6B@`
//  directly at `+0x2278` - the promotion site this note went looking for.
//  `0x6F54FC50`'s own `??_7CEntity@NTempest@@6B@` stamp at the same offset
//  is `~CNetData()`'s ordinary mid-unwind restamp to the base class, not a
//  second, competing construction.
//
//  Not modelled as deriving from `NTempest::CEntity` (the vtable's own base,
//  per the RTTI chain the doc above walks): CEntity has no C++
//  reconstruction of its own in this repo yet, and CLAUDE.md's own house
//  rule is to stay on this exact checksum family rather than pull in an
//  unrelated base purely to make the inheritance syntactically real. The
//  vtable pointer is carried as a plain field instead, the same way
//  Net/netcommand_dispatch.cpp's own `SNetEventSaveComplete` does for a
//  vtable this repo has not reconstructed.
//
//  Not wrapped in `namespace NTempest` (unlike the RTTI-confirmed mangled
//  name) even though other files in this repo do use that namespace for
//  real (Agent/presencetagged_dumpdebugstate.cpp): a `::`-qualified
//  compiler comment (`NTempest::CSyncData_GetChecksum`) does not literally
//  match this file's own bare `tools/funcmap.py` readable name
//  (`CSyncData_GetChecksum`), which is exactly the "no `; readable-name`
//  comment for extract_gen.py to key off" gap Misc/cheatdata.cpp's own
//  Crc32/SErrSetLastError note already documents for an unrelated symbol -
//  scoring these three came back "not compiled into src/" until the
//  namespace was dropped.
//
//  Layout (4 members, 0x10 bytes - matches the two call sites that read
//  past it, `CNetData+0x2278` through `+0x2288` exclusive, `netdata.h`'s own
//  `m_reserved2278`):
//    +0x00 - vtable pointer (`??_7CSyncData@NTempest@@6B@`, 5 slots)
//    +0x04 - m_checksum - the cached checksum dword; GetChecksum() just
//            returns it, so whatever real value ends up here has to be
//            written by a setter this repo has not found yet (Reset() only
//            clears it back to 0).
//    +0x08 - m_count - a companion count DebugDumpTo prints alongside the
//            checksum ("count[%u]"); Reset() clears it too.
//    +0x0C - m_name - a debug-name string DebugDumpTo prints
//            ("[CSyncData] (%s)").
//============================================================================
#ifndef SYNCDATA_H
#define SYNCDATA_H

struct CSyncData
{
    void*        m_vtable;    // +0x00 - ??_7CSyncData@NTempest@@6B@, 0x6F9581A4
    unsigned int m_checksum;  // +0x04
    unsigned int m_count;     // +0x08
    const char*  m_name;      // +0x0C
};

//  0x6F536BE0 - vtable slot 1.  A plain cached-field getter, not a
//  computation - see the file header above for why nothing in this repo has
//  yet found the real writer of m_checksum.
unsigned int __fastcall CSyncData_GetChecksum(CSyncData* self);

//  0x6F536C10 - vtable slot 2.  `Reset()`: zeroes both the checksum and the
//  count.
void __fastcall CSyncData_Reset(CSyncData* self);

//  0x6F536C30 - vtable slot 4.  `DebugDumpTo(sink, arg1)`: prints the name
//  and the checksum/count pair through the already-reconstructed variadic
//  sink (Storm/textsink.h's LogFormatted-shaped sub_6F4B73F0, out of scope,
//  declared not defined - see .cpp).  `retn 8` (two stack dwords) but only
//  the first is ever read by this override; the second is carried as an
//  unused parameter the same way this codebase already carries an unused
//  `edx` register slot in several other family members (e.g.
//  Net/netcommand_dispatch.cpp's `CNetData_NotifyResyncComplete`) rather
//  than guessing what it is.
void __fastcall CSyncData_DebugDumpTo(CSyncData* self, void* /*unused_edx*/,
                                       void* sink, unsigned int /*unused_arg*/);

#endif
