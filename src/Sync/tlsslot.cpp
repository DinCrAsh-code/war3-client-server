//============================================================================
//  0x6F4C34D0 - one slot of this thread's pointer array.
//
//  Storm hands out a single TLS index; the value stored in it is an array of
//  subsystem pointers, and every subsystem knows its own slot number.  A
//  thread that was never registered has no array at all, which is a null
//  return rather than a fault.
//============================================================================
#include "gamecontext.h"

//  0x6F6D98E0, in src/tls.cpp - the checked TlsGetValue.  Declared here rather
//  than in a header because that is the only thing this file needs from it.
void* __fastcall GetThreadLocalChecked(unsigned int tlsIndex);

//  The process-wide TLS index Storm allocated for the array.
extern unsigned int g_dwTlsIndex;

void* __fastcall GetThreadLocalSlot(unsigned int slot)
{
    //  The success case is the fall-through, as in the shipped code.
    void** slots = (void**)GetThreadLocalChecked(g_dwTlsIndex);
    if (slots != 0)
        return slots[slot];
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4C34F0 - the writer: same lookup, and a no-op (not a fault) on an
//  unregistered thread, exactly as the reader is.
//----------------------------------------------------------------------------
void __fastcall SetThreadLocalSlot(unsigned int slot, void* value)
{
    void** slots = (void**)GetThreadLocalChecked(g_dwTlsIndex);
    if (slots != 0)
        slots[slot] = value;
}

//----------------------------------------------------------------------------
//  0x6F4C34B0 - install this thread's slot array at Storm's own TLS index.
//  Moved here from Sync/tls.cpp (gamemain-batch-1's original placement):
//  its own address falls in this file's 0x6F4C34D0/0x6F4C34F0 neighbourhood,
//  not tls.cpp's 0x6F6D98E0/0x6F6D9910 one, so it belongs in this TU under
//  CLAUDE.md's own "one TU per original module" rule - and putting it here
//  is also what makes it match: TlsSetValueRaw (0x6F6D9910, tls.cpp) is only
//  *declared* in this TU, never defined, so this compiler cannot inline it
//  away the way it did when both functions shared tls.cpp (scored 0.000,
//  the call collapsing straight through to `TlsSetValue` with two pushed
//  stack args instead of the tail `jmp` the dump shows). The one argument
//  arrives in ecx; the tail jump into TlsSetValueRaw is why this function
//  has no `retn` of its own in the dump.
//----------------------------------------------------------------------------
int __fastcall TlsSetValueRaw(unsigned int tlsIndex, void* value);   // tls.cpp, 0x6F6D9910 (BOOL there; plain int here, gamecontext.h pulls in no <windows.h>)

int __fastcall SetThreadLocalArray(void* array)
{
    return TlsSetValueRaw(g_dwTlsIndex, array);
}
