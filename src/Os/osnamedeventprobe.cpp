//============================================================================
//  0x6F6CDEE0/0x6F6CE090/0x6F6CE0A0 - one small module, all reached out of
//  GameMain's own init sequence (Game/gamemaininitsequence.cpp).  Address
//  order in the binary does not match dump/call order here; kept together
//  because all three are within one 0x1F0-byte span.
//============================================================================
#include <windows.h>

//----------------------------------------------------------------------------
//  0x6F6CDEE0 - a plain global store, no computation.  The global itself is
//  already declared and named by Window/wininputbridge.h - g_inactivitySleepMs
//  (dword_6FAD1A94), the input pump's own idle-sleep duration
//  (Window/wininputpump.cpp).  This tree's one call site passes a literal
//  0x32 (50, milliseconds).
//----------------------------------------------------------------------------
extern int g_inactivitySleepMs;   // 0x6FAD1A94

void __fastcall SetInactivitySleepMs(int milliseconds)
{
    g_inactivitySleepMs = milliseconds;
}

//----------------------------------------------------------------------------
//  0x6F6CE090 - a bare CreateEventA(0, TRUE, FALSE, name) wrapper: manual
//  reset, initially non-signalled, no error handling at all.
//----------------------------------------------------------------------------
HANDLE __fastcall OsCreateManualEvent(const char* name)
{
    return CreateEventA(0, TRUE, FALSE, name);
}

//----------------------------------------------------------------------------
//  0x6F6CE0A0 - the same CreateEventA, but used purely as an
//  already-running probe: on success it always closes the handle right
//  back and reports whether GetLastError() was ERROR_ALREADY_EXISTS
//  (0xB7); on failure it reports 0 (CreateEventA's own NULL return, never
//  overwritten before the early `retn`).
//----------------------------------------------------------------------------
BOOL __fastcall OsProbeNamedEventExists(const char* name)
{
    HANDLE handle = CreateEventA(0, TRUE, FALSE, name);
    if (!handle)
        return FALSE;

    BOOL alreadyExisted = (GetLastError() == ERROR_ALREADY_EXISTS);
    CloseHandle(handle);
    return alreadyExisted;
}
