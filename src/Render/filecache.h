//============================================================================
//  Storm's file cache - `.\FileCache.cpp`, the string the module's own
//  SMemAlloc/SMemFree calls pass in the __FILE__ slot.
//
//  Three string-keyed hash tables and a handful of module globals:
//
//    0x6FAB7A3C  TAllocatorHashTable<PrefetchNode,   HASHKEY_STRI,   128>
//    0x6FAB7A64  TAllocatorHashTable<UncachableNode, HASHKEY_STRI,    32>
//    0x6FAB7A8C  TSHashTable_P<PrefetchListFile,     HASHKEY_STRI_P>
//
//  All three class names, key policies and block sizes are read out of the
//  binary: the three constructors at 0x6F861800/0x6F861820/0x6F861840 each
//  stamp the instantiation's own `??_7...@@6B@` into the table they build.
//  See Containers/allochashtable.h.
//
//  What the module does: a file loaded through LoadCachedFile stays in the
//  prefetch table keyed by its name, with its bytes and length beside it,
//  until the table has more entries than the limit and the least recently
//  linked one is dropped.  A name that failed to load once goes into the
//  *uncachable* table so the cache stops trying.  The third table is the
//  recording side of the prefetch list - the names a session actually
//  touched, so a later run can pull them in up front.
//============================================================================
#ifndef FILECACHE_H
#define FILECACHE_H

#include "allochashtable.h"

//  0x6F6D83A0 / 0x6F6D83B0 - Sync/critsec.cpp.  Spelled against the tag
//  name windows.h itself mangles the type as, so this header does not have
//  to pull windows.h in and the two declarations still name one symbol.
void __fastcall EnterCritSec(struct _RTL_CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(struct _RTL_CRITICAL_SECTION* cs);

//  Storm.dll by ordinal: 279 loads a whole file, 280 releases what 279
//  handed back.  The five arguments are SFileLoadFile's - name, buffer,
//  length, how many extra bytes to over-allocate (this module always asks
//  for one, for the NUL it appends), and a load callback.
int  __stdcall Storm_279(const char* filename, void** buffer, int* length,
                         int extraBytes, void* callback);
int  __stdcall Storm_280(void* buffer);

extern "C" void* __cdecl memcpy(void* dest, const void* src, unsigned int n);

//----------------------------------------------------------------------------
//  The event a PrefetchNode is loaded behind.  Only the wait is reached from
//  this closure - `add ecx,20h` / `jmp` at 0x6F4BC650 is a member tail call
//  on the sub-object at +0x20, and the body it tails into reads the handle
//  at +0x10 and blocks on it forever.
//----------------------------------------------------------------------------
struct SLoadEvent
{
    //  0x6F6D7E90 - Render/renderthunks.cpp.
    void Wait();

    char  m_reserved00[0x10];   // +0x00
    void* m_handle;             // +0x10
};

//----------------------------------------------------------------------------
//  One cached file.  The hash base is TSHashObject's - a hash value, the
//  bucket link and the table-wide link - with the key at +0x14 where that
//  base puts it.
//----------------------------------------------------------------------------
struct PrefetchNode
{
    //  0x6F4BC650 - Render/prefetchwait.cpp.
    void WaitReady();

    unsigned int          m_hashval;    // +0x00
    TSLink<PrefetchNode>  m_hashlink;   // +0x04
    TSLink<PrefetchNode>  m_listlink;   // +0x0C
    const char*           m_key;        // +0x14  the file name
    void*                 m_data;       // +0x18
    unsigned int          m_size;       // +0x1C
    SLoadEvent            m_loaded;     // +0x20
    int                   m_useCount;   // +0x34
    int                   m_stamp;      // +0x38
    void*                 m_arg0;       // +0x3C
    void*                 m_arg4;       // +0x40
};

//----------------------------------------------------------------------------
//  A name the cache has given up on.  Key and nothing else.
//----------------------------------------------------------------------------
struct UncachableNode
{
    unsigned int            m_hashval;  // +0x00
    TSLink<UncachableNode>  m_hashlink; // +0x04
    TSLink<UncachableNode>  m_listlink; // +0x0C
    const char*             m_key;      // +0x14
};

//----------------------------------------------------------------------------
//  A name the recording side of the prefetch list has seen.
//----------------------------------------------------------------------------
struct PrefetchListFile
{
    unsigned int              m_hashval;    // +0x00
    TSLink<PrefetchListFile>  m_hashlink;   // +0x04
    TSLink<PrefetchListFile>  m_listlink;   // +0x0C
    const char*               m_key;        // +0x14  pooled, not owned
    int                       m_recorded;   // +0x18
};

typedef TAllocatorHashTable<PrefetchNode, HASHKEY_STRI, 128>   CPrefetchTable;
typedef TAllocatorHashTable<UncachableNode, HASHKEY_STRI, 32>  CUncachableTable;
typedef TSHashTable_P<PrefetchListFile, HASHKEY_STRI_P>        CPrefetchListTable;

//: The three tables are the objects, not pointers to them - every reference
//: in the module is `mov ecx, offset <sym>`.
extern CPrefetchTable      g_prefetchTable;         // dword_6FAB7A3C
extern CUncachableTable    g_uncachableTable;       // dword_6FAB7A64
extern CPrefetchListTable  g_prefetchListTable;     // dword_6FAB7A8C

//: The module's own counters and switches.
extern unsigned int g_prefetchBytes;        // dword_6FAB7A14
extern int          g_prefetchCount;        // dword_6FAB7A18
extern int          g_prefetchLimit;        // dword_6FAB7A1C
extern int          g_fileCacheEnabled;     // dword_6FAB7A20
extern int          g_prefetchStamp;        // dword_6FAB790C
extern unsigned char g_recordPrefetchList;  // byte_6FAB7910

//: The one lock the whole module takes.  IDA named it `CriticalSection`.
extern struct _RTL_CRITICAL_SECTION g_fileCacheLock;

//  0x6F4BC660 - Misc/trivialaccessors_04.cpp.  Drops PrefetchNode::m_useCount.
void __fastcall DecField0x34_6F4BC660(void* self);

//  --- the module's own functions --------------------------------------------
//  0x6F4BC6B0 - hand a loaded buffer back, either to Storm's own loader or to
//  the heap, depending on which of the two produced it.
int  __fastcall ReleaseLoadedFile(void* buffer, int fromHeap);
//  0x6F4BE9E0 - drop one node out of the cache.  Its argument arrives in esi
//  in the shipped build; see the .cpp.
void __fastcall DropPrefetchNode(PrefetchNode* node);
//  0x6F4BEA10 - drop the oldest node if the cache is over its limit.
int  TrimPrefetchCache();
//  0x6F4C1300 - find the cached file or load it.  Its name arrives in edi and
//  its `created` flag in eax in the shipped build; see the .cpp.
PrefetchNode* __fastcall FindOrLoadPrefetchNode(const char* name, int* created,
                                                void* arg0, void* arg4);
//  0x6F4C1400 - FindOrLoadPrefetchNode, waited on and reference-counted.
PrefetchNode* __fastcall AcquirePrefetchNode(const char* name);
//  0x6F4C1490 - remember that this name cannot be cached.
void __fastcall MarkFileUncachable(const char* name);
//  0x6F4C1550 - the module's public entry: hand back a file's bytes, out of
//  the cache when it is there and through Storm's own loader when it is not.
int  __fastcall LoadCachedFile(const char* name, void** buffer, int* length,
                               int takeOwnership);

#endif
