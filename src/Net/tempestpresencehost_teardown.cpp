//============================================================================
//  0x6F46FCC0 - the "Agile"/NIpse subsystem's own shutdown routine: tears
//  down its type-descriptor registries, drains/resets four CSyncChannel-
//  sized sub-objects embedded in `g_pTimeSync` (Game/game.h's own
//  CTimeSync - two of which, `m_localChannel`/`m_remoteChannel` at
//  +0x14/+0x68, are already named there; this call tree also touches two
//  more at +0xBC/+0x110, immediately following them at the same 0x54-byte
//  stride, which game.h does not yet name), releases `g_pTimeSync`'s own
//  `m_pathSearch`/`m_pathOwner` (+0x24C/+0x250, `CTimeSync::GetPathSearch`/
//  `GetPathOwner`'s own targets), destroys and NULLs both `g_pTimeSync`
//  itself and the tempest presence host (`g_unk6FAB778C`), and finally
//  frees this object's own owned array of `AgileAgentQuery*` pointers.
//
//  Investigated as part of "does any of the 8 unreconstructed
//  g_unk6FAB778C-touching functions perform the missing
//  STwoIndexedFreeLists::AllocateAt call" (docs/notes/checksum-provider-
//  registry.md).  Answer for this one: no.  The only thing this function
//  does to `g_unk6FAB778C` is destroy it, free it and null the global
//  (`DestructTempestPresenceHost` + `SMemFree("delete", ...)` +
//  `g_unk6FAB778C = 0`) - the exact teardown counterpart of
//  tempestpresencehostctor.cpp's own `RebuildTempestPresenceHost`, not an
//  entry-level allocation into its own free-list array.
//
//  `arg_0 == 0` skips the whole progress-callback-driven tree walk below
//  and only tears down the three type-descriptor chains
//  (`unk_6FAAE2D8`/`unk_6FAAE2C4`/`unk_6FAAE2B0`, tagged
//  "AUAgileTypeRelation"/"...Description"/"...Data" in the dump's own
//  string table) before falling into the shared teardown tail; `arg_0 != 0`
//  additionally walks a tree rooted at `this+0x18` (a throttled walk -
//  `GetTickCount`-gated to roughly 64ms or 8 nodes between progress-
//  callback invocations, calling each visited node's own vtable slot 4 to
//  clear something at that node's own `+0x6C`) before the same shared tail.
//  `arg_4` is a caller-supplied progress/idle callback invoked at several
//  fixed points along the way; it is optional (checked against null before
//  every call).
//
//  None of the individual helper leaves below (drain/reset a channel,
//  release a path-search/path-owner pair, walk the AgileAgentQuery array)
//  is one of this session's eight addresses or already reconstructed
//  elsewhere, so all stay naked redirects - the point of this
//  reconstruction is the shutdown's own control flow and its two
//  g_pTimeSync/g_unk6FAB778C teardown calls, not every leaf underneath it.
//  Two of the four channel-reset pairs dispatch into
//  `SScheduler::DrainDueNodes` (Sync/scheduler.h, already reconstructed) by
//  address, which is surprising for a CSyncChannel-sized object - noted
//  rather than resolved, since this call tree does not otherwise
//  characterize what lives at `g_pTimeSync`+0xBC/+0x110 beyond "the same
//  0x54-byte stride as `m_localChannel`/`m_remoteChannel`".
//============================================================================
#include "tempestpresencehost.h"
#include "game.h"
#include "scheduler.h"
#include <string.h>
#include <windows.h>

//  0x6F46B790 - defined (as a naked redirect) in tempestpresencehostctor.cpp;
//  declared, not redefined, here to avoid a duplicate symbol.
void __fastcall DestructTempestPresenceHost(void* obj);

//  0x6F46CF60 - tear down the throttled-walk tree root at `this+4`/`this+0xC`
//  (a growable node array walked via `[edi+0x1Ch]`), then zero `this+0x10`.
//  `retn 4` (one stack arg, a bool flag threaded down into each visited
//  node's own release).
ADDR_THUNK(void __fastcall TeardownAgileNodeTree(void* self, int flag), 0x6F46CF60)

//  0x6F4C1A60 - walk a singly-linked chain off `receiver->m_field0C`,
//  `SMemFree`-ing each node with the given type-descriptor name (or, once
//  the chain is empty, a fixed default filename/line), then zero
//  `receiver`'s own `+0x08`/`+0x10`.  `retn 0Ch` (three stack args, `this`
//  in ecx - already established via its own call sites' shape, not
//  reconstructed itself as it is not one of this session's addresses).
ADDR_THUNK(void __fastcall DestroyTypeDescriptorChain(void* receiver, const char* name,
                                                       int code, int flag), 0x6F4C1A60)

