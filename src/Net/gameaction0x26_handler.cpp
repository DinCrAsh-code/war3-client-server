//============================================================================
//  0x6F671000 - Probably_W3GS_ActionHandler's (0x6F672340) case 38 handler,
//  `PacketId.GameAction` (0x26) - per the user's own correction, this
//  dispatcher (the *first* one, not "MaybeServerVersion") switches on the
//  top-level W3GS packet type, and case 38 is where a raw `GameAction`
//  packet - one player's queued game actions, still serialized - actually
//  lands. This function is genuinely host-exclusive: only a host ever
//  receives a raw `GameAction` packet directly from a client (every other
//  peer only ever sees the host's own relayed `Tock`), matching what this
//  body does - validate the block, then queue its *raw, still-serialized*
//  bytes into a per-player buffer for the host to relay, not decode any
//  individual action's own fields - that per-verb decode (where a real
//  `GameActionId.DropOrGiveItem`, 0x13, would actually get switched on)
//  happens on the receiving end of the `Tock` this handler's queue
//  eventually produces: case 39 of *this same dispatcher*
//  (`sub_6F66FCA0`, `PacketId.Tock` 0x27) - not reconstructed yet, see
//  that function for the current investigation.
//
//  Shape:
//    1. Guard: `self`'s own +0x40 field must be set (a "ready"/session
//       pointer of some kind - nothing in this call tree says what
//       object it points to), and the declared block length (`declaredLen`,
//       0x26's own trailing argument) must be 4..0x404 - a 4-byte checksum
//       header plus up to 0x400 bytes of actual action data.
//    2. Read the checksum header (first 4 bytes) through a local,
//       non-owning `CDataStore` view.
//    3. CRC32 the remaining `declaredLen - 4` bytes and compare against
//       the header - reject (`return 1`) on mismatch.
//    4. Find-or-create this player's own record in a global table
//       (`g_actionQueueTable`, `stru_6FACFF00` in the dump) keyed by
//       `self->0x40`.
//    5. Under that record's own lock, unless the record's state (`+0xE4`)
//       or a couple of flag bits on `self->0x80` say otherwise: reserve
//       room in the record's action queue (`AllocateActionQueueSlot`),
//       write a 1-byte player id (`self->0xB4`) + the block length + the
//       raw, still-undecoded action bytes into it via a growable write
//       buffer (`CDataStoreScratch`), and track this session's own running
//       queued-byte total (`self->0x6C`) against a 4KB cap.
//    6. Notify a global condition (`g_actionQueueSignal`) if a new record
//       was just created.
//
//  Field/global names below are addresses this session did not further
//  identify (`self`'s own class, the record's own class, what `self+0x40`,
//  `self+0x80`'s two tested bits, `self+0xB4`, `self+0x6C`, or the
//  record's `+0xE4`/`+0x1C8` actually are) - kept as raw offset casts per
//  CLAUDE.md's own rule against inventing names for fields the call tree
//  does not otherwise establish.
//============================================================================
#include <windows.h>
#include "cdatastore.h"
#include "cdatastorescratch.h"

//============================================================================
//  DEBUG ONLY - not part of the reconstruction. Set to 0 (or delete the
//  AH26_LOG lines) before judging this function's match: the calls are real
//  code and will show up as extra instructions against the shipped body.
//
//  Every exit is logged, not just the interesting one. Silence from a log
//  placed *after* the guards cannot distinguish "the handler never ran" from
//  "it ran and returned early" - and this function has four early returns
//  before it does anything observable.
//
//  Read with DebugView (Capture Win32 + Capture Global Win32), or from
//  tools/run_war3.py's result["debug_strings"] when running under the
//  harness - a debugger owns OutputDebugString, so while war3 runs under it
//  DebugView shows nothing.
//============================================================================
#define GAMEACTION_0x26_DEBUG_LOG 1

#if GAMEACTION_0x26_DEBUG_LOG
#include <stdio.h>
#include <stdarg.h>
#pragma warning(push)
#pragma warning(disable: 4996)   //  _vsnprintf "deprecated" - VC8's own
                                 //  replacement is _vsnprintf_s, and this is
                                 //  debug scaffolding, not shipped code
static void AH26_LOG(const char* fmt, ...)
{
    //  _vsnprintf from libcmt.lib and OutputDebugStringA from Kernel32.Lib -
    //  both already linked by tools/build_mix.py. Deliberately NOT
    //  wsprintfA/wvsprintfA: those live in user32, which the injection DLL
    //  does not link, and the link fails with LNK2019 __imp__wvsprintfA@12.
    char line[192];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(line, sizeof(line) - 1, fmt, ap);
    va_end(ap);
    line[sizeof(line) - 1] = '\0';
    OutputDebugStringA(line);
}
#pragma warning(pop)
#else
static void AH26_LOG(const char*, ...) { }
#endif

