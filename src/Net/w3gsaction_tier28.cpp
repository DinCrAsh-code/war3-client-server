//============================================================================
//  batch-H-w3gsaction-and-replaystream - 0x6F66EF60 and 0x6F66F050, two
//  near-identical single-target "find one node and forward it a framed
//  record" handlers - the same ActionQueueTable::FindOrCreateRecord/
//  self+0x80 flag family already established, but reporting status 0
//  (not 2) on the record's own +0x80 bit 0 already being latched, and
//  finding at most ONE matching node on record->field0xD4's own list
//  (matched against the field reader's own first byte) to forward a
//  single framed record to via DispatchVtableSlot11 - unlike
//  w3gsaction_tier26.cpp's own per-name-byte multi-target search or
//  w3gsaction_tier27.cpp's own broadcast-to-everyone.
//
//  Each has its own field reader (ParseByteByteDwordDwordAction_6F666B90 /
//  ParseByteByteDwordAction_6F666C40, both in w3gsaction_parsers.cpp) and
//  its own framing callee (sub_6F66A9E0/sub_6F66AB60, tags 0x44/0x45,
//  reusing netclient_notifyevent_0x1f.cpp's own CDataStoreCache1460/
//  CDataStoreScratch shape once again) - each callee's own payload writer
//  (sub_6F6849E0/sub_6F684A60) forwards the field reader's own output
//  struct verbatim, byte for byte, with no reordering.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

//  0x6F666B90 / 0x6F666C40 - already reconstructed for real
//  (w3gsaction_parsers.cpp).
extern int __fastcall ParseByteByteDwordDwordAction_6F666B90(void* src, unsigned int declaredLength, unsigned char* out);
extern int __fastcall ParseByteByteDwordAction_6F666C40(void* src, unsigned int declaredLength, unsigned char* out);

//  0x6F668130 - already reconstructed for real (w3gsaction_tier10.cpp).
struct ListWalker
{
    void* Step(void* prev);
};

//  0x6F6DAE20 - already reconstructed for real elsewhere in this tree;
//  re-declared identically here for extern linkage.
extern void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  Same ActionQueueTable/ConditionVariable infrastructure
//  w3gsaction_tier4.cpp and friends already establish.
struct ConditionVariable
{
    void SignalOne(unsigned int value);   // sub_6F6D8950
};
extern "C" unsigned int g_actionQueueSignal;   // unk_6FACFF20

struct ActionQueueTable
{
    void* FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                              unsigned int arg4, unsigned int createIfMissing);
};
extern ActionQueueTable g_actionQueueTable;   // stru_6FACFF00

static void g_actionQueueTable_Signal(int wasCreated)
{
    ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)(wasCreated != 0));
}

//  Redeclared exactly as netclient_notifyevent_0x1f.cpp and friends declare
//  it - the real constructor/destructor bodies live in cdatastorecache.cpp.
class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

//----------------------------------------------------------------------------
//  0x6F66EF60 family - ParseByteByteDwordDwordAction_6F666B90's own output
//  (byte, byte, dword, dword - 10 bytes), forwarded to its match verbatim.
//----------------------------------------------------------------------------
struct ByteByteDwordDword
{
    unsigned char field0;   // +0x0 - the target id searched for on the list
    unsigned char field1;   // +0x1 - checked against self+0xB4 up front
    unsigned int  field2;   // +0x2
    unsigned int  field6;   // +0x6
};

//  0x6F6849E0 - fastcall(this=writer, edx=&ByteByteDwordDword). Forwards
//  the parsed struct's own four fields verbatim, in order.
void __fastcall WriteByteByteDwordDwordPayload(CDataStoreScratch* writer, ByteByteDwordDword* fields)
{
    writer->WriteByte(fields->field0);
    writer->WriteByte(fields->field1);
    writer->WriteDword(fields->field2);
    writer->WriteDword(fields->field6);
}

