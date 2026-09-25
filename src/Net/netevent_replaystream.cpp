//============================================================================
//  0x6F54EB00 - CNetEventTypeDispatcher::ReplayAckDispatch.
//
//  Called from CNetEventTypeDispatcher::ConstructAndBroadcast's own type-
//  0x1B/0x10 branches (netevent_dispatch.cpp) whenever a slot's replay
//  record signals "streaming just activated" - this is the "echo the
//  replay header's own startup records a second time, now that streaming
//  has begun" dispatcher the mission doc describes: a jump-table decode
//  on a byte read straight off `self->m_replayStream`
//  (CDataStore::ReadByte, already reconstructed), dispatching to one of
//  ~14 small per-record-type handlers, each of which re-serializes the
//  freshly decoded record into a *second*, throwaway `CDataStore` and
//  hands that plus a clamped broadcast id to a shared "publish this
//  record" tail (`SubmitReplayRecordViaCC10_6F54CC10`,
//  netevent_replaystream_records.cpp).
//
//  asm/sub_6F54EB00_0x6F54EB00_calltree_asm.md (depth-3, 121 functions) is
//  the source.  Its own direct callees (the call graph at the top of that
//  dump) are what netevent_replaystream_records.cpp thunks or
//  reconstructs; deeper functions the depth-3 walk pulled in (the record
//  re-serializers each of the ~14 handlers calls internally, and the
//  chunk-pool internals PreparePayloadBuffer_6F650D40 itself reaches -
//  replaypayloadbuffer.cpp) are not called directly by this function and
//  are out of scope here.
//
//  batch-C-netevent_replaystream (networking_desync_closure_worklist.json)
//  un-thunked this whole closure - this dispatcher plus everything it
//  calls into.  Reading the real dispatcher call sites (0x6F54EB00's own
//  dump) settled three things a previous session's naked-thunk
//  placeholders got wrong here, corrected in this pass:
//
//    * the ~14 per-record-type handlers are NOT "echo one lobby/session
//      record a second time" leaves - each builds its own local
//      `CDataStore`-shaped record from the freshly decoded one and
//      forwards it through the shared publish tail.  Real dispatch
//      logic, not a passthrough (see netevent_replaystream_records.cpp).
//    * the ~14 handlers' own 3 stack arguments were declared in the
//      wrong order (`zero, record, id`) - the real push order, read off
//      every one of the dispatcher's own call sites, is `id, record,
//      zero` (`arg_0` is the clamped broadcast id, `arg_4` the record,
//      `arg_8` the always-0 trailer nothing reads).  Fixed at every call
//      site below.
//    * byte 0x23 (the TURNSSYNCMISMATCH case) calls its own handler
//      (`ReplayEcho_Handler_6F54E210`) *unconditionally* right after
//      building the record (unless the overflow guard trips) - a
//      previous session's code here skipped that call entirely and
//      gated the teardown on the wrong thing.  Fixed below; see that
//      case's own comment.
//
//  Every reconstructed callee this dispatcher reaches for real
//  (netevent_replaystream_records.cpp, replaypayloadbuffer.cpp) lives in
//  its own translation unit apart from this one, purely so this file's
//  own `/Ob2` cannot inline any of them into this dispatcher's own
//  listing the way the shipped, separately-compiled module never did -
//  every one of them is a genuine `call` in this function's own dump,
//  not an inlined body (CLAUDE.md, "one translation unit per original
//  module").
//============================================================================
#include "netdata.h"
#include "cdatastore.h"
#include "netdata_replayrecords.h"   // ReadReplayType0x10Record/0x16Record
#include "savegame.h"                // Net::GAMESETUP_SLOTRECORD

//  0x6F6527E0 - already reconstructed as ReadReplaySlotRecord
//  (netdata_loadreplayheader.cpp, the LoadReplayHeader wave's own name -
//  a trivial passthrough onto SaveGameReadSlotRecord).
CDataStore* __fastcall ReadReplaySlotRecord(CDataStore* store,
                                            Net::GAMESETUP_SLOTRECORD* out);

//  Already reconstructed - CDataStore's own readers (cdatastore.h).
//  ReadByte/ReadWord/ReadRaw are declared there; used directly below.

void CNetData_ParseError(const char* tag);
void __cdecl nullsub_1(const char*, ...);

//  0x6F4C7150 and 0x6F54C7E0 already have real bodies elsewhere
//  (cdatastorecachedrain.cpp / netsessionend.cpp) under funcmap.py's own
//  established names - redeclared here for extern linkage rather than
//  re-thunked under a new name (funcmap.py, tools/thunk_inventory.py's own
//  "one shipped address, two names" defect class).
struct SDataStoreCache;
void __fastcall DataStoreCacheDrain(SDataStoreCache* self);
void __fastcall EndSessionGame(unsigned int eventId, int eventArg, int index);

