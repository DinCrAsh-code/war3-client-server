//============================================================================
//  CUnit batch-14 (slot-3 closure, off the message dispatcher 0x6F2A7E60).
//
//  0x6F0283A0 - a two-instruction constant getter: no `this`, no
//  arguments, `retn 0`.  Returns the literal 0x41726F6F.  Same shape as
//  GetFixedAgileTypeId_58646973 (unit_fixedtypeidgetter.cpp) - a fixed
//  four-char code, not otherwise identified from this call tree (its own
//  callers, off RefreshPositionNotifyState's closure - already thunked as
//  NotifyPositionStateChange, unit_slotthunks.cpp - do not say what it
//  gates).
//============================================================================
unsigned int GetFixedConstant_41726F6F()
{
    return 0x41726F6F;
}
