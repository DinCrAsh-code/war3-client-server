//============================================================================
//  NetTickRateBump_6F54CC80 (0x6F54CC80) / NetTickRateDrop_6F54CCE0
//  (0x6F54CCE0) - the two Battle.net-timer event handlers
//  InitializeNetSessionConfig (netdata_initconfig.cpp) registers at slots
//  0x18/0x19.  Both take the same shape: if this thread's own
//  `SNetSessionInfo` session tag is still "NONE" (no session up yet),
//  forward straight to NotifyTurnPayload_6F650DB0 with the event's own
//  dest/size fields; otherwise clamp a byte read out of the event record
//  and submit it as a decoded turn payload through
//  SubmitDecodedTurnPayload_6F54CAD0 (both already reconstructed,
//  Net/netevent_replaystream_records.cpp).
//
//  Neither this repo nor the dump names the event-record type the two
//  handlers receive as `self` (`+4` a byte, `+8`/`+0xC`/`+0x10` a
//  dest/size/size2 triple matching SubmitDecodedTurnPayload's own a2/a3/a4)
//  - reached only by raw offset, the same way CLAUDE.md's own "only touched
//  members get names" rule treats every other untyped event blob in this
//  closure.
//
//  The clamp shapes differ by one instruction each and are kept as
//  literally shaped as the dump: Bump clamps *before* comparing (raw < 0x40
//  keeps it, else saturates to 0xFF); Drop adds first and then compares the
//  *wrapped* byte against 0xFF - which can only ever be true when the
//  addition already produced exactly 0xFF, making its own `or eax,0FFh` a
//  practical no-op the shipped code still carries.
//============================================================================
#include "netdata.h"
#include "gamecontext.h"

extern int __fastcall SubmitDecodedTurnPayload_6F54CAD0(
    SNetSessionInfo* self, void* /*unused_edx*/,
    int a1, void* a2, int a3, int a4, int a5);
extern void __fastcall NotifyTurnPayload_6F650DB0(void* self, void* size);

void __fastcall NetTickRateBump_6F54CC80(void* self)
{
    SNetSessionInfo* session = (SNetSessionInfo*)NetDataRecords();

    if (session->m_sessionTag == kNetSessionTagNone)
    {
        NotifyTurnPayload_6F650DB0(*(void**)((char*)self + 8),
                                   *(void**)((char*)self + 0x10));
        return;
    }

    unsigned char raw = *(unsigned char*)((char*)self + 4);
    unsigned int id = (raw < 0x40) ? raw : 0xFF;

    SubmitDecodedTurnPayload_6F54CAD0(session, 0, id,
                                      *(void**)((char*)self + 8),
                                      *(int*)((char*)self + 0xC),
                                      *(int*)((char*)self + 0x10), 1);
}

void __fastcall NetTickRateDrop_6F54CCE0(void* self)
{
    SNetSessionInfo* session = (SNetSessionInfo*)NetDataRecords();

    if (session->m_sessionTag == kNetSessionTagNone)
    {
        NotifyTurnPayload_6F650DB0(*(void**)((char*)self + 8),
                                   *(void**)((char*)self + 0x10));
        return;
    }

    unsigned char raw = *(unsigned char*)((char*)self + 4);
    unsigned int id = (unsigned char)(raw + 0x40);
    if ((unsigned char)id >= 0xFF)
        id = 0xFF;

    SubmitDecodedTurnPayload_6F54CAD0(session, 0, id,
                                      *(void**)((char*)self + 8),
                                      *(int*)((char*)self + 0xC),
                                      *(int*)((char*)self + 0x10), 1);
}
