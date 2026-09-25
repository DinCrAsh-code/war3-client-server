//============================================================================
//  .\AsyncFile.cpp - the asynchronous file layer.
//
//  The module name is not a guess: AsyncFileOpen's own SMemAlloc tags itself
//  ".\\AsyncFile.cpp", and the per-operation record's frees hand Storm the
//  RTTI descriptor ".?AUASYNCFILEOP@@" instead of a filename, i.e.
//  `struct ASYNCFILEOP`.
//
//  One process-wide registry (0x6FAE7578) holds every open async file on an
//  intrusive list under a two-mode lock - Storm's own ConditionVariable,
//  which every entry and exit here goes through with a mode of 0 or 1.  Each
//  ASYNCFILE then has its own lock, its own pool of ASYNCFILEOPs and eight
//  hash buckets keyed on the low three bits of the operation id.
//
//  Reached by all three game-state natives: the reload path opens the map
//  archive through this, and the shutdown path closes it.
//============================================================================
#ifndef ASYNCFILE_H
#define ASYNCFILE_H

#include <windows.h>
#include "storm.h"
#include "conditionvariable.h"
#include "event.h"

struct ASYNCFILE;

//----------------------------------------------------------------------------
//  One queued operation.  0x30 bytes, which is both what the pool is asked
//  for and what every free of one passes back.
//----------------------------------------------------------------------------
struct ASYNCFILEOP
{
    TSLink<ASYNCFILEOP> m_link;     // +0x00 - the owning bucket's list
    unsigned int        m_id;       // +0x08 - the ticket the caller cancels by
    char                m_reservedC[0x04];   // +0x0C
    int                 m_arg0;     // +0x10
    int                 m_arg1;     // +0x14
    void*               m_lock;     // +0x18 - the caller's own lock, or null
    int                 m_arg2;     // +0x1C
    int                 m_arg3;     // +0x20
    int                 m_state;    // +0x24
    int                 m_arg4;     // +0x28
    int                 m_arg5;     // +0x2C
};                                  // 0x30

//----------------------------------------------------------------------------
//  A pool of them.  Only the two entry points this call tree reaches are
//  named; the shape is Storm's CDataAllocator, which src/Agent already has
//  at 0x6F4C1AB0/0x6F4C1B50 under different instantiations.
//----------------------------------------------------------------------------
struct ASYNCFILEOPPOOL
{
    //  0x6F7E8040 - retn 0.  Take one, growing the pool if it is empty.
    ASYNCFILEOP* Alloc();

    //  0x6F4C68F0 - retn 10h.  Give one back.
    void Free(void* block, unsigned int size, const char* tag, int line);

    char m_reserved0[0x18];
};

//----------------------------------------------------------------------------
//  One open asynchronous file.  0xC8 bytes - the size AsyncFileOpen asks
//  Storm for - and every offset below is pinned by that plus the two array
//  strides the bucket walks use (12 for the lists at +0x5C, and the eight
//  buckets ending exactly at +0xBC).
//----------------------------------------------------------------------------
struct ASYNCFILE
{
    //  0x6F7E8340 - retn 1Ch.  Take an ASYNCFILEOP from the pool, fill it
    //  in, give it the next free id and hang it on that id's bucket.
    //  A member, not a free function taking the receiver: the shipped call
    //  site puts the file in ecx and pushes all seven other arguments,
    //  which is __thiscall and 28 bytes of callee cleanup.  A
    //  `__fastcall` free function would take the second in edx and clean
    //  four bytes too few - the crash class tools/abi_audit.py's FATAL
    //  bucket exists for.
    unsigned int QueueOp(void* callerLock, int a0, int a1, int a2,
                         int a3, int a4, int a5);

    //  0x6F7E8440 - retn 4.  Unlink and free the operation with that id.
    void CancelOp(unsigned int id);

    void**              m_vtable;       // +0x00 - slot 0 is the abort
    TSLink<ASYNCFILE>   m_link;         // +0x04 - the registry's list
    void*               m_key;          // +0x0C - what a lookup matches on
    void*               m_transport;    // +0x10 - null once it has been shut
    int                 m_pending;      // +0x14
    Event*              m_openEvent;    // +0x18
    Event*              m_closeEvent;   // +0x1C
    Event*              m_flushEvent;   // +0x20
    ASYNCFILEOPPOOL     m_pool;         // +0x24 .. +0x3B
    CRITICAL_SECTION    m_lock;         // +0x3C .. +0x53
    unsigned int        m_nextId;       // +0x54
    int                 m_idWrapped;    // +0x58
    TSExplicitList<ASYNCFILEOP> m_buckets[8];   // +0x5C .. +0xBB
    char                m_reservedBC[0x0C];     // +0xBC .. +0xC7
};                                      // 0xC8

//----------------------------------------------------------------------------
//  The registry.  Storm's ConditionVariable is the lock, and the list of
//  live files is the twelve bytes just past its own state.
//----------------------------------------------------------------------------
struct ASYNCFILEREGISTRY
{
    char                      m_lockState[0x0C];   // +0x00
    TSExplicitList<ASYNCFILE> m_files;             // +0x0C .. +0x17
};

//: unk_6FAE7578 - the one registry.  Its first twelve bytes are the
//: ConditionVariable every Lock/Unlock below casts to.
extern ASYNCFILEREGISTRY g_asyncFiles;

//: stru_6FAE7558 - the lock the open path registers under.
extern CRITICAL_SECTION g_asyncFileOpenLock;

//----------------------------------------------------------------------------
//  The module's own entry points.
//----------------------------------------------------------------------------
ASYNCFILE* __fastcall AsyncFileOpen(const char* path, int context);   // 0x6F7E89E0
void __fastcall AsyncFileClose(ASYNCFILE* file);                      // 0x6F7E7E10
void __fastcall AsyncFileFlush(ASYNCFILE* file);                      // 0x6F7E7D40
unsigned int __fastcall AsyncFileQueue(ASYNCFILE* file, int a0, int a1,
                                       int a2, int a3, int a4, int a5);  // 0x6F7E85C0
void __fastcall AsyncFileCancelList(void* key,
                                    TSExplicitList<ASYNCFILEOP>* ops);   // 0x6F7E89D0



//: 0x6F7E72D0 / 0x6F7E72E0 - the pending-count pair around a queued call.
void __fastcall AsyncFileAddPending(ASYNCFILE* file);
void __fastcall AsyncFileDropPending(ASYNCFILE* file);

#endif