unsigned int __fastcall Crc32(const void* data, unsigned int length);
CDataStore* __fastcall ReadHeaderDword(CDataStore* self, unsigned int* out);
struct ConditionVariable
{
    void SignalOne(unsigned int value);   // sub_6F6D8950, gameaction0x26_notify.cpp
};
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  stru_6FACFF00 - a global table (lock + bucket array, the same shape
//  several other TSHashTable-style structures in this codebase already
//  established) mapping `self->0x40` to a per-player action-queue record.
//  Not reconstructed - only the one method this handler calls.
struct ActionQueueTable
{
    void* FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                              unsigned int arg4, unsigned int createIfMissing);
};
extern ActionQueueTable g_actionQueueTable;   // stru_6FACFF00

//  The per-player action-queue record's own "reserve N bytes" allocator
//  (sub_6F670E90) - this session did not trace its own body (a real
//  allocator with its own lock/event-wait machinery, not a leaf). Returns
//  a queue-slot descriptor whose own `+0xC` (write position) and `+0x10`
//  (buffer base) fields the handler advances directly.
struct ActionQueueRecord
{
    void* __fastcall AllocateSlot(unsigned int size, unsigned int sequence);

    char m_reserved00[0xE4];
    int  m_state;              // +0xE4
    char m_reservedE8[0x188 - 0xE4 - 4];
    char m_lock[0x1C8 - 0x188];  // +0x188, a CRITICAL_SECTION this handler locks directly (EnterCritSec/LeaveCritSec take its address) - exact size unknown, sized to reach the next touched field
    //  +0x1C8 - other functions in this same call tree (sub_6F66BEF0's own
    //  body, elsewhere in this dump) prove this is a nullable POINTER to a
    //  heap sub-object, not a boolean: it's null-checked, dereferenced at
    //  +0xC8/+0xD0, and SMemFree'd (tag "delete") when cleared back to 0 -
    //  a registered consumer/target of some kind, not a "flush pending"
    //  flag. This handler only ever reads it as `!= 0`, which is
    //  byte-identical whether the field is typed `int` or a pointer, so the
    //  wrong type here was never a functional bug - just a wrong name.
    //  Kept as `void*` and read as a boolean the same way the shipped code
    //  does, without inventing what the sub-object itself actually is.
    void* m_consumerTarget;     // +0x1C8
};

extern "C" unsigned int g_actionQueueSignal;   // unk_6FACFF20, the ConditionVariable this handler signals

//  DEBUG ONLY - not part of the reconstruction. A tiny watch buffer instead
//  of OutputDebugStringA: that call always raises exception 0x40010006
//  (DBG_PRINTEXCEPTION_C) as its debugger-handshake mechanism, which a
//  trace/monitor tool that breaks on first-chance exceptions reads as a
//  crash even though nothing is actually wrong - so just write the byte
//  somewhere a debugger can watch it directly, no calls involved.
extern "C" volatile unsigned char g_lastGameActionId = 0;
extern "C" volatile unsigned int  g_lastGameActionCount = 0;