//  0x6F4814F0 - release `self`'s own `+0x24C`/`+0x250` sub-objects (a
//  virtual `Release(0)` call through each one's own vtable slot 4) and null
//  both fields - `CTimeSync::m_pathSearch`/`m_pathOwner`
//  (Game/game.h), reached here with `self = g_pTimeSync` directly.
ADDR_THUNK(void __fastcall ReleasePathSearchAndOwner(void* self), 0x6F4814F0)

//  0x6F46F390/0x6F46F3A0 - "reset a channel-shaped sub-object against a
//  float threshold" (the same fixed threshold, `flt_6FAAE4B0`, at every
//  call site below).  `retn 4`.
ADDR_THUNK(void __fastcall ResetSyncChannelA(void* channel, const float* threshold), 0x6F46F390)
ADDR_THUNK(void __fastcall ResetSyncChannelB(void* channel, const float* threshold), 0x6F46F3A0)

//  0x6F46E910 - the drain counterpart run right after each `ResetSyncChannelA`
//  above, on `m_localChannel`/`m_remoteChannel` specifically (+0x14/+0x68).
//  `retn` (no stack args).  `SScheduler::DrainDueNodes` (Sync/scheduler.h,
//  already reconstructed) plays the identical role for the two channel-sized
//  sub-objects at +0xBC/+0x110 - called by address below, not through this
//  declaration, since it is a different, already-named class.
ADDR_THUNK(void __fastcall DrainSyncChannelA(void* channel), 0x6F46E910)

//  0x6F46E370 / 0x6F46D490 - the two-step teardown this function runs on
//  `this+0x40`/`this+0x44` specifically (not the four channel sub-objects
//  above): `PrepareRefCountedTeardown` first, then `ThenRefCountedTeardown`
//  a second time on the same (possibly re-read) pointer, before the
//  object's own vtable slot 0 is finally invoked with `flags=1`.  Neither
//  is characterised beyond that call shape.
ADDR_THUNK(void __fastcall PrepareRefCountedTeardown(void* obj), 0x6F46E370)
ADDR_THUNK(void __fastcall ThenRefCountedTeardown(void* obj), 0x6F46D490)

//  0x6F489E70/0x6F46A860 - destructors for `g_pTimeSync` itself and for
//  this object's own `+0x34` sub-object, each carrying its own
//  `__CxxFrameHandler3`-shaped SEH frame (docs/msvc-vc8-idioms.md's own
//  unreproducible-with-this-toolchain example).
ADDR_THUNK(void __fastcall DestructTimeSync(void* self), 0x6F489E70)
ADDR_THUNK(void __fastcall DestructAgileField34(void* self), 0x6F46A860)

//  0x6F46DA10 - the Agile subsystem's own bring-up (see
//  tempestpresencehostctor.cpp's own header) - the counterpart this
//  function's `arg_0 != 0` path calls to re-arm everything just torn down.
//  Not reconstructed this session.
ADDR_THUNK(void __fastcall AgileSubsystemInit(void* self), 0x6F46DA10)

typedef void (__stdcall *AgileProgressCallback)();

