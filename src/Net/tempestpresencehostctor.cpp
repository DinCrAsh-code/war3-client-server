//============================================================================
//  0x6F46D940 - unconditionally (re)builds the tempest presence host
//  (`g_unk6FAB778C`) from scratch: allocate a fresh 0xDC-byte object, run
//  it through its real constructor (`sub_6F46B6B0`), tear down and free
//  whatever object was previously installed (`sub_6F46B790` + a tagged
//  `SMemFree("delete", ...)`), then publish the new pointer.
//
//  Investigated as part of the "does any of the 8 unreconstructed
//  g_unk6FAB778C-touching functions perform the missing
//  STwoIndexedFreeLists::AllocateAt call" question
//  (docs/notes/checksum-provider-registry.md).  Answer for this one: no -
//  this constructs/replaces the *host object itself* (the
//  NTempest::CDynTable<CPrRgEntry> instance), not an entry inside its own
//  free-list array.  It is the concrete site the doc's own "Lead 2"
//  section already inferred existed ("the host object gets rebuilt
//  wholesale by sub_6F46D940 ... construct via sub_6F46B6B0") - this
//  session confirms the disassembly matches that description exactly and
//  gives it a real name.
//
//  The only caller (`AgileSubsystemInit`, `sub_6F46DA10`, not reconstructed
//  this session - see tempestpresencehost_teardown.cpp's own header for its
//  shutdown counterpart) calls this unconditionally, once, during the
//  NIpse/"Agile" subsystem's own bring-up (that function loads the literal
//  ASCII tag 0x69707365 = "ipse" into ecx right before this call) - not a
//  per-tick or per-move path.
//
//  `sub_6F46B6B0` (construct) and `sub_6F46B790` (destruct the four owned
//  0x20-byte sub-objects at +0x5C/+0x7C/+0x9C/+0xBC plus the +0x0C/+0x1C
//  growable free-list array) are both real, dumped bodies - see
//  checksum-provider-registry.md's own "Lead 2" batch - but neither is one
//  of this session's eight addresses, so both stay naked redirects here.
//
//  This body carries the same `push -1 / push offset SEH_.../ mov eax, fs:0`
//  `__CxxFrameHandler3`-shaped frame docs/msvc-vc8-idioms.md already
//  documents (the CAgentPtr/TSGrowableArray example, also cited by
//  AcquireGameSingleton and CGameWar3::CGameWar3) as not reproducible with
//  this toolchain's `/EHs-c-` - a real behavioural gap, not a
//  transcription slip, so the frame bytes are the entire expected shortfall
//  against the shipped stream.
//============================================================================
#include "tempestpresencehost.h"

//  0x6F46B6B0 - the host's own out-of-line constructor: allocates the
//  +0x0C/+0x1C growable free-list array (via a 0x800-capacity call to a
//  TSGrowableArray-shaped helper) and constructs the four owned 0x20-byte
//  sub-objects at +0x5C/+0x7C/+0x9C/+0xBC.  Not reconstructed this session.
ADDR_THUNK(void* __fastcall ConstructTempestPresenceHost(void* obj), 0x6F46B6B0)

//  0x6F46B790 - the mirror-image destructor: tears down the four owned
//  sub-objects in reverse order, then the free-list array.  Not
//  reconstructed this session.
ADDR_THUNK(void __fastcall DestructTempestPresenceHost(void* obj), 0x6F46B790)

void RebuildTempestPresenceHost()
{
    void* fresh = SMemAlloc(0xDC, "e:\\Drive1\\temp\\buildwar3x\\Engine\\Source\\Tempest/tempest_thread.h", 0x66, 8);

    void* newHost;
    if (fresh)
        newHost = ConstructTempestPresenceHost(fresh);
    else
        newHost = 0;

    if (g_unk6FAB778C)
    {
        DestructTempestPresenceHost(g_unk6FAB778C);
        SMemFree(g_unk6FAB778C, "delete", -1, 0);
    }

    g_unk6FAB778C = newHost;
}
