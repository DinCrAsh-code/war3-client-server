//============================================================================
//  The one global the file cache module has to *define* rather than declare.
//
//  IDA knows the lock only as the symbolic name `CriticalSection` - it named
//  it from some other call tree's use of the same storage - so unlike every
//  other global in this module there is no `dword_<address>` for
//  matching_decomp_injection's build_mix.py binder to resolve, and the
//  funcmap.DATA row has nothing to bind to.  That is the same "unbound
//  private storage" case Net/ostcp_globals.cpp records for g_pTcpManager:
//  a `.mix` built from this takes its *own* lock rather than the shipped
//  one, so **nothing in this module may be armed in mix_config.json until
//  the live address is found.**  Declaring it `extern` instead would simply
//  fail to link.
//============================================================================
#include <windows.h>

CRITICAL_SECTION g_fileCacheLock;