//  netevent_dispatch.cpp already thunks these two at the same addresses
//  this dispatcher also calls - redeclared here for extern linkage rather
//  than thunked twice (one definition, two callers).
extern void* __fastcall TurnsSyncRecordCtor_6F652870(void* store, void* /*unused_edx*/);
extern void* __fastcall TurnsSyncMismatchRecordCtor_6F653650(void* store, void* /*unused_edx*/);

//  netevent_replaystream_records.cpp - ClampBroadcastId, the ~14 echo
//  handlers, the ~9 dispatcher-level record decoders and
//  SubmitDecodedTurnPayload.  See that file's own header for why these
//  live apart from this dispatcher.
extern unsigned char __fastcall ClampBroadcastId_6F5375E0(unsigned int id);

extern int __fastcall ReplayEcho_CameraBookmark_6F54DC60(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54DD30(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54DE00(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54DB90(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54DED0(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54DFA0(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54E070(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54E140(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54E210(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54E2E0(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54E3B0(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54E480(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54E550(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);
extern int __fastcall ReplayEcho_Handler_6F54E620(
    SNetSessionInfo* self, void* /*unused_edx*/, unsigned char id, void* record, int zero);

extern CDataStore* __fastcall RecordCtor_6F6515B0(CDataStore* store, void* record);
extern CDataStore* __fastcall RecordCtor_6F651620(CDataStore* store, void* record);
extern CDataStore* __fastcall RecordCtor_6F651640(CDataStore* store, void* record);
extern CDataStore* __fastcall RecordCtor_6F651660(CDataStore* store, void* record);
extern CDataStore* __fastcall RecordCtor_6F652820(CDataStore* store, void* record);
extern CDataStore* __fastcall RecordCtor_6F652F50(CDataStore* store, void* record);
extern CDataStore* __fastcall RecordCtor_6F652AE0(CDataStore* store, void* record);

//  0x6F6516F0 / 0x6F651790 - already reconstructed in netmsgio_651.cpp
//  under its own family of names; called directly here rather than
//  re-thunked/re-defined under a second name (funcmap.py,
//  tools/thunk_inventory.py's own "one shipped address, two names"
//  defect class).
extern CDataStore* __fastcall NetMsgReadBytePair_6F6516F0(CDataStore* self, unsigned char* dst);
extern CDataStore* __fastcall NetMsgReadDwordPair_6F651790(CDataStore* self, unsigned char* dst);

extern void __fastcall PreInitRecord_6F548830(void* buf);

extern int __fastcall SubmitDecodedTurnPayload_6F54CAD0(
    SNetSessionInfo* self, void* /*unused_edx*/,
    int a1, void* a2, int a3, int a4, int a5);

//  0x6F650D40 - PreparePayloadBuffer (replaypayloadbuffer.cpp).
extern void* __fastcall PreparePayloadBuffer_6F650D40(unsigned int requestedSize, unsigned int* outCapacity);

//----------------------------------------------------------------------------
//  CNetEventTypeDispatcher::ReplayAckDispatch - 0x6F54EB00.  `this` only,
//  retn bare.
//----------------------------------------------------------------------------
void __fastcall CNetEventTypeDispatcher_ReplayAckDispatch(SNetSessionInfo* self)
{
    //  Only two states run this at all: 2 (STREAMING) or 3 ("no more
    //  data") - read off the same +0x614 field
    //  netdatareplaystate.cpp's SNetDataStates::m_replayState names (this
    //  object is the same SNetSessionInfo either way).
    int replayState = *(int*)((char*)self + 0x614);
    if (replayState != 2 && replayState != 3)
        return;

    //  +0x58C - a per-session flag gating whether m_replayStream.m_field10
    //  gets refreshed from the caller-supplied snapshot
    //  (self->m_routeReplayArg, netevent_dispatch.cpp's own +0xACC) before
    //  the decode loop starts.  Not named further - this is the only
    //  reader/writer either dump reaches.
    int* flag58C = (int*)((char*)self + 0x58C);
    bool haveSavedPos = (*flag58C == 0);
    if (haveSavedPos)
        self->m_replayStream.m_field10 = self->m_routeReplayArg;

    //  var_8FC/var_8F4 in the dump: whether the stream's own +0x62C
    //  ("owns pool-recycled storage") sentinel was already -1 going in.
    bool alreadyIdle = (self->m_replayStream.m_field0xC != -1);
    self->m_routeReplayArg = self->m_replayStream.m_field10;

    if (!alreadyIdle)
    {
        typedef void (__thiscall *CloseFn)(void*);
        void* vtbl = *(void**)&self->m_replayStream;
        CloseFn close = *(CloseFn*)((char*)vtbl + 0x20);   // vtable slot 8
        close(&self->m_replayStream);
    }

    self->m_routeRetry = self->m_routeReplayArg;

readNext:
    if ((unsigned int)self->m_replayStream.m_readPos > (unsigned int)self->m_replayStream.m_field10)
        goto tail;

    {
        unsigned char recType = 0;
        self->m_replayStream.ReadByte(&recType);
        if ((unsigned int)self->m_replayStream.m_readPos > (unsigned int)self->m_replayStream.m_field10)
            goto tail;

        unsigned char localRec[0x60] = { 0 };

        //  ------------------------------------------------------------
        //  The byte-driven decode.  Every branch shares the same shape:
        //  build a small local record off `self->m_replayStream`, bail
        //  the whole dispatch if that overran the stream, snapshot the
        //  new read position into self->m_routeRetry, clamp the record
        //  type into a broadcast id, and hand both to the matching
        //  handler - `(self, id, record, 0)`, the corrected argument
        //  order this file's own header note explains.  Left generic (an
        //  opaque local buffer) rather than modelling each of the ~14
        //  record shapes byte-for-byte - see this file's own header
        //  comment.
        //  ------------------------------------------------------------
#define REPLAY_CASE(rt, ctorFn, handlerFn)                                        \
        case rt:                                                                 \
            ctorFn(&self->m_replayStream, localRec);                             \
            if ((unsigned int)self->m_replayStream.m_readPos >                    \
                (unsigned int)self->m_replayStream.m_field10)                     \
                goto tail;                                                        \
            self->m_routeRetry = self->m_replayStream.m_readPos;                  \
            handlerFn(self, 0, ClampBroadcastId_6F5375E0(rt), localRec, 0);        \
            goto readNext;

        switch (recType)
        {
        REPLAY_CASE(0x17, RecordCtor_6F6515B0,                ReplayEcho_Handler_6F54DB90)
        REPLAY_CASE(0x1A, RecordCtor_6F651620,                ReplayEcho_Handler_6F54DED0)
        REPLAY_CASE(0x1B, RecordCtor_6F651640,                ReplayEcho_Handler_6F54DFA0)
        REPLAY_CASE(0x1C, RecordCtor_6F651660,                ReplayEcho_Handler_6F54E070)
        REPLAY_CASE(0x22, TurnsSyncRecordCtor_6F652870,       ReplayEcho_Handler_6F54E140)
        REPLAY_CASE(0x21, NetMsgReadBytePair_6F6516F0,        ReplayEcho_Handler_6F54E2E0)
        REPLAY_CASE(0x20, RecordCtor_6F652820,                ReplayEcho_Handler_6F54E3B0)
        REPLAY_CASE(0x2F, NetMsgReadDwordPair_6F651790,       ReplayEcho_Handler_6F54E480)
        REPLAY_CASE(0x31, RecordCtor_6F652AE0,                ReplayEcho_Handler_6F54E620)

#undef REPLAY_CASE

        case 0x19:
        {
            //  0x6F6527E0 - already reconstructed as ReadReplaySlotRecord
            //  (netdata_loadreplayheader.cpp).
            Net::GAMESETUP_SLOTRECORD slotRec;
            ReadReplaySlotRecord(&self->m_replayStream, &slotRec);
            if ((unsigned int)self->m_replayStream.m_readPos >
                (unsigned int)self->m_replayStream.m_field10)
                goto tail;
            self->m_routeRetry = self->m_replayStream.m_readPos;
            ReplayEcho_Handler_6F54DD30(self, 0, ClampBroadcastId_6F5375E0(0x19), &slotRec, 0);
            goto readNext;
        }

        case 0x10:
        {
            //  0x6F652560 - already reconstructed as ReadReplayType0x10Record
            //  (netdata_replayrecordreaders.cpp), CNetData::LoadReplayHeader's
            //  own record-type-0x10 reader - the same one netevent_dispatch.
            //  cpp's own route-candidate probe reuses.
            SReplayType0x10Record rec10;
            ReadReplayType0x10Record(&self->m_replayStream, &rec10);
            if ((unsigned int)self->m_replayStream.m_readPos >
                (unsigned int)self->m_replayStream.m_field10)
                goto tail;
            self->m_routeRetry = self->m_replayStream.m_readPos;
            ReplayEcho_CameraBookmark_6F54DC60(self, 0, ClampBroadcastId_6F5375E0(0x10), &rec10, 0);
            goto readNext;
        }

        case 0x16:
        {
            //  0x6F652780 - already reconstructed as ReadReplayType0x16Record
            //  (same file).
            SReplayType0x16Record rec16;
            ReadReplayType0x16Record(&self->m_replayStream, &rec16);
            if ((unsigned int)self->m_replayStream.m_readPos >
                (unsigned int)self->m_replayStream.m_field10)
                goto tail;
            self->m_routeRetry = self->m_replayStream.m_readPos;
            ReplayEcho_Handler_6F54DE00(self, 0, ClampBroadcastId_6F5375E0(0x16), &rec16, 0);
            goto readNext;
        }

        case 0x23:
        {
            //  Byte 0x23 - the TURNSSYNCMISMATCH-shaped record.  The
            //  dump's own case 35 calls the handler *unconditionally*
            //  (unless the overflow guard trips) right after building the
            //  record, then - regardless of whether the handler ran -
            //  checks the record's own +0x40 field and, if non-zero,
            //  tears it down with a logged SMemFree (Storm_403).  A
            //  previous session's own code here skipped the handler call
            //  entirely and gated the teardown on the wrong thing; fixed
            //  to match the dump.
            TurnsSyncMismatchRecordCtor_6F653650(&self->m_replayStream, localRec);
            if ((unsigned int)self->m_replayStream.m_readPos <=
                (unsigned int)self->m_replayStream.m_field10)
            {
                self->m_routeRetry = self->m_replayStream.m_readPos;
                ReplayEcho_Handler_6F54E210(self, 0, ClampBroadcastId_6F5375E0(0x23), localRec, 0);
            }

            unsigned int mismatchValue = *(unsigned int*)((char*)localRec + 0x40);
            if (mismatchValue != 0)
            {
                //  Storm_403 (SMemFree)-based teardown of the object at
                //  `mismatchValue`, mirroring TurnsSyncMismatchRecordDtor_
                //  6F545250's own shape in netevent_dispatch.cpp - not
                //  reconstructed here; this branch's own dump body
                //  (0x6F54EE37) is its own naked thunk target for a
                //  future session, left unimplemented rather than guessed.
            }
            goto readNext;
        }

        case 0x30:
        {
            //  Byte 0x30 - a two-stage builder (PreInitRecord_6F548830
            //  then RecordCtor_6F652F50).
            PreInitRecord_6F548830(localRec);
            RecordCtor_6F652F50(&self->m_replayStream, localRec);
            if ((unsigned int)self->m_replayStream.m_readPos >
                (unsigned int)self->m_replayStream.m_field10)
                goto tail;
            self->m_routeRetry = self->m_replayStream.m_readPos;
            ReplayEcho_Handler_6F54E550(self, 0, ClampBroadcastId_6F5375E0(0x30), localRec, 0);
            goto readNext;
        }

        case 0x1D:
        case 0x1E:
        {
            //  This session's own best read of the "raw turn-payload" path
            //  puts it somewhere in this dispatcher, but the exact record-
            //  type byte pair it keys on was not pinned down against the
            //  dump with confidence - 0x1D/0x1E are placeholders, not
            //  confirmed values (0x10/0x16 are the two confirmed values,
            //  reused above from ReadReplayType0x10Record/0x16Record).
            //  The raw turn-payload path: read a 2-byte
            //  length prefix, size-check it against the stream's own
            //  remaining bytes, and either submit the decoded payload
            //  (SubmitDecodedTurnPayload_6F54CAD0, with the id/flag byte
            //  clamped the same way every other case's broadcast id is,
            //  the payload's own reported capacity as its fourth
            //  argument, and a trailing 0) or, if not enough data is
            //  available yet, push the read position back by one and stop
            //  the whole dispatch for this call (the dump's own "not
            //  enough data yet, retry next time" branch).
            unsigned short len = 0;
            self->m_replayStream.ReadWord(&len);
            if ((unsigned int)self->m_replayStream.m_readPos >
                (unsigned int)self->m_replayStream.m_field10)
                goto tail;

            unsigned int remaining = self->m_replayStream.m_field10 - self->m_routeRetry;
            if (remaining < len)
            {
                self->m_replayStream.m_field10 += 1;
                goto tail;
            }

            unsigned int capacity;
            void* payload = PreparePayloadBuffer_6F650D40(len, &capacity);
            self->m_replayStream.ReadRaw(payload, len);
            self->m_routeRetry = self->m_replayStream.m_field10;

            SubmitDecodedTurnPayload_6F54CAD0(self, 0, ClampBroadcastId_6F5375E0(recType),
                                               payload, len, (int)capacity, 0);
            goto readNext;
        }

        default:
            nullsub_1("CNetData::EnqueueReplayTurn: Bad replay record type %d\n", (int)recType);
            EndSessionGame(7, 0, self->m_activeIndex);
            goto tail;
        }
    }

tail:
    if (self->m_flagBE4 == 1)
        DataStoreCacheDrain((SDataStoreCache*)&self->m_replayStream);

    if (self->m_routeRetry == 0)
    {
        if (!alreadyIdle)
            self->m_replayStream.m_field10 = -1;
        self->m_replayStream.m_field10 = self->m_routeReplayArg;
        self->m_replayStream.m_readPos = self->m_routeRetry;
    }
}
