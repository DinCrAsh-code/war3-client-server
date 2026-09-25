//============================================================================
//  0x6F537AA0 - SerializeExtraHeader, CNetEventTypeDispatcher::
//  ConstructAndBroadcast's own first jump table's shared tail: writes extra
//  header fields into `self`'s own embedded CDataStoreScratch (`self+0xBE8`,
//  the observer stream) through its vtable slot 9 (offset 0x24), then -
//  only for msg types 0x1E/0x1F (dispatcher decimal 30/31, see the two
//  `cmp edi,...` in the dump) - serializes `dataPtr` too through
//  sub_6F4C2160/sub_6F4C2210 (already reconstructed, CDataStoreScratch's
//  own WriteByte/WriteWord), then always writes msgType and dataPtr through
//  sub_6F4C25A0 (WriteRaw, chunked).  retn 8.
//
//  Its own translation unit, split out of netevent_dispatch.cpp: it is
//  called from six sites in that file (five inside ConstructAndBroadcast,
//  plus AppendRouteCandidateRecord_6F543A40/6F543B00/6F543BC0/6F543C80's
//  own four bodies) and every one of the shipped call sites is a real
//  `call sub_6F537AA0`, never inlined - keeping this function in the same
//  TU as its small AppendRouteCandidateRecord_* callers let this build's
//  /Ob2 inline it into all four of them (a small function, cheap to fold
//  into an equally small caller), producing an entirely different
//  instruction shape at each site. Same reasoning as CLAUDE.md's own "one
//  translation unit per original module" rule.
//============================================================================
#include "cdatastorescratch.h"

void __fastcall SerializeExtraHeader_6F537AA0(void* self, void* /*unused_edx*/,
                                               unsigned int msgType, void* dataPtr)
{
    typedef void (__thiscall *HeaderFieldWriteFn)(void*, void*, void*);
    void* observerStream = (char*)self + 0xBE8;
    HeaderFieldWriteFn writeHeader =
        *(HeaderFieldWriteFn*)(*(char**)observerStream + 0x24);
    writeHeader(observerStream, &self, &msgType);

    if (msgType == 0x1E || msgType == 0x1F)
    {
        CDataStoreScratch* scratch = (CDataStoreScratch*)observerStream;
        scratch->WriteByte((unsigned char)msgType);
    }

    CDataStoreScratch* scratch = (CDataStoreScratch*)observerStream;
    scratch->WriteRaw(dataPtr, msgType);
}
