//============================================================================
//  Net::NetProviderLOOP::ClearShutdownPending - 0x6F660EA0, direct callee of
//  both Shutdown (slot 2) and NotifyStateChange (slot 3).  Its own
//  translation unit, and that is the whole point of the file: the shipped
//  code makes a real `call sub_6F660EA0` from both callers rather than
//  inlining it, and this build's /Ob2 will happily inline a short function
//  that shares a TU with its caller - the same same-TU inlining trap
//  docs/msvc-vc8-idioms.md's deleting-destructor note documents (and the
//  Net::NetProvider base sweep hit for real, in ShrinkFreePool/
//  ReleasePayload).  Split apart, both callers keep the real call.
//============================================================================
#include "netproviderloop.h"

//  unreconstructed_thunks.cpp - sub_6F65AB10 (2-byte status write).
class PostStatusTarget
{
public:
    void PostProviderStatusWord(int);
};

namespace Net
{

void NetProviderLOOP::ClearShutdownPending()
{
    if (m_shutdownPending)
    {
        m_shutdownPending = 0;
        ((PostStatusTarget*)this)->PostProviderStatusWord(0xC);
    }
}

}  // namespace Net
