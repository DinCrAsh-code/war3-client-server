//============================================================================
//  Storage for the three OsNet globals matching_decomp_injection's binder
//  cannot bind, for exactly the reason ostcp_globals.cpp already gives for
//  g_pTcpManager: IDA shows them only by the symbolic names `hModule`,
//  `hLibModule` and `WSAData`, with no numeric address anywhere in these
//  dumps, so build_mix.py has nothing to resolve and the reference comes out
//  of the .mix link unresolved.
//
//  Defining them here makes the link succeed and does **not** make them
//  correct: this is private storage in the injected DLL, permanently zero,
//  not the game's own module handles.  The consequence is bounded and stated
//  in the target write-up's arm-with-caution list - the two functions that
//  read them are OsNetWinsockStartup (0x6F6D9FB0) and OsNetWinsockShutdown
//  (0x6F6E40C0), so neither may be armed until the live addresses are found.
//  Hooked as they stand, the startup would re-LoadLibrary both DLLs into a
//  private slot the game's own shutdown never frees, and the shutdown would
//  FreeLibrary nothing.
//
//  **If the live addresses are ever found**, add them to funcmap.py's DATA
//  table as `dword_6FADAxxx` entries and delete this file: the binder would
//  then emit its own definitions and the two would collide.
//============================================================================
#include "ostcp.h"

HMODULE g_hMswsock;
HMODULE g_hWs2_32;
WSADATA g_wsaData;
