//============================================================================
//  0x6F67A2F0 - Net::NetClient vtable slot 5 (networking wave 8,
//  agent-networking-wave3-2026-08-29's claim, continued).
//
//  A getter, not a member of the seven-slot notify family (waves 5-7) even
//  though it shares that family's own record-table walk shape
//  (g_netClientRecordTableGuard/g_netClientRecordListHead, MaskedPositive,
//  this->field4 as the record key - all reused verbatim from
//  netclient_recvappend.cpp/netclient_notifyevent_0x1f.cpp): given one
//  stack output pointer, it finds this connection's own record and, only
//  if found, copies two dwords out of it (record+0x138/+0x13C - the same
//  offset range NetClientRecordFields already reserves as padding between
//  m_state (+0x130) and m_provider (+0x148), so no field name conflict) -
//  otherwise leaves the caller's buffer untouched. What those two dwords
//  actually mean (a timestamp pair, an address+port, a byte-count pair) is
//  not established anywhere in this call tree; kept as two anonymous
//  dwords per CLAUDE.md's own rule against inventing field names the dump
//  does not support.
//
//  Two structural details the disassembly makes explicit and this source
//  reproduces on purpose:
//   - the immediate-return path (record key itself is null) never takes
//     the global guard at all - matching the shipped code's own very first
//     branch, `test esi,esi / jz` straight to `pop esi / retn 4` before
//     the `Wait(0)` call;
//   - the "not found" path (guard taken, list empty or exhausted) still
//     signals the guard before returning, exactly like the notify family's
//     own "record not found" early exit - SignalOne(0) is prepared via
//     the shipped code's own tail-jmp-with-argument-slot-reuse trick,
//     reproduced here as an ordinary call since a hand-tail-called-looking
//     wrapper would not itself change the instruction stream once /O2
//     tail-call-optimises the trailing SignalOne call in the same shape.
//============================================================================

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

//  netclient_recvappend.cpp - the same global guard and list head every
//  other NetClient record-table walk in this file uses.
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

//  Field layout matches NetClientRecordFields (netclient_recvappend.cpp)
//  through +0x148; this getter additionally reads the two dwords at
//  +0x138/+0x13C that file's own struct currently reserves as padding.
//  Redeclared locally with only the fields this file reads named, per
//  CLAUDE.md's own rule against merging two independent partial views of
//  one struct without reconciling them first.
struct NetClientRecordFields
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x138 - 0x10];
    unsigned int m_field138;   // +0x138
    unsigned int m_field13C;   // +0x13C
};

//  A masked field is only ever a positive object pointer or a poisoned
//  non-positive encoding - the same idiom netclient_recvappend.cpp's own
//  MaskedPositive documents, redeclared locally here (not exported through
//  a header) the same way every other user of it in this directory does.
static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

struct NetClientGetSelf
{
    void GetField138Pair(void* out);
};

void NetClientGetSelf::GetField138Pair(void* out)
{
    void* self = this;
    void* key = *(void**)((char*)self + 4);
    if (!key)
        return;

    g_netClientRecordTableGuard.Wait(0);

    //  Only the list *head* goes through the full "positive pointer or
    //  poisoned encoding" mask; the per-node `m_nextRecord` step below is
    //  tested with a plain `> 0`, matching the shipped code's own
    //  do-while shape exactly (a second re-mask on every step would cost
    //  an extra `setle`/`sub`/`and` the target's own loop body does not
    //  have - see docs/targets/NetRouter_NetPlayer_NetClient_oneslot_sweep.md's
    //  wave 8 section).
    NetClientRecordFields* rec = (NetClientRecordFields*)MaskedPositive(g_netClientRecordListHead);
    if (rec)
    {
        do
        {
            if ((void*)rec->m_recordKey == key)
                goto found;
            rec = (NetClientRecordFields*)rec->m_nextRecord;
        } while ((int)rec > 0);
    }

    g_netClientRecordTableGuard.SignalOne(0);
    return;

found:
    ((unsigned int*)out)[0] = rec->m_field138;
    ((unsigned int*)out)[1] = rec->m_field13C;
    g_netClientRecordTableGuard.SignalOne(0);
}
