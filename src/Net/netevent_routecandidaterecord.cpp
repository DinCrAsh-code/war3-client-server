//============================================================================
//  0x6F543A40/0x6F543B00/0x6F543BC0/0x6F543C80 - AppendRouteCandidateRecord:
//  four sibling "build one wire record and serialize it" bodies, each
//  constructing a local CDataStoreCache1460 (the same 1460-byte inline-
//  buffer CDataStore view tock_relay_builders.cpp's own BuildRelay0/1
//  already establish - re-declared identically here for extern linkage),
//  writing `payload` into it through its own reserialize/write helper
//  (sub_6F652510/sub_6F652750/sub_6F6527B0 - each its own further,
//  un-investigated record-reserialize subsystem, stay naked thunks; the
//  fourth, sub_6F651610, is netmsgio_651.cpp's own already-real
//  NetMsgWriteDword_6F651610), then handing the finished local store to
//  SerializeExtraHeader_6F537AA0 along with the wire type code.  retn 8
//  (two stack args) confirmed against each.
//
//  Signature correction: the shipped call sites (in DesyncCandidateCheck_
//  6F5453A0, netevent_dispatch.cpp) push the wire-type constant LAST - i.e.
//  it lands in `arg_0`, the first stack parameter - and the record pointer
//  FIRST - landing in `arg_4`, the second - confirmed against each body's
//  own `mov ecx,[esp+arg_0]` feeding SerializeExtraHeader's `msgType` and
//  `mov edi,[esp+arg_4]` feeding the reserialize call. netevent_dispatch.cpp
//  previously declared `(candidateName, payload)` in the opposite roles and
//  order (a pre-existing bug this session fixes): calling that declaration
//  with a real candidate-name pointer as `arg_0` and a bare wire-type
//  integer (0x10/0x16/0x19) as `arg_4` would have handed each reserialize
//  helper a near-null pointer to dereference - a live crash the very first
//  time this path runs, invisible to `verify.py` (an indirect naked thunk
//  call costs nothing to the score either way) and to `thunk_abi_audit.py`
//  (which checks stack *byte count*, not argument identity).
//
//  Its own translation unit, split out of netevent_dispatch.cpp: all four
//  are called from a tight loop in DesyncCandidateCheck_6F5453A0
//  (same file otherwise), and this build's /Ob2 inlined each of these small
//  bodies straight into that caller when they shared a TU - producing an
//  entirely different instruction shape at every call site, which the
//  shipped code (four real `call sub_6F543Axx`) never does. Same reasoning
//  as CLAUDE.md's own "one translation unit per original module" rule.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

//  0x6F537AA0 - already reconstructed (netevent_serializeextraheader.cpp).
void __fastcall SerializeExtraHeader_6F537AA0(void* self, void* /*unused_edx*/,
                                               unsigned int msgType, void* dataPtr);

class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

//  Already reconstructed (as naked thunks) in netevent_replaystream_records.cpp;
//  re-declared identically here for extern linkage - see that file's own
//  comment for why each stays a naked thunk (out of scope for this batch).
extern void __fastcall RecordReserialize_6F652510(void* destStore, void* srcRecord);
extern void __fastcall RecordReserialize_6F652750(void* destStore, void* srcRecord);
extern void __fastcall RecordReserialize_6F6527B0(void* destStore, void* srcRecord);
extern CDataStoreScratch* __fastcall NetMsgWriteDword_6F651610(CDataStoreScratch* self,
                                                                const unsigned int* src);
//  Already reconstructed (netevent_replaystream_records.cpp) - real body,
//  not a thunk.
extern void __fastcall RecordReserialize_6F652840(void* destStoreVoid, void* srcRecord);

void __fastcall AppendRouteCandidateRecord_6F543A40(void* self, void* /*unused_edx*/,
                                                      unsigned int typeCode, void* payload)
{
    CDataStoreCache1460 store;
    RecordReserialize_6F652510(&store, payload);
    SerializeExtraHeader_6F537AA0(self, 0, typeCode, &store);
}

void __fastcall AppendRouteCandidateRecord_6F543B00(void* self, void* /*unused_edx*/,
                                                      unsigned int typeCode, void* payload)
{
    CDataStoreCache1460 store;
    RecordReserialize_6F652750(&store, payload);
    SerializeExtraHeader_6F537AA0(self, 0, typeCode, &store);
}

void __fastcall AppendRouteCandidateRecord_6F543BC0(void* self, void* /*unused_edx*/,
                                                      unsigned int typeCode, void* payload)
{
    CDataStoreCache1460 store;
    RecordReserialize_6F6527B0(&store, payload);
    SerializeExtraHeader_6F537AA0(self, 0, typeCode, &store);
}

void __fastcall AppendRouteCandidateRecord_6F543C80(void* self, void* /*unused_edx*/,
                                                      unsigned int typeCode, void* payload)
{
    CDataStoreCache1460 store;
    NetMsgWriteDword_6F651610((CDataStoreScratch*)&store, (const unsigned int*)payload);
    SerializeExtraHeader_6F537AA0(self, 0, typeCode, &store);
}

//  0x6F543980 - checksum-round2-G (2026-09-13): a fifth sibling of the
//  same "build one wire record and serialize it" family above, found via
//  `worktree_store.py xrefs 0x6F551D80 --from` rather than
//  DesyncCandidateCheck_6F5453A0's own four-appender loop - its one real
//  caller is CNetEventTypeDispatcher_ConstructAndBroadcast's own table2
//  case 34 (msgType 0x22, "TURNSSYNC"), with `typeCode` always the literal
//  0x22 at that call site (`push 22h` right before `call sub_6F543980`,
//  netevent_dispatch.cpp's own dump) rather than a variable the way the
//  four DesyncCandidateCheck siblings pass it.
//
//  Concrete, not-yet-chased connection worth flagging for a follow-up:
//  the caller builds `payload` immediately before this call from a
//  length-prefixed `memcpy` (`al` = a length byte, then that many raw
//  bytes) fed by a `TrustedSerializeCacheCtor_6F53FAB0`/
//  `NetMsgWriteDword_6F537820`-built scratch object that writes
//  `self+0x1C64` (the applied-turn-delta / per-tick sync value this whole
//  document's "Part B" section already tracks) through an unidentified
//  vtable slot at `[eax+0x24]` first - i.e. this is very plausibly the
//  "downstream flush was not traced further this batch" scratch object
//  Part B's own note left open, now with a real destination (this
//  function, then `SerializeExtraHeader_6F537AA0` onto the wire) rather
//  than an unknown one. Reconstructing that whole surrounding block (the
//  `var_244`/`var_2D4` locals, the `[eax+0x24]` dispatch, and the
//  length-prefixed copy itself) is a real follow-up in its own right, out
//  of this batch's own scope - only this one leaf function is
//  reconstructed here.
void __fastcall AppendTurnsSyncRecord_6F543980(void* self, void* /*unused_edx*/,
                                                 unsigned int typeCode, void* payload)
{
    CDataStoreCache1460 store;
    RecordReserialize_6F652840(&store, payload);
    SerializeExtraHeader_6F537AA0(self, 0, typeCode, &store);
}
