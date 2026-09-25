//============================================================================
//  The per-thread network client at thread-local slot 0x0E, and the two
//  things this closure does with it.
//
//  Slot 0x0E is a second context beside the game-data context at 0x0D that
//  every data lookup in this binary uses (Game/gamecontext.h): three
//  functions in the 0x6F650xxx module fetch it, and all three then dispatch
//  through one of its vtable slots.  Nothing here names the class - the only
//  evidence is the three slots - so the object is modelled by its vtable and
//  the one field 0x6F657340 reads.
//============================================================================
#ifndef NETCLIENT_H
#define NETCLIENT_H

//  Storm ordinal 465 - set the thread's last error.  Declared exactly as
//  Storm/storm.h's own family is (ordinary C++, not extern "C"), so MASM
//  writes the readable name beside the EXTRN and the call resolves.
void __stdcall SErrSetLastError(unsigned int error);

//  What Storm's SErrSetLastError is handed on every one of the argument
//  checks in this module: ERROR_INVALID_PARAMETER.
const unsigned int kErrorInvalidParameter = 0x57;

class CNetClient
{
public:
    //  0x6F657340 - hand a packet to whatever sits at +0x2B4, through that
    //  object's own vtable slot 0.  A tail jump, so the two words the
    //  caller pushed are cleaned by the slot: `retn 8`.
    void SendPacket(void* data, unsigned int size);

    //  0x6F657350 - the same shape as SendPacket, one slot further along
    //  (0x44/4 = 17) on the same sink object - CTurnsSyncStore::Flush's own
    //  caller (netturnsyncstore.cpp).  Purpose past "another two-word
    //  message the sink forwards" is not established - the sink's own
    //  dynamic type is unidentified either way, same as slot 0's.
    void Method_0x44(void* data, unsigned int size);

    char  m_reserved000[0x2B4];
    void* m_pPacketSink;       // +0x2B4
};

//  0x6F4C34D0, under the name Game/gamecontext.h already binds to it.
void* __fastcall GetThreadLocalSlot(unsigned int slot);

//  Slot 0x0E of this thread's pointer array.
const unsigned int kThreadLocalNetClient = 0x0E;

//----------------------------------------------------------------------------
//  The send queue 0x6F650480 drains.  Its own class is not named by anything
//  in this call tree; the one thing established about it is that vtable slot
//  9 hands back a {buffer, size} pair, so that is all this declares.
//----------------------------------------------------------------------------
class CNetSendQueue
{
public:
    //  0x6F650480 - `retn 0`, `this` in ecx and no stack arguments.
    void Flush();
};

//  0x6F6501F0 - post one three-word event to the client's vtable slot
//  0x90.  `retn 4`.
void __fastcall PostNetClientEvent(unsigned int eventId, int arg1, int arg2);

//  0x6F6505C0 - hand the client's vtable slot 0x84 a validated array of
//  twenty-byte game results.  `retn 0Ch`.
void __fastcall SubmitGameResults(unsigned int count, void* results,
                                  const char* nameA, const char* nameB,
                                  void* summary);

//----------------------------------------------------------------------------
//  Net::CTurnsSyncStore (`??_7CTurnsSyncStore@Net@@6B@`, 0x6F9586A0 -
//  docs/targets/vtables/CTurnsSyncStore.txt) - a CDataStoreCache1460-based
//  growable record queue (netturnsyncstore.cpp), built as a local by
//  CNetData::DispatchQueuedTurnRecord (netcommand_dispatch.cpp) to
//  accumulate one applied-delta record and flush it to the network.
//
//  Only two of its 11 real vtable slots are used by anything this repo has
//  reached: slot 3 (Grow, sub_6F2C9450) and slot 9 (GetBuffer, sub_6F4C1BB0
//  - the exact address Net::CTurnStore's own vtable shares at the same
//  slot, and the same {buffer,size}-out-param shape CNetSendQueue::Flush
//  above already uses, strongly suggesting both are the same underlying
//  queue template). The other 9 are real, dumped, unreconstructed
//  addresses (docs/targets/vtables/CTurnsSyncStore.txt has every one) -
//  not modelled as C++ `virtual`s here, because a constructor may stamp
//  our own vtable only for a class whose vtable is *finished*
//  (CLAUDE.md/ctor_vtable_audit.py); with 9 slots still real shipped
//  addresses this repo has never reconstructed, the constructor keeps
//  storing the real shipped vtable address (0x6F9586A0) directly instead -
//  every hand-written dispatch below still reaches the real, unhooked
//  slot 3/9 bodies through it, exactly as the shipped code does.
//
//  Field layout: CDataStore's own append-buffer shape (m_field4/8/0xC/10 -
//  cdatastore.h) plus CDataStoreCache1460's 1460-byte inline buffer
//  (cdatastorecache.cpp) - the base subobject CTurnsSyncStore's own
//  constructor (0x6F543E50) builds through the already-reconstructed
//  0x6F2C9290 before re-stamping the vtable.  No members of its own beyond
//  that: its destructor is exactly CDataStoreCache1460's own
//  (netcommand_dispatch.cpp's own CNetData_AppliedDeltaReporterDtor).
//----------------------------------------------------------------------------
const void* const kTurnsSyncStoreVftable = (const void*)0x6F9586A0;

struct CTurnsSyncStore
{
    void*         m_vtable;             // +0x00
    void*         m_field4;             // +0x04
    int           m_field8;             // +0x08
    int           m_field0xC;           // +0x0C
    int           m_count;              // +0x10 - CDataStore's own m_field10
    int           m_readPos;            // +0x14
    char          m_inlineBuffer[1460]; // +0x18

    //  0x6F652080 - reserve room for one more 5-byte record, growing the
    //  backing buffer through vtable slot 3 first if the current
    //  [m_field8, m_field8+m_field0xC) window can't already fit it.
    void ReserveRecord();

    //  0x6F6504F0 - drain the queue (vtable slot 9) and forward what's left
    //  after the record's own 5-byte header to the thread's network client,
    //  the same shape CNetSendQueue::Flush (netclientevent.cpp) already
    //  uses with an 8-byte header and a 0x400 bound instead of 5/0x60.
    void Flush();
};

#endif
