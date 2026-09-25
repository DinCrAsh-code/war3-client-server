//============================================================================
//  sub_6F6690F0/sub_6F6691F0 - two SEH-protected packet builders
//  (0x6F669xxx neighbourhood), reached from ServerTock_0x27_PacketHandler's
//  own sub_6F66BEF0 callee. Each constructs a local CDataStoreCache1460 and
//  uses it as a CDataStoreScratch-compatible write buffer - its first five
//  fields are laid out identically to CDataStoreScratch's own, and the tiny
//  writes here never approach the object's 1460-byte inline capacity, so
//  its unreconstructed default Grow() (inherited from CDataStore) never
//  actually runs - to build a fixed {0xF7, tag} header, a two-byte
//  placeholder length, one relay-chunk callee's own output, and patches the
//  placeholder with the real ending position via WriteWordAt before
//  dispatching (BuildRelay0: once, to `self` directly; BuildRelay1: once
//  per live player, walking the same index-linked-list convention
//  sub_6F66BEF0's own body uses - base=self->0, each node's own "next"
//  index at node+4, tested `> 0` to continue, this list's own dispatch
//  target at node+0x3C - accumulating the per-dispatch length into the
//  return value).
//
//  Same unreproducible `__except_handler4`-shaped SEH frame
//  GameAction_0x26_PacketHandler's own entry documents (a local object with
//  a real destructor) - this session accepts the same score/verdict
//  tradeoff that entry already establishes for this exact shape.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteHeaderFromWrapper(CDataStoreScratch* self, const unsigned char* src);
CDataStoreScratch* __fastcall WritePacketFrom(CDataStoreScratch* self, const unsigned char* src);
void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

//----------------------------------------------------------------------------
//  0x6F6690F0 - header tag {0xF7, 0x0E}, one WritePacketFrom chunk, single
//  dispatch straight to `self`.
//----------------------------------------------------------------------------
int __fastcall BuildRelay0(void* self, void* arg)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x0E);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WritePacketFrom(writer, (const unsigned char*)arg);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(self, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//----------------------------------------------------------------------------
//  0x6F6691F0 - header tag {0xF7, 0x11}, one WriteHeaderFromWrapper chunk,
//  dispatched once per live player in self's own index-linked list.
//----------------------------------------------------------------------------
int __fastcall BuildRelay1(void* self, void* arg)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x11);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WriteHeaderFromWrapper(writer, (const unsigned char*)arg);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    int totalSent = 0;
    int index = *(int*)((char*)self + 8);
    while (index > 0)
    {
        char* node = (char*)*(void**)self + index;
        void* dispatchTarget = *(void**)(node + 0x3C);
        if (dispatchTarget != 0)
        {
            DispatchVtableSlot11(dispatchTarget, (int)bufferBase, (unsigned int)bufferLen);
            totalSent += bufferLen;
        }
        index = *(int*)(node + 4);
    }

    return totalSent;
}
