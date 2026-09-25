//============================================================================
//  Naked redirects for the .\AsyncFile.cpp callees this batch declares and
//  calls but does not reconstruct.  Argument counts are the shipped call
//  sites' own push counts; everything in this binary is callee-cleanup, so a
//  call site that pushes nothing and does not adjust esp can only be calling
//  something that pops nothing.  None has a dump body in this tree, so
//  tools/thunk_abi_audit.py reports each as "no dump body for the target" -
//  unknown, not clean.
//
//      0x6F7E8040  ecx = pool, no push               -> retn 0
//      0x6F4C68F0  ecx = pool, four pushes           -> retn 10h
//      0x6F7E8200  ecx = block, no push              -> retn 0
//      0x6F7E79C0  ecx = lock, one push              -> retn 4
//      0x6F6E2B40  ecx = path, edx = callback, two pushes -> retn 8
//      0x6F6DF230  ecx = conn, five pushes           -> retn 14h
//
//  0x6F6DB280 was here too; it is a real body now, in
//  src/Os/asyncfiletransportshutdown.cpp, under the same name.
//      0x6F7E87E0  the transport's completion callback; its address is only
//                  ever taken, never called from here
//============================================================================
#include "asyncfile.h"

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

ADDR_THUNK(ASYNCFILEOP* ASYNCFILEOPPOOL::Alloc(), 0x6F7E8040)
ADDR_THUNK(void ASYNCFILEOPPOOL::Free(void*, unsigned int, const char*, int), 0x6F4C68F0)
ADDR_THUNK(ASYNCFILE* __fastcall AsyncFileConstruct(void*), 0x6F7E8200)
ADDR_THUNK(ASYNCFILE* __fastcall AsyncFileRegister(CRITICAL_SECTION*, ASYNCFILE*), 0x6F7E79C0)
ADDR_THUNK(void __fastcall OsNetOpenAsyncFile(const char*, void*, ASYNCFILE*, int), 0x6F6E2B40)
ADDR_THUNK(int __fastcall NetConnSubmitAsyncOpReal(void*, int, int, int, int, unsigned int), 0x6F6DF230)
ADDR_THUNK(void __stdcall AsyncFileOpenComplete(), 0x6F7E87E0)
