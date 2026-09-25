//============================================================================
//  gamemain-batch-3: two tail-call member forwards (docs/msvc-vc8-idioms.md,
//  "`add ecx, <offset>` / `jmp` is a member tail call, not an MI thunk") onto
//  already-reconstructed leaves, plus the CDataRecycler::Purge one.
//============================================================================
#include "event.h"
#include "../Net/cdatarecycler.h"

//----------------------------------------------------------------------------
//  0x6F6C5230 - `add ecx, 14h` / `jmp Event::Set` (0x6F6D8520).  The
//  receiver's real class is not otherwise reached by this call tree (only
//  this one embedded Event member is ever touched through it), so it is
//  named for the forward it makes rather than for an invented owning class.
//----------------------------------------------------------------------------
void __fastcall SignalEmbeddedEventAt0x14(void* obj)
{
    ((Event*)((char*)obj + 0x14))->Set();
}

//----------------------------------------------------------------------------
//  0x6F631820 - `mov ecx, offset dword_6FACEBC0` / `jmp CDataRecycler::Purge`
//  (0x6F4C6820).  dword_6FACEBC0 is a fixed, shipped-binary-owned
//  CDataRecycler instance this call tree reaches only through this one
//  forward.
//----------------------------------------------------------------------------
extern CDataRecycler g_netDataRecyclerB3;   // dword_6FACEBC0

void PurgeNetDataRecyclerB3()
{
    g_netDataRecyclerB3.Purge();
}