int __stdcall GameAction_0x26_PacketHandler(void* self, unsigned int Src, unsigned int declaredLength)
{
    void* src = (void*)Src;

    //  First statement in the function, before every guard: this line firing
    //  is the proof the hook is on the live path at all.
    AH26_LOG("GameAction_0x26: ENTRY self=%08X src=%08X declaredLen=%u\n",
             self, Src, declaredLength);

    if (*(int*)((char*)self + 0x40) == 0)
    {
        AH26_LOG("GameAction_0x26: reject - self+0x40 is null (ret 1)\n");
        return 1;
    }

    unsigned int payloadLength = declaredLength - 4;
    if (payloadLength > 0x400)
    {
        //  Also catches declaredLength < 4, which underflows to a huge value.
        AH26_LOG("GameAction_0x26: reject - payloadLen=%u out of range "
                 "(declaredLen=%u) (ret 1)\n", payloadLength, declaredLength);
        return 1;
    }

    unsigned int checksum;
    {
        CDataStore local;
        local.m_field4 = src;
        local.m_field8 = 0;
        local.m_field0xC = -1;
        local.m_field10 = declaredLength;
        local.m_readPos = 0;
        ReadHeaderDword(&local, &checksum);
        if ((unsigned int)local.m_readPos > (unsigned int)local.m_field10)
        {
            AH26_LOG("GameAction_0x26: reject - read pos %u past end %u (ret 1)\n",
                     local.m_readPos, local.m_field10);
            return 1;
        }
    }

    const void* payload = (const char*)src + 4;
    {
        unsigned int computed = Crc32(payload, payloadLength);
        //  payload[0] is the first byte of the raw, still-serialized action
        //  data - the real GameActionId of the first queued action
        //  (e.g. 0x13 = DropOrGiveItem).
        AH26_LOG("GameAction_0x26: crc computed=%08X expected=%08X actionId=%02X\n",
                 computed, checksum,
                 payloadLength ? ((const unsigned char*)payload)[0] : 0);
        if (computed != checksum)
        {
            AH26_LOG("GameAction_0x26: reject - crc mismatch (ret 1)\n");
            return 1;
        }
    }

    //  DEBUG ONLY - not part of the reconstruction. payload[0] is the first
    //  byte of the raw, still-serialized action data (this handler queues it
    //  verbatim, see the file header) - i.e. the real GameActionId of the
    //  first queued action (e.g. 0x13 = DropOrGiveItem). Set a breakpoint on
    //  this write, or just watch g_lastGameActionId's address in memory -
    //  g_lastGameActionCount increments on every call so you can tell two
    //  packets with the same id apart.
    g_lastGameActionId = payloadLength ? ((const unsigned char*)payload)[0] : 0;
    g_lastGameActionCount++;

    void* sessionKey = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    ActionQueueRecord* record = (ActionQueueRecord*)
        g_actionQueueTable.FindOrCreateRecord(sessionKey, 0, &wasCreated, 0, 1);

    int status;
    if (!record)
    {
        status = 0;
    }
    else
    {
        status = 0;
        unsigned int selfFlags = *(unsigned int*)((char*)self + 0x80);
        if (selfFlags & 0x400000)
        {
            status = 2;
        }
        else if (record->m_state == 7)
        {
            status = 2;
        }
        else if (record->m_state == 6)
        {
            if (!(selfFlags & 0x80000))
            {
                //  6F671171: `mov [esp+68h+arg_8], 2` - the shipped code sets
                //  the result to 2 *before* taking the lock, so the ordinary
                //  "action queued" path returns 2, not 0. This was missing:
                //  the queueing path returned 0, which the caller reads as
                //  something quite different (0 is otherwise only reached
                //  when the record is absent, or when the 4KB cap below is
                //  hit) - so a successful queue looked like a failure and the
                //  game dropped out on the first action.
                status = 2;
                EnterCritSec((CRITICAL_SECTION*)&record->m_lock);

                unsigned int sequence = *(unsigned int*)((char*)self + 0x60) + 1;
                unsigned int slotSize = payloadLength + 3;
                // AllocateSlot's own return value: an opaque queue-slot
                // descriptor, +0xC a pointer this handler advances after
                // writing and +0x10 an integer offset from it - a
                // different object from `record` (own field meanings),
                // kept as a raw pointer rather than reusing that struct.
                void* slot = record->AllocateSlot(slotSize, sequence);

                void* writeDest = (char*)*(void**)((char*)slot + 0xC) + *(int*)((char*)slot + 0x10) + 0x14;
                CDataStoreScratch writer(writeDest, (void*)(unsigned int)slotSize);

                unsigned char playerId = *((unsigned char*)self + 0xB4);
                writer.WriteByte(playerId);
                writer.WriteWord((unsigned short)payloadLength);
                writer.WriteRaw(payload, payloadLength);

                *(unsigned int*)((char*)slot + 0xC) += slotSize;

                if (record->m_consumerTarget != 0)
                {
                    unsigned int* runningTotal = (unsigned int*)((char*)self + 0x6C);
                    *runningTotal += payloadLength;
                    //  6F671206: knocks the 2 above back down to 0 once this
                    //  session has queued 4KB. With the 2 missing this line
                    //  was a no-op (0 -> 0) and the cap signalled nothing.
                    if (*runningTotal >= 0x1000)
                        status = 0;
                }

                LeaveCritSec((CRITICAL_SECTION*)&record->m_lock);
            }
            // else: status stays 0, no queueing (matches the shipped code's own empty branch)
        }
        else
        {
            status = 1;
        }

        if (wasCreated != -1)
            ((ConditionVariable*)&g_actionQueueSignal)->SignalOne(wasCreated != 0);
    }

    //  status: 2 = queued (or deliberately skipped), 1 = record in an
    //  unexpected state, 0 = no record, or this session hit the 4KB cap.
    AH26_LOG("GameAction_0x26: EXIT status=%d state=%d wasCreated=%d\n",
             status, record ? record->m_state : -1, wasCreated);
    return status;
}