//  0x6F66A9E0 - fastcall(this=target, edx=&ByteByteDwordDword). Frames
//  {0xF7, 0x44, fields} through a local CDataStoreCache1460 the same way
//  netclient_notifyevent_0x1f.cpp's own family does, and dispatches it to
//  the one target given (not a list walk - the caller already found the
//  one matching node). Returns the framed buffer's own length.
int __fastcall SendFramedRecord_0x44(void* target, ByteByteDwordDword* fields)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x44);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WriteByteByteDwordDwordPayload(writer, fields);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F66EF60 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction_6F66EF60(void* self, unsigned int arg4, unsigned int arg8)
{
    ByteByteDwordDword fields;
    if (!ParseByteByteDwordDwordAction_6F666B90((void*)arg4, arg8, (unsigned char*)&fields))
        return 1;

    if (fields.field1 != *(unsigned char*)((char*)self + 0xB4))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    void* record = g_actionQueueTable.FindOrCreateRecord(key, 1, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        unsigned int selfFlags = *(unsigned int*)((char*)self + 0x80);
        if (selfFlags & 0x400000)
        {
            status = 2;
        }
        else if (!(selfFlags & 1))
        {
            int node = *(int*)((char*)record + 0xD4);
            node = (node > 0) ? node : 0;
            if (node > 0)
            {
                do
                {
                    if (*(unsigned char*)((char*)node + 0xB4) == fields.field0)
                    {
                        void* target = *(void**)((char*)node + 0x3C);
                        SendFramedRecord_0x44(target, &fields);
                        break;
                    }
                    node = (int)((ListWalker*)((char*)record + 0xCC))->Step((void*)node);
                } while (node > 0);
            }
            status = 2;
        }

        if (wasCreated != -1)
            g_actionQueueTable_Signal(wasCreated);
    }
    return status;
}

//----------------------------------------------------------------------------
//  0x6F66F050 family - ParseByteByteDwordAction_6F666C40's own output
//  (byte, byte, dword - 6 bytes), same shape one field narrower.
//----------------------------------------------------------------------------
struct ByteByteDword
{
    unsigned char field0;   // +0x0 - the target id searched for on the list
    unsigned char field1;   // +0x1 - checked against self+0xB4 up front
    unsigned int  field2;   // +0x2
};

//  0x6F684A60 - same shape as WriteByteByteDwordDwordPayload above, one
//  field narrower.
void __fastcall WriteByteByteDwordPayload(CDataStoreScratch* writer, ByteByteDword* fields)
{
    writer->WriteByte(fields->field0);
    writer->WriteByte(fields->field1);
    writer->WriteDword(fields->field2);
}

//  0x6F66AB60 - same shape as SendFramedRecord_0x44 above, tag 0x45.
int __fastcall SendFramedRecord_0x45(void* target, ByteByteDword* fields)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x45);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WriteByteByteDwordPayload(writer, fields);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F66F050 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction_6F66F050(void* self, unsigned int arg4, unsigned int arg8)
{
    ByteByteDword fields;
    if (!ParseByteByteDwordAction_6F666C40((void*)arg4, arg8, (unsigned char*)&fields))
        return 1;

    if (fields.field1 != *(unsigned char*)((char*)self + 0xB4))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    void* record = g_actionQueueTable.FindOrCreateRecord(key, 1, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        unsigned int selfFlags = *(unsigned int*)((char*)self + 0x80);
        if (selfFlags & 0x400000)
        {
            status = 2;
        }
        else if (!(selfFlags & 1))
        {
            int node = *(int*)((char*)record + 0xD4);
            node = (node > 0) ? node : 0;
            if (node > 0)
            {
                do
                {
                    if (*(unsigned char*)((char*)node + 0xB4) == fields.field0)
                    {
                        void* target = *(void**)((char*)node + 0x3C);
                        SendFramedRecord_0x45(target, &fields);
                        break;
                    }
                    node = (int)((ListWalker*)((char*)record + 0xCC))->Step((void*)node);
                } while (node > 0);
            }
            status = 2;
        }

        if (wasCreated != -1)
            g_actionQueueTable_Signal(wasCreated);
    }
    return status;
}
