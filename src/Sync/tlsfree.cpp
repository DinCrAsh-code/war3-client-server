//============================================================================
//  0x6F6D98D0 - FreeThreadLocalIndex (gamemain-batch-3).  A one-call
//  TlsFree() forward, `__fastcall` so the index arrives in ecx the same way
//  GetThreadLocalChecked's does (tls.cpp).
//============================================================================
#include <windows.h>

void __fastcall FreeThreadLocalIndex(unsigned int tlsIndex)
{
    TlsFree(tlsIndex);
}
