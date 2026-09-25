//============================================================================
//  Storage for the one OsTcp global matching_decomp_injection's binder cannot
//  bind, kept in its own translation unit so no reconstructed function's
//  listing changes.
//
//  Every other global this module reads has a `dword_`/`flt_` address in the
//  dump, so build_mix.py resolves it to the live one in game.dll and emits
//  `PUBLIC <name> = 0x6FADAxxx` for it.  `g_pTcpManager` does not: IDA shows
//  it only by the symbolic name `lpAddend` (it named the storage from some
//  other call tree's Interlocked* use of it) with no numeric address anywhere
//  in this dump, so there is nothing to resolve and the reference comes out of
//  the .mix link unresolved.
//
//  Defining it here makes the link succeed.  It does **not** make it correct:
//  this is private storage in the injected DLL, permanently null, not the
//  game's own manager pointer.  The consequence is bounded and stated in
//  tools/funcmap.py's DATA table next to the entry - `OsTcpPump` (0x6F6E49A0)
//  reads it, so `OsTcpPump` must stay out of `enabled_hooks` until the live
//  address is found.  Hooked as it stands it would see a null manager and
//  Sleep() instead of pumping the network, which is a behaviour change, not a
//  crash - but a behaviour change in the one function whose whole job is
//  choosing between those two.
//
//  **If the live address is ever found**, add it to the DATA table as a
//  `dword_6FADAxxx` entry and delete this file: the binder would then emit its
//  own definition and the two would collide.
//============================================================================
#include "ostcp.h"

OsNet::TCPMGR* g_pTcpManager;
