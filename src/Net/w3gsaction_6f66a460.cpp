//============================================================================
//  0x6F66A460 - the real body behind Method_6F66A460
//  (w3gsaction_6f66c500.cpp's own case-44 handler and its only caller in
//  this call tree).  Deliberately its own translation unit: a local
//  CDataStoreCache1460 with a real destructor is enough to force this
//  build's own unified-EH frame, and CLAUDE.md's house rule prefers a
//  second TU over `__declspec(noinline)` to keep the shipped `call` real
//  rather than risk /Ob2 folding it into its one caller.
//
//  Builds a small header-only packet in a stack CDataStoreCache1460 -
//  fixed bytes 0xF7/0x17, a 2-byte length placeholder, the message body
//  through NetMsgIoNoFields_6F683B70 (a no-op: this message carries no
//  fields), then patches the placeholder with the real final length
//  (WriteWordAt) - and dispatches the finished buffer through
//  DispatchVtableSlot11 (self=`this`, buffer pointer + length read back via
//  QueryBufferState).  Returns the final buffer length in eax, even though
//  its own (and only) caller discards it.
//
//  Same unreproducible `__except_handler4`/`__CxxFrameHandler3`-shaped
//  unified-EH frame this repo's fixed /GS- /EHs-c- cannot produce
//  (docs/msvc-vc8-idioms.md) - the frame furniture never matches, the body
//  does.  IDENTICAL, not DIFFERS.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

//  0x6F683B70 - already reconstructed (netmsgio_683_empty.cpp): the
//  message struct has no serialised fields, so the whole body is a fluent
//  `return this`.
extern CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B70(CDataStoreScratch* self,
                                                                const void* src);

//  0x6F6DAE20 - already reconstructed (cdatastorescratch_writers.cpp) as
//  DispatchVtableSlot11; re-declared identically here for extern linkage.
extern void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

unsigned int __fastcall Method_6F66A460_Real(void* self, void* payload)
{
    CDataStoreCache1460 store;
    CDataStoreScratch* scratch = (CDataStoreScratch*)&store;

    scratch->WriteByte(0xF7);
    scratch->WriteByte(0x17);
    unsigned int lengthFieldPos = (unsigned int)store.m_field10;
    scratch->WriteWord(0);

    NetMsgIoNoFields_6F683B70(scratch, payload);

    unsigned int finalPos = (unsigned int)store.m_field10;
    scratch->WriteWordAt(lengthFieldPos, (unsigned short)finalPos);

    void* bufferPtr = 0;
    int bufferLen = 0;
    scratch->QueryBufferState(&bufferPtr, &bufferLen, 0);

    DispatchVtableSlot11(self, (int)bufferPtr, (unsigned int)bufferLen);

    return (unsigned int)bufferLen;
}
