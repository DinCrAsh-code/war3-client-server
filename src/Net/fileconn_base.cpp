//============================================================================
//  OsNet::FILECONN - the base of the file-connection trio (IOFILECONN,
//  SLFILECONN), `: public NETCONN`.  See osnetfamily.h for the class.
//  ReleasePending, OnWriteComplete and OnReadComplete (slots 6/7/8).
//============================================================================
#include "osnetfamily.h"
#include "ostcp.h"   // TCPCONN::Shutdown - see the file header in
                     // osnetfamily.h for why the (TCPCONN*)this reinterpret
                     // is sound: Shutdown only ever touches NETCONN's own
                     // base fields.

//  Storm.dll ordinal 403.  Declared here, ahead of `namespace OsNet`,
//  rather than block-scoped below - see fileconn_deleteself.cpp's own
//  comment on this for why.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

namespace OsNet
{

typedef void (__thiscall *FileConnNotifyFn)(void* thisObj, void* a, void* b,
                                            int c, int d, void* e,
                                            int reason);
typedef void (__thiscall *PostWriteFn)(void* thisObj, void* node);

//----------------------------------------------------------------------------
//  0x6F6E0800 - slot 6, retn 0.  Drop one pending-I/O reference under the
//  connection lock; on reaching zero, dispatch slot 3 (OnPendingDrained)
//  through the vtable, then Shutdown(1) via the (TCPCONN*)this reinterpret.
//  Otherwise just release the lock.
//----------------------------------------------------------------------------
void FILECONN::ReleasePending()
{
    EnterCriticalSection(&m_critSec);
    if (InterlockedDecrement((LONG*)&m_pending) != 0)
    {
        LeaveCriticalSection(&m_critSec);
        return;
    }

    typedef void (__thiscall *OnPendingDrainedFn)(void*);
    ((OnPendingDrainedFn)(*(void***)this)[3])(this);

    ((TCPCONN*)this)->Shutdown(1);
}

//----------------------------------------------------------------------------
//  0x6F6E0840 - slot 7 (a completed write).  0x6F6E0840, retn 0Ch.  Marks
//  the just-completed node "fully sent" (m_sendState-shaped: state 2,
//  transferred byte count recorded), then drains the queue from its head:
//  a head already marked fully sent is unlinked, notified (reason 4,
//  0x6F6DA310, out of this session's scope) and freed, repeating until the
//  head is not fully sent; a head that has never been started is armed via
//  slot 10 (PostWrite); anything else just releases the lock.  A notify
//  that reports the connection has closed mid-drain ends the loop with a
//  Close() dispatch instead of re-locking.
//----------------------------------------------------------------------------
void FILECONN::OnWriteComplete(void* node, void* unused, DWORD bytes)
{
    (void)unused;

    EnterCriticalSection(&m_critSec);

    *(int*)((char*)node + 0x18) = 2;
    *(DWORD*)((char*)node + 0x20) = bytes;

    for (;;)
    {
        void* head = m_queueHeadNext;
        if (head == 0)
        {
            LeaveCriticalSection(&m_critSec);
            return;
        }

        int headState = *(int*)((char*)head + 0x20);
        if (headState != 2)
        {
            if (headState == 0)
            {
                typedef void (__thiscall *PostWriteSlotFn)(void*, void*);
                ((PostWriteSlotFn)(*(void***)this)[10])(this, head);
            }
            else
            {
                LeaveCriticalSection(&m_critSec);
            }
            return;
        }

        //  Unlink `head` off the queue (TSList-shaped, see storm.h).
        void* nextLink = *(void**)head;
        void* prevSlot = *(void**)((char*)head + 4);
        if (nextLink != 0)
            *(void**)((char*)prevSlot) = nextLink;
        m_queueHeadNext = nextLink;

        BOOL closed = FALSE;
        LeaveCriticalSection(&m_critSec);
        if (m_hFile != (HANDLE)-1)
        {
            ((FileConnNotifyFn)0x6F6DA310)(this, 0, 0, 0, 0, 0, 4);
            closed = TRUE;
        }

        void* buf = *(void**)((char*)head + 0x30);
        if (buf != 0)
        {
            SMemFree(buf, ".\\W32\\OsTcp.cpp", 0x190, 0);
        }

        {
            SMemFree(head, "delete", -1, 0);
        }

        if (closed)
        {
            typedef void (__thiscall *CloseFn)(void*);
            ((CloseFn)(*(void***)this)[9])(this);
            return;
        }

        EnterCriticalSection(&m_critSec);
    }
}

//----------------------------------------------------------------------------
//  0x6F6E2740 - slot 8 (a completed read).  Same drain shape as
//  OnWriteComplete (reason 5 rather than 4), written out separately rather
//  than delegating - the shipped body is its own full copy of the loop, not
//  a forwarder, so a thin wrapper here scored far worse than a matching
//  independent body does.
//----------------------------------------------------------------------------
void FILECONN::OnReadComplete(void* node, DWORD bytes, BOOL ok)
{
    (void)ok;

    EnterCriticalSection(&m_critSec);

    *(int*)((char*)node + 0x18) = 2;
    *(DWORD*)((char*)node + 0x20) = bytes;

    for (;;)
    {
        void* head = m_queueHeadNext;
        if (head == 0)
        {
            LeaveCriticalSection(&m_critSec);
            return;
        }

        int headState = *(int*)((char*)head + 0x20);
        if (headState != 2)
        {
            if (headState == 0)
            {
                typedef void (__thiscall *PostWriteSlotFn)(void*, void*);
                ((PostWriteSlotFn)(*(void***)this)[10])(this, head);
            }
            else
            {
                LeaveCriticalSection(&m_critSec);
            }
            return;
        }

        void* nextLink = *(void**)head;
        void* prevSlot = *(void**)((char*)head + 4);
        if (nextLink != 0)
            *(void**)((char*)prevSlot) = nextLink;
        m_queueHeadNext = nextLink;

        BOOL closed = FALSE;
        LeaveCriticalSection(&m_critSec);
        if (m_hFile != (HANDLE)-1)
        {
            ((FileConnNotifyFn)0x6F6DA310)(this, 0, 0, 0, 0, 0, 5);
            closed = TRUE;
        }

        void* buf = *(void**)((char*)head + 0x30);
        if (buf != 0)
        {
            SMemFree(buf, ".\\W32\\OsTcp.cpp", 0x190, 0);
        }

        {
            SMemFree(head, "delete", -1, 0);
        }

        if (closed)
        {
            typedef void (__thiscall *CloseFn)(void*);
            ((CloseFn)(*(void***)this)[9])(this);
            return;
        }

        EnterCriticalSection(&m_critSec);
    }
}

}  // namespace OsNet
