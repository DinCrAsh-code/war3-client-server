//============================================================================
//  AcquireHProfile (0x6F4D18D0) / ConfigProfileHasSection_6F4D26C0
//  (0x6F4D26C0) - the two small "HPROFILE" wrappers Net/netdata_initconfig.cpp
//  (InitializeNetSessionConfig, 0x6F553920) calls to read the user's stored
//  Battle.net locale out of `config.txt`.
//
//  `AcquireHProfile` is the same allocate-and-construct-and-AddRef shape
//  Net/netdata_acquire.cpp/randdataprovider.cpp/GameUI/acquiregamesingleton.cpp
//  already established, just without a per-thread cache slot: allocate
//  `0x3C` bytes tagged `"HPROFILE"` (Storm's own log-file/log-line slots
//  carry the RTTI descriptor name and `-2`, not a real file/line - the same
//  idiom storm.h's own header documents), construct a
//  `ProfileInternal::PROFILE` there, and hand it straight to
//  `TRefCnt::AddRef` (0x6F4C40C0, Agent/refcnt.cpp) - which already handles
//  a null `this` (construction failure) by returning null rather than
//  faulting, so no extra check is needed here.
//
//  `ProfileInternal::PROFILE`'s own construction (0x6F4D1600) is a
//  different, unrelated subsystem this batch does not otherwise touch (a
//  `TAllocatorHashTable<SECTION,...>` of Windows-profile sections) - left a
//  naked redirect rather than reconstructed, the same "opens onto a
//  subsystem with nothing else here to reconstruct from" call
//  netdata_teardown.cpp's own header explains for
//  `NetProviderNotifyRelease_6F6533B0`.  Its return value (`this`, in `eax`)
//  is exactly what a normal constructor call gives back, so calling it
//  through a plain pointer-returning thunk reproduces that without lying
//  about the type.
//
//  `ConfigProfileHasSection_6F4D26C0` is a plain forwarder into
//  `sub_6F4D25D0` (a registry/profile-file lookup this batch also leaves a
//  naked redirect, for the same reason) - `this`/`name` pass straight
//  through untouched.  The one gap: the shipped body loads its own third
//  (stack) argument into `esi` and never reads it again before restoring
//  and returning - a genuine dead read, not a transcription slip, and not
//  reproduced here (see the `DIFFERS` note on this function in
//  `agent_worktrees`).  What actually reaches `sub_6F4D25D0`'s own third
//  parameter is `name` a second time, read straight out of the still-live
//  `edx` register.
//============================================================================
#include "storm.h"
#include "refcnt.h"

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig             \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F4D1600 - ProfileInternal::PROFILE::PROFILE() - out of scope, see the
//  file header.  Returns `this` in eax, exactly like a normal constructor.
ADDR_THUNK(void* __fastcall ConstructHProfile_6F4D1600(void* self), 0x6F4D1600)

//  0x6F4D25D0 - the registry/profile-file lookup ConfigProfileHasSection_
//  6F4D26C0 forwards into - out of scope, see the file header.
ADDR_THUNK(int __fastcall ConfigProfileHasSectionImpl_6F4D25D0(
               void* self, const char* name, const char* nameAgain),
           0x6F4D25D0)

#undef ADDR_THUNK

void* __fastcall AcquireHProfile()
{
    void* block = SMemAlloc(0x3C, "HPROFILE", -2, 0);

    void* obj;
    if (block)
        obj = ConstructHProfile_6F4D1600(block);
    else
        obj = 0;

    return ((TRefCnt*)obj)->AddRef("HPROFILE");
}

int __fastcall ConfigProfileHasSection_6F4D26C0(void* self, const char* name,
                                                int /*unused - see header*/)
{
    return ConfigProfileHasSectionImpl_6F4D25D0(self, name, name);
}
