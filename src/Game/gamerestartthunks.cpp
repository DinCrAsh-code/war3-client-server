//============================================================================
//  Naked redirects for the callees the three game-state natives' own chain
//  declares and calls but does not reconstruct.  Argument counts came off
//  the shipped call sites' push counts; everything in this binary is
//  callee-cleanup, so a site that pushes nothing and does not adjust esp can
//  only be calling something that pops nothing.  None of these has a dump
//  body in this tree, so tools/thunk_abi_audit.py reports them as unknown.
//
//      0x6F534CE0  ecx = this, two pushes            -> retn 8 (a member:
//                  a __fastcall free function takes the first in edx and
//                  cleans four bytes too few - thunk_abi_audit.py MISMATCH)
//      0x6F011300  ecx = this, one push              -> retn 4 (a member:
//                  see RCString::Construct in src/Misc/rcstring.h - the free
//                  __fastcall this was cleaned four bytes too few)
//      0x6F535DC0  ecx = this, nothing pushed        -> retn 0
//      0x6F471910  ecx = a, edx = b, nothing pushed  -> retn 0
//      0x6F3DACD0  nothing at all                    -> retn 0
//      0x6F4778F0  ecx = host, five pushes           -> retn 14h
//      0x6F0202E0  ecx = event, nothing pushed       -> retn 0
//      0x6F01FCB0  ecx = string, one push            -> retn 4
//      0x6F537B20  ecx = this, nothing pushed        -> retn 0
//============================================================================
#include "savegame.h"
#include "gamerestart.h"
#include "gamedatawriter.h"
#include "objectdataowner.h"
#include "netgameload.h"
#include "rcstring.h"

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

ADDR_THUNK(void SGameDataBase::Construct(int, int), 0x6F534CE0)
ADDR_THUNK(void RCString::Construct(const char*), 0x6F011300)
ADDR_THUNK(void __fastcall SaveGameBaseDestruct(void*), 0x6F535DC0)

//----------------------------------------------------------------------------
//  A second block, for the small functions landed alongside the natives.
//
//      0x6F535660  ecx = stream, nothing pushed     -> retn 0
//      0x6F5349B0  ecx = stream, four pushes        -> retn 10h (a member,
//                  same reason)
//      0x6F535A90  ecx = stream, nothing pushed     -> retn 0
//      0x6F7E7F30  ecx = file, nothing pushed       -> retn 0
//      0x6F4C7060  ecx = cache, nothing pushed      -> retn 0
//      0x6F3F4410  ecx = key, nothing pushed        -> retn 0
//      0x6F3E94D0  ecx = this, two pushes           -> retn 8 (a member,
//                  same reason)
//      0x6F405810  ecx = store, edx = body, no push -> retn 0
//      0x6F535050  ecx = this, one push             -> retn 4 (a member,
//                  same reason)
//----------------------------------------------------------------------------
struct ASYNCFILE;
class CDataStore;
class CDataStoreScratch;
struct SNetMsg653;
struct SDataStoreCache;

ADDR_THUNK(void __fastcall GameDataStreamFlush(void*), 0x6F535660)
ADDR_THUNK(int SGameDataWriter::WriteRecord(int, int, int, int), 0x6F5349B0)
ADDR_THUNK(void SGameDataWriter::Abandon(), 0x6F535A90)
ADDR_THUNK(void __fastcall AsyncFileReset(ASYNCFILE*), 0x6F7E7F30)
ADDR_THUNK(void __fastcall DataStoreCacheRetireOne(SDataStoreCache*), 0x6F4C7060)
ADDR_THUNK(void* __fastcall ObjectDataFind(const void*), 0x6F3F4410)
ADDR_THUNK(void SObjectDataOwner::Register(int, void**), 0x6F3E94D0)
ADDR_THUNK(CDataStore* __fastcall GameCacheReadEntryBody(CDataStore*, void*), 0x6F405810)

ADDR_THUNK(int SGameDataLoad::LoadBlocks(NetGameLoadSource*), 0x6F535050)


//----------------------------------------------------------------------------
//  A third block: the frontier of this session's own chain.  Each of these is
//  a real function in the dumps that a later session should reconstruct; the
//  redirect is what lets the callers above link and run against the shipped
//  body in the meantime.
//
//      0x6F3A83E0  ecx = world, nothing pushed       -> retn 0
//      0x6F4061F0  ecx = store, one push             -> retn 4
//      0x6F545A80  ecx = record, edx = index, no push -> retn 0
//----------------------------------------------------------------------------
ADDR_THUNK(void* __fastcall WorldEnsureObjectDataCache(void*), 0x6F3A83E0)
ADDR_THUNK(int SGameCacheStore::ReloadFromDisk(int), 0x6F4061F0)
ADDR_THUNK(int __fastcall SlotRecordCapture(void*, int), 0x6F545A80)
