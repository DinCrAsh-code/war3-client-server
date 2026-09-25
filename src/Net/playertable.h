//============================================================================
//  Net::NetRouter::PlayerTable - shared type declarations.
//
//  See playertable_installhandler.cpp's own header comment for the full
//  trace (0x6F67F3F0, the "open lead" CLAUDE.md's networking wave 1 pointed
//  at). Split into three translation units - one per shipped address -
//  because ArmRetryTimer (0x6F673720) and ClearRecordConnection
//  (0x6F6737E0) are small enough that /Ob2 inlines them straight into
//  InstallHandler's own body if left in the same TU, which is not what the
//  shipped binary does (each is a real, separately addressed `call`) -
//  CLAUDE.md's own "one TU per original module" rule, applied at function
//  granularity here because each of the three has its own real address.
//============================================================================
#ifndef PLAYERTABLE_H
#define PLAYERTABLE_H

#include "storm.h"

//----------------------------------------------------------------------------
//  netprovider_method0x90.cpp's own class, minimally re-declared here the
//  same way netprovider_slot2.cpp already does for its one method - only
//  the one call this file makes needs to be visible, not the real layout.
//----------------------------------------------------------------------------
class NetProviderMethod0x90Self
{
public:
    int CheckOrRegister();   // sub_6F656FE0
};

//  Sync/critsec.cpp's own function.
long __fastcall InterlockedIncrementAt(volatile long* target);   // 0x6F6C4060

struct PlayerRecord
{
    char           pad_000[0x10];
    long           m_refCount;     // +0x10 - InterlockedIncrementAt's own target, on the other-player path
    char           pad_014[0x3C - 0x14];
    void*          m_connection;   // +0x3C - non-null once a real connection is attached
    char           pad_040[0x58 - 0x40];
    int            m_field058;     // +0x58 - zeroed by ClearRecordConnection; not otherwise read in this call tree
    unsigned char  m_flags;        // +0x5C - bit 0x2 is the "needs reset" flag InstallHandler's self path clears
    unsigned char  m_field05D;     // +0x5D - set to 0xFF by ClearRecordConnection
    char           pad_05E[0xB4 - 0x5E];
    unsigned char  m_playerId;     // +0xB4 - 1-based; used as (1 << (id-1)) in a bitmask elsewhere in this file
};

typedef TSExplicitList<PlayerRecord> PlayerRecordList;

//  The "pending connect request" list: a node's own TSLink lives at the
//  node's own address (no separate runtime link-offset field), the same
//  shape src/Net/netprovider_pendinglist.cpp's own DirectLinkedList
//  documents - so a walk steps through `m_prevlink` directly with nothing
//  added, and the table's own terminator is a bare TSLink<T> rather than a
//  full TSExplicitList<T>.
struct PendingConnectRequest
{
    void*                    m_next;      // +0x00 - TSLink's own m_next; unread by this function
    PendingConnectRequest*   m_prevlink;  // +0x04 - walked directly (see above)
    char                     pad_08[0x10 - 0x08];
    long                     m_refCount;  // +0x10 - InterlockedIncrementAt's own target
    char                     pad_14[0x18 - 0x14];
    unsigned char*           m_idBuf;     // +0x18 - m_idBuf[0] is the target player id this request is for
};

class NetRouterDispatchTable
{
public:
    void RegisterPending(void* result);   // sub_6F679D90, retn 4 - real thiscall member
};
extern NetRouterDispatchTable g_netRouterDispatchTable;   // dword_6FACFFA8, funcmap.DATA row

class PlayerTable
{
public:
    char              pad_000[0x5C];
    void*             m_field05C;        // +0x05C - address taken, forwarded opaque into InstallSelfCallback
    char              pad_060[0x114 - 0x60];
    PlayerRecordList  m_records;         // +0x114 (12 bytes: TSExplicitList<PlayerRecord>)
    char              pad_120[0x140 - 0x120];
    void*             m_field140;        // +0x140 - opaque, forwarded verbatim
    char              pad_144[0x148 - 0x144];
    PlayerRecord*     m_selfRecord;      // +0x148 - this machine's own player record
    char              pad_14C[0x170 - 0x14C];
    unsigned int      m_nextRetryTick;   // +0x170 - GetTickCount() deadline, ArmRetryTimer's own field
    char              pad_174[0x19C - 0x174];
    void*             m_field19C;        // +0x19C - opaque, forwarded verbatim
    char              pad_1A0[0x1A8 - 0x1A0];
    void*             m_field1A8;        // +0x1A8 - opaque, forwarded verbatim
    char              pad_1AC[0x1D0 - 0x1AC];
    void*             m_field1D0;        // +0x1D0 - opaque, forwarded verbatim
    void*             m_field1D4;        // +0x1D4 - address taken, never read by ArmRetryTimer/ClearRecordConnection/InstallHandler
    void*             m_field1D8;        // +0x1D8 - address taken, never read by ArmRetryTimer/ClearRecordConnection/InstallHandler
    char              pad_1DC[0x244 - 0x1DC];
    int               m_installedFlag;   // +0x244 - 1 on the self-install path, 0 on the other-player path
    char              pad_248[0x258 - 0x248];
    TSLink<PendingConnectRequest> m_pendingTerm;  // +0x258 (m_next @0x258, m_prevlink @0x25C)
    unsigned char     m_pendingPlayerId; // +0x260
    unsigned char     m_pendingGen;      // +0x261
};

//  0x6F673720 - playertable_armretrytimer.cpp. Free `__fastcall(self,
//  timeoutMs)`: `edx` carries a genuine second argument at every call
//  site, which plain thiscall never puts there.
void __fastcall ArmRetryTimer(PlayerTable* self, unsigned int timeoutMs);

//  0x6F6737E0 - playertable_clearrecordconnection.cpp. Same free-fastcall
//  reasoning as ArmRetryTimer.
void __fastcall ClearRecordConnection(PlayerTable* self, PlayerRecord* record);

#endif  // PLAYERTABLE_H
