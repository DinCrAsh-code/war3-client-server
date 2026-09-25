//============================================================================
//  0x6F6DB280 - dispatch an async file's transport through its own vtable
//  slot 9 (+0x24), which is what shuts the transport down.
//
//  Its own TU: AsyncFileClose calls it for real, and it is the only caller in
//  the image (`ida_query xrefs 0x6F6DB280` reports exactly one).
//
//  Was a naked redirect in src/Os/asyncfilethunks.cpp until this session;
//  the name is the one that redirect already carried, so no call site moved
//  and no shipped address acquired a second C++ name.
//============================================================================
#include "asyncfile.h"

//  Storm.dll ordinal 465, spelled as the rest of the repo spells it - see
//  src/Sync/tls.cpp for why it is not `extern "C"`.
void __stdcall SErrSetLastError(unsigned int error);

//----------------------------------------------------------------------------
//  The null guard reports through Storm's error channel and returns nothing;
//  the live path is a tail call, so whatever slot 9 answers is this
//  function's answer.  Nothing reads it - AsyncFileClose discards it - which
//  is why the declaration is `void` and the shipped tail is `jmp edx`.
//----------------------------------------------------------------------------
void __fastcall NetConnDispatchSlot0x24(void* conn)
{
    if (conn == 0)
    {
        SErrSetLastError(0x57);     // ERROR_INVALID_PARAMETER
        return;
    }

    typedef void (__thiscall *ShutdownFn)(void*);
    ((ShutdownFn)(*(void***)conn)[0x24 / 4])(conn);
}
