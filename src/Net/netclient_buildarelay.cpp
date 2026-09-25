//============================================================================
//  0x6F677870 - NetClientNotifySelf::PostEvent_0x25's own family shape
//  (waves 5-7's own {0xF7,tag}-then-DispatchVtableSlot11 framing), but for
//  Net::NetClient vtable slot 4's own root (0x6F67A170, networking wave 8),
//  which builds a slightly different frame: {0xF7, ':'} rather than the
//  notify family's {0xF7, tag}, and calls a differing "differs per member"
//  callback (0x6F6850F0, not reconstructed by this wave - see below) rather
//  than a fixed no-op.
//
//  Local write buffer, tag bytes, length-placeholder capture, and final
//  QueryBufferState -> DispatchVtableSlot11 dispatch all reuse the exact
//  same shapes and real definitions the notify family already established
//  (netclient_notifyevent_0x1f.cpp's own CDataStoreCache1460-cast-to-
//  CDataStoreScratch* trick; netclient_pendingflush.cpp's own
//  DispatchVtableSlot11 declaration) - nothing new invented here, only
//  reused.
//
//  A real __except_handler4-shaped SEH frame (local object with a
//  non-trivial destructor, this build's fixed /GS- /EHs-c- cannot
//  reproduce - docs/msvc-vc8-idioms.md's own entry for the shape) plus the
//  0x5D8-byte stack frame IDA's own listing carries account for the
//  register/offset noise around the reconstructed body; the calls
//  themselves, in order, with the same arguments, are the target this
//  session judges against.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

//  netclient_notifyevent_0x1f.cpp's own redeclaration - the real
//  constructor/destructor bodies live in cdatastorecache.cpp, and every
//  TU that needs this class redeclares it locally the same way (see that
//  file's own header comment for why a shared header isn't used yet).
class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

//  netclient_pendingflush.cpp - the shared vtable-slot-11 send dispatcher
//  every framed-record builder in this neighbourhood already reuses.
void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  0x6F650EC0 - per-record field serializer for the record array this
//  callback (below) walks; called once per relay record with `ecx` = the
//  write buffer and `edx` = the record pointer. No dump reaches this
//  address this session (the batch-E worklist scopes the file this
//  callback lives in, not this callee) - a thunk to the real, unhooked
//  body rather than a guess at what it writes.
void __fastcall SerializeRelayRecord_6F650EC0(void* buffer, void* record);
__declspec(naked) void __fastcall SerializeRelayRecord_6F650EC0(void*, void*)
{
    __asm { mov eax, 0x6F650EC0 }
    __asm { jmp eax }
}

//  The payload NetClientBuildARelay_Callback (below) reads: a byte record
//  count, an array of 0x14-byte records (each record's own layout unknown -
//  handed whole to SerializeRelayRecord_6F650EC0 above), then a
//  byte-counted trailing raw blob. Offsets are exact; the gap at +0..+3 and
//  the 3-byte pad at +5..+7 are never read by this function, so they stay
//  unnamed reserved bytes rather than invented fields.
struct NetClientBuildARelayPayload
{
    char          m_reserved0[4];
    unsigned char m_recordCount;   // +0x04
    char          m_reserved5[3];
    void*         m_records;       // +0x08 - array, stride 0x14
    unsigned char m_trailingLen;   // +0x0C
    char          m_trailingData[1]; // +0x0D - m_trailingLen bytes follow
};

//  0x6F6850F0 - the "differs per member" callback this slot's own frame
//  calls between the length placeholder and the length patch-back, the
//  same call-site shape the notify family's own per-member callback sits
//  at (`ecx` = the write buffer, `edx` = passthrough - a free __fastcall
//  function, not a member, since the family's own real member calls
//  never use `edx` for a second argument the way this one does). Writes
//  the record count, then each record via SerializeRelayRecord_6F650EC0,
//  then a byte-counted trailing raw blob - reconstructed from the
//  disassembly's own call shapes (WriteByte/WriteByte/TailCallWriteRaw),
//  not from knowing what any record itself contains.
CDataStoreScratch* __fastcall NetClientBuildARelay_Callback(void* buffer, void* passthrough)
{
    CDataStoreScratch* writer = (CDataStoreScratch*)buffer;
    NetClientBuildARelayPayload* payload = (NetClientBuildARelayPayload*)passthrough;

    writer->WriteByte(payload->m_recordCount);

    unsigned int i = 0;
    if (payload->m_recordCount != i)
    {
        unsigned int offset = 0;
        do
        {
            SerializeRelayRecord_6F650EC0(writer, (char*)payload->m_records + offset);
            ++i;
            offset += 0x14;
        } while (i < payload->m_recordCount);
    }

    writer->WriteByte(payload->m_trailingLen);
    writer->TailCallWriteRaw(payload->m_trailingData, payload->m_trailingLen);
    return writer;
}

struct NetClientBuildARelaySelf
{
    void BuildAndSend(void* passthrough);
};

//  0x6F677870 - `this` (ecx) is the caller's own outer `this` (the target
//  of the final DispatchVtableSlot11 call, not touched until then);
//  `passthrough` (edx) is forwarded, unexamined, straight into the
//  callback above - the same "caller owns an address it never itself
//  dereferences" shape PostEvent_0x1F's own passthrough argument
//  documents.
void NetClientBuildARelaySelf::BuildAndSend(void* passthrough)
{
    void* self = this;

    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(':');
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    NetClientBuildARelay_Callback(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(self, (int)bufferBase, (unsigned int)bufferLen);
}
