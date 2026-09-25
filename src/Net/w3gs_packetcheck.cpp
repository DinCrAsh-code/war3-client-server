//============================================================================
//  ContainsW3GSBytePacketCheck - 0x6F6637C0. Registered as the listener
//  thread's own dispatch callback by NetProviderLTCP__Method_0xC
//  (netproviderltcp.cpp): this is where every event the LAN TCP listener
//  thread raises actually lands, and where a genuine W3GS packet (a leading
//  0xF7 magic byte) gets recognised and routed on to sub_6F663700
//  (w3gs_typedispatch.cpp), the per-message-type switch.
//
//  arg_8 selects one of four thread-lifecycle events (0..3); only cases 0
//  ("connection accepted", stores the new connection's AddRef'd handle),
//  1 (a byte of data arrived - the W3GS parse below), 2 ("connection
//  closed") and 3 ("thread starting") are handled; anything else falls
//  through to the shared epilogue untouched. Every path always returns 1
//  and writes the message id (arg_14) back through *arg_18 - this
//  reconstruction does not know what a caller does with either.
//============================================================================
#include "cdatastore.h"
#include "netconn.h"
#include "netproviderltcp.h"

using Net::NetProviderLTCP;

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  sub_6F65AAF0/sub_6F65AB10/sub_6F65AB50/sub_6F65CD70 - none reconstructed
//  this session (see netproviderltcp.cpp's own note on sub_6F65AAF0; the
//  other three are the same CDataStore-backed status/event write family at
//  1/2/4 bytes a call, guessed from the shared shape with sub_6F65AAF0
//  rather than confirmed). All four take `this`(ecx) implicitly from
//  whatever the caller already left there, plus at most one stack arg -
//  modelled the same way as PostStatusTarget elsewhere in this file.
struct PostStatusTarget
{
    void PostProviderStatus(int code);       // sub_6F65AAF0 (1-byte write)
    void PostProviderStatusWord(int code);   // sub_6F65AB10 (2-byte write)
    void PostProviderStatusDword(int code);  // sub_6F65AB50 (4-byte write)
    void ResetRecycleState();                // sub_6F65CD70 - no arguments
};

//  w3gs_typedispatch.cpp
void __stdcall W3GSDispatchByType(void* arg0, void* arg4, int msgType, void* data, int dataLen);   // sub_6F663700

int __stdcall ContainsW3GSBytePacketCheck(void* arg0, void* arg4, int selector, NetProviderLTCP* provider,
                                                      void* arg10, int msgId, int* pOutMsgId)
{
    switch (selector)
    {
    case 3:
        EnterCritSec(&provider->m_lock);
        provider->m_threadRunning = 0;
        ((PostStatusTarget*)provider)->PostProviderStatus(0xB);
        provider->m_threadDoneEvent.Set();
        LeaveCritSec(&provider->m_lock);
        break;

    case 0:
        EnterCritSec(&provider->m_lock);
        provider->m_threadRunning = 0;
        provider->m_listenHandle = ((OsNet::NETCONN*)arg0)->AddRef();
        ((PostStatusTarget*)provider)->PostProviderStatus(1);
        provider->m_threadDoneEvent.Set();
        LeaveCritSec(&provider->m_lock);
        break;

    case 2:
    {
        EnterCritSec(&provider->m_lock);
        ((OsNet::NETCONN*)provider->m_listenHandle)->Release();
        bool hadRecycle = (provider->m_recycleFlag != 0);
        provider->m_listenHandle = 0;
        if (hadRecycle)
        {
            provider->m_recycleFlag = 0;
            ((PostStatusTarget*)provider)->ResetRecycleState();
            ((PostStatusTarget*)provider)->PostProviderStatusDword(1);
        }
        ((PostStatusTarget*)provider)->PostProviderStatusWord(0xC);
        void* pending = provider->m_pendingSignalTarget;
        LeaveCritSec(&provider->m_lock);
        if (pending)
            ((Event*)pending)->Set();
        break;
    }

    case 1:
        if ((unsigned int)msgId >= 4)
        {
            void* buffer = arg10;
            CDataStore packet;
            packet.m_field4 = buffer;
            packet.m_field8 = 0;
            packet.m_field0xC = -1;
            packet.m_field10 = 4;
            packet.m_readPos = 0;

            unsigned char firstByte;
            packet.ReadByte(&firstByte);
            if (firstByte == 0xF7)
            {
                //  A single 8-byte local: the type byte at +0, the length
                //  word right after it at +1. The dump reads a whole dword
                //  starting at +1 (picking up two adjacent, otherwise-unused
                //  bytes along with the real length word) rather than
                //  widening the word it just read, and a second dword
                //  starting at +0 (type byte plus the length word) to hand
                //  the message type to W3GSDispatchByType, which only ever
                //  looks at its lowest byte - reproduced as-is rather than
                //  narrowed to what is actually read, on the same principle
                //  as "a pure computation... that cannot observe it" in
                //  CLAUDE.md's own behaviour-verdict guidance.
                unsigned char hdr[8];
                packet.ReadByte(&hdr[0]);
                packet.ReadWord((unsigned short*)&hdr[1]);

                //  The bounds check is against (length-4); the equality
                //  check just below it is against the *raw* length word -
                //  two different truncations of the same dword, matching
                //  the dump's own `lea ecx,[eax-4]` (non-destructive) then
                //  `movzx eax,ax` off the untouched original.
                unsigned int combined1 = *(unsigned int*)&hdr[1];
                unsigned short boundsCheck = (unsigned short)(combined1 - 4);
                if (boundsCheck <= 0xFFB)
                {
                    unsigned short rawLength16 = (unsigned short)combined1;
                    if ((unsigned int)msgId == rawLength16)
                    {
                        unsigned int msgTypeCombined = *(unsigned int*)&hdr[0];
                        unsigned int dataLen = rawLength16 - 4;
                        W3GSDispatchByType(arg0, arg4, msgTypeCombined, (char*)buffer + 4, dataLen);
                    }
                }
            }
        }
        break;

    default:
        break;
    }

    *pOutMsgId = msgId;
    return 1;
}
