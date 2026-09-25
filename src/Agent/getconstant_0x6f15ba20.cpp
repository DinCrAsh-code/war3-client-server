//============================================================================
//  0x6F15BA20 - `retn`, no arguments: a bare literal-constant return,
//  `mov eax, 41636172h`.  Named `GetConstant_0x6F15BA20` by an earlier
//  session (cunit_agent5_agentqueries.cpp's own closure reached it
//  first, as a naked thunk).  Given its own translation unit
//  (checksum-round2-G, 2026-09-13) rather than defined in either of its
//  two real callers' own files: it is a one-instruction body, and this
//  build's /Ob2 will inline a same-TU call to something this trivial on
//  sight - which the shipped binary never does (both real call sites are
//  a genuine, separate `call sub_6F15BA20`, not an inlined constant).
//  cunit_agent5_agentqueries.cpp's `QueryAgentSingleton_0x6F239860` calls
//  it twice in one function; Unit/unit_queryvision.cpp's
//  `CUnit::QuerySpecialSelectionMask` once more - splitting it out here
//  is what keeps every one of those three call sites a real `call`
//  rather than a silently-inlined `mov eax, 41636172h` at some of them
//  and not others.
//============================================================================
unsigned int __fastcall GetConstant_0x6F15BA20()
{
    return 0x41636172;
}
