//============================================================================
//  OsTcpPump (0x6F6E49A0) - the one entry point the rest of the game drives
//  this module through.
//
//  Its own translation unit so TCPMGR::Pump stays a real call.
//============================================================================
#include "ostcp.h"

//----------------------------------------------------------------------------
//  0x6F6E49A0 - `__fastcall`: the millisecond budget arrives in ecx and
//  nothing is cleaned off the stack (`retn`, not `retn 4`), which is what a
//  free function with one register argument looks like.  See
//  docs/msvc-vc8-idioms.md, "`__fastcall` is how you get a pointer in ecx".
//
//  With no manager yet - which is the state the module starts in, and the
//  reason this test is here at all - the budget is spent sleeping instead.
//  The push of `ms` is shared between the two paths in the shipped code
//  because both spend the same argument.
//----------------------------------------------------------------------------
void __fastcall OsTcpPump(DWORD ms)
{
    OsNet::TCPMGR* mgr = g_pTcpManager;
    if (mgr != 0)
        mgr->Pump(ms);
    else
        Sleep(ms);
}
