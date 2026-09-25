//============================================================================
//  0x6F6D98E0 - a Storm-managed thread-local slot fetch that treats a
//  Windows-reported error on the get as failure even though TlsGetValue's own
//  return value cannot distinguish "not set" from "failed": if
//  GetLastError() is still nonzero right after the call, the slot was never
//  initialised for this thread, and the function reports that through Storm's
//  own error channel (0x57 = ERROR_INVALID_PARAMETER, the code Storm raises
//  for "not initialised on this thread") instead of quietly returning
//  whatever TlsGetValue happened to leave in eax.
//============================================================================
#include <windows.h>

//  Storm.dll ordinal 465.  Declared under its readable name, like the rest
//  of the Storm imports in this repo (see src/storm.h): tools/funcmap.py
//  maps the dump's `Storm_465` onto it, and that mapping only bites if both
//  sides spell it the same way.  Not `extern "C"`, for the same reason
//  storm.h's SMem* are not: the pipeline reads readable names out of the
//  comment MASM puts on a mangled EXTRN, and an undecorated C symbol has
//  no such comment, so the call canonicalises to an anonymous SYM and the
//  function scores one instruction short for no real reason.
void __stdcall SErrSetLastError(unsigned int code);

void* __fastcall GetThreadLocalChecked(unsigned int tlsIndex)
{
    void* value = TlsGetValue(tlsIndex);
    if (GetLastError() != 0)
    {
        SErrSetLastError(0x57);
        return 0;
    }
    return value;
}

//============================================================================
//  0x6F6D9910 - the setter GetThreadLocalChecked has no counterpart for:
//  a plain TlsSetValue passthrough, no error check.  Natural argument
//  order (index, then value) - both arrive in registers and both are just
//  forwarded, so there is nothing here for a check to catch anyway.
//============================================================================
BOOL __fastcall TlsSetValueRaw(unsigned int tlsIndex, void* value)
{
    return TlsSetValue(tlsIndex, value);
}
