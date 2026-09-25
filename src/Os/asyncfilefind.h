//============================================================================
//  The registry lookup every entry point in .\AsyncFile.cpp starts with,
//  and the two lock calls that bracket it.
//
//  __forceinline, not plain inline: /Ob2's own cost model declines the walk
//  at this size and emits a real call, where the shipped stream writes the
//  same eight-instruction masked tail-read and key walk out in full inside
//  each of the five callers with no call between the lock and the first
//  compare.  Same reason storm.h's TSGrowableArray::Add carries it.
//============================================================================
#ifndef ASYNCFILEFIND_H
#define ASYNCFILEFIND_H

#include "asyncfile.h"

__forceinline void AsyncFileLock(unsigned int mode)
{
    ((ConditionVariable*)&g_asyncFiles)->Wait(mode);
}

__forceinline void AsyncFileUnlock(unsigned int mode)
{
    ((ConditionVariable*)&g_asyncFiles)->SignalOne(mode);
}

//  The `> 0 ? x : 0` is the shipped `setle` / `sub 1` / `and` triple: a tail
//  link is either a positive object pointer or a negative complemented
//  terminator address, and the walk wants null for the second.
__forceinline ASYNCFILE* AsyncFileFind(void* key)
{
    int node = g_asyncFiles.m_files.m_terminator.m_prevlink;
    node = node > 0 ? node : 0;

    while (node > 0)
    {
        if (((ASYNCFILE*)node)->m_key == key)
            return (ASYNCFILE*)node;
        node = ((ASYNCFILE*)node)->m_link.m_prevlink;
    }

    return 0;
}

#endif