//  `this` (`edi`) is the Agile subsystem manager object: `+0x0C`/`+0x18` the
//  throttled-walk tree, `+0x34` an owned sub-object, `+0x40`/`+0x44` two
//  ref-counted objects released through their own vtable slot 0,
//  `+0x4C`/`+0x50`/`+0x54` a flat memory block, `+0x58`/`+0x5C`/`+0x60` a
//  growable `AgileAgentQuery*` array (tagged `.PAVAgileAgentQuery@@` in the
//  dump's own type-descriptor string table).
void TeardownAgileSubsystem(void* self, int full, AgileProgressCallback callback)
{
    char* edi = (char*)self;

    if (!full)
    {
        TeardownAgileNodeTree((char*)edi + 0x0C, 0);
        DestroyTypeDescriptorChain((void*)0x6FAAE2D8, "AUAgileTypeRelation", -2, 1);
        DestroyTypeDescriptorChain((void*)0x6FAAE2C4, "AUAgileTypeDescription", -2, 1);
        DestroyTypeDescriptorChain((void*)0x6FAAE2B0, "AUAgileTypeData", -2, 1);
    }
    else
    {
        unsigned int startTick = GetTickCount();
        int throttleCount = 0;
        int* node = *(int**)((char*)edi + 0x18);

        while (node)
        {
            //  Visit `node`'s own children (`node[0x6C/4]`'s own vtable
            //  slot 4), throttled roughly every 64ms or 8 nodes.
            int* child = (int*)node[0x6C / 4];
            if (child)
            {
                typedef void (__thiscall *NodeVisitFn)(void*, void*);
                ((NodeVisitFn)(*(void***)child)[4])(child, &startTick);
                node[0x6C / 4] = 0;
            }

            if (callback)
            {
                if (++throttleCount > 8)
                {
                    if (GetTickCount() - startTick > 0x3F)
                        throttleCount = 0;
                    else
                        callback();
                }
            }

            int* next = *(int**)((char*)node + 4);
            node = (next && *next > 0) ? next : 0;
        }

        if (callback)
            callback();

        TeardownAgileNodeTree((char*)edi + 0x0C, full != 0);

        if (callback)
            callback();
    }

    //  Shared tail: reset/drain the four CTimeSync-embedded channel-shaped
    //  sub-objects, release its path-search/path-owner pair, then destroy
    //  and null g_pTimeSync itself and the tempest presence host in turn.
    static const float threshold = *(float*)0x6FAAE4B0;

    if (g_pTimeSync)
    {
        char* base = (char*)g_pTimeSync;

        ResetSyncChannelA(base + 0x14, &threshold);
        DrainSyncChannelA(base + 0x14);
        ResetSyncChannelA(base + 0x68, &threshold);
        DrainSyncChannelA(base + 0x68);
        ResetSyncChannelB(base + 0xBC, &threshold);
        ((SScheduler*)(base + 0xBC))->DrainDueNodes();
        ResetSyncChannelB(base + 0x110, &threshold);
        ((SScheduler*)(base + 0x110))->DrainDueNodes();

        if (callback)
            callback();

        ReleasePathSearchAndOwner(g_pTimeSync);
        if (callback)
            callback();
    }

    //  `+0x40`/`+0x44`: two ref-counted sub-objects, released the same way
    //  regardless of `full`.
    void** field40 = *(void***)(edi + 0x40);
    if (field40)
    {
        PrepareRefCountedTeardown(field40);
        field40 = *(void***)(edi + 0x40);
        if (field40)
        {
            ThenRefCountedTeardown(field40);
            field40 = *(void***)(edi + 0x40);
            if (field40)
                ((void (__thiscall *)(void*, int))(*(void***)field40)[0])(field40, 1);
        }
    }
    void** field44 = *(void***)(edi + 0x44);
    if (field44)
    {
        PrepareRefCountedTeardown(field44);
        field44 = *(void***)(edi + 0x44);
        if (field44)
        {
            ThenRefCountedTeardown(field44);
            field44 = *(void***)(edi + 0x44);
            if (field44)
                ((void (__thiscall *)(void*, int))(*(void***)field44)[0])(field44, 1);
        }
    }

    if (callback)
        callback();

    if (g_pTimeSync)
    {
        void* ts = g_pTimeSync;
        DestructTimeSync(ts);
        SMemFree(ts, "delete", -1, 0);
    }
    g_pTimeSync = 0;

    if (g_unk6FAB778C)
    {
        void* host = g_unk6FAB778C;
        DestructTempestPresenceHost(host);
        SMemFree(host, "delete", -1, 0);
    }
    g_unk6FAB778C = 0;

    void* field34 = *(void**)(edi + 0x34);
    if (field34)
    {
        DestructAgileField34(field34);
        SMemFree(field34, "delete", -1, 0);
    }
    *(void**)(edi + 0x34) = 0;

    if (callback)
        callback();

    if (full)
    {
        AgileSubsystemInit(self);
    }
    else
    {
        //  Release every live `AgileAgentQuery*` in the owned array
        //  (`+0x58`/`+0x5C`/`+0x60`), then free the array's own storage and
        //  clear its bookkeeping fields (`+0x4C`/`+0x50`/`+0x54`).
        void** array = *(void***)(edi + 0x60);
        unsigned int count = *(unsigned int*)(edi + 0x5C);
        for (unsigned int i = 0; i < count; ++i)
        {
            void* entry = array ? array[i] : 0;
            if (entry)
            {
                void* target = *(void**)entry;
                if (target)
                {
                    typedef void (__thiscall *ReleaseFn)(void*, int);
                    ((ReleaseFn)(*(void***)target)[0])(target, 1);
                }
            }
        }
        if (array)
            SMemFree(array, "AUAgileAgentQuery", -2, 0);

        *(void**)(edi + 0x58) = 0;
        *(unsigned int*)(edi + 0x5C) = 0;
        *(void**)(edi + 0x60) = 0;

        void* blockPtr = *(void**)(edi + 0x50);
        unsigned int blockSize = *(unsigned int*)(edi + 0x4C);
        if (blockPtr)
            memset(blockPtr, 0, blockSize);
        *(void**)(edi + 0x54) = 0;
    }

    if (callback)
        callback();
}
