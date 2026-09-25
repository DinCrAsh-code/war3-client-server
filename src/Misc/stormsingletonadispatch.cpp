//============================================================================
//  0x6F6F3520 - reached from CItem's own vtable BFS (batch 8 of 8, see
//  docs/targets/CItem-callee-batch8.md) via several callers this closure
//  does not name (sub_6F002380, sub_6F701D90, sub_6F7120F0, sub_6F7122A0),
//  none of which establish the receiver's real class - only that it has a
//  pointer field at +4, read here and forwarded whole.
//
//  `mode == -1` builds a comma-joined key list (BuildMiscDatabaseKeyList,
//  0x6F71AB10, thunked in citemclosure_batch8_gridleaves.cpp - its own
//  register-argument shape is what identified this call site's `__fastcall`
//  first two arguments) and always reports success; any other `mode` does a
//  single lookup-and-report (MiscDatabaseLookupAndReport, 0x6F4D2080,
//  already a THUNK elsewhere) and - matching the shipped stream exactly -
//  never sets a return value of its own for that path, so this reads
//  whatever `MiscDatabaseLookupAndReport` last left in `eax`.  `void`
//  return there is honest about that call's own contract but leaves this
//  function's own return value on that path unspecified by source, which is
//  exactly the shipped behaviour: nothing in this closure's callers reads
//  it.  `__thiscall`, `retn 14h` (5 stack dwords).
//============================================================================

void __fastcall BuildMiscDatabaseKeyList(int, int, int, int, int);
void __stdcall MiscDatabaseLookupAndReport(int, int, int, int);

struct SMiscDatabaseDispatchThunk
{
    int __thiscall LookupOrReportKey(int a0, int a4, int a8, int aC, int mode);
    char m_reserved00[4];
    int  m_field4;
};

int __thiscall SMiscDatabaseDispatchThunk::LookupOrReportKey(
        int a0, int a4, int a8, int aC, int mode)
{
    if (mode == -1)
    {
        BuildMiscDatabaseKeyList(a0, a4, a8, aC, m_field4);
        return 1;
    }

    MiscDatabaseLookupAndReport(mode, aC, a8, a4);
    return 0;
}
