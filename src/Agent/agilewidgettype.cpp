//============================================================================
//  0x6F2C6140 - GetWidgetAgileTypeId: the four-character agile type id
//  CWidget's vtable slot 68 looks up, 'hgw+' (0x2B776768), in the same
//  '+'-terminated family as kAgileRootTypeId's 'aga+' (agiletype.h).
//
//  Own translation unit: three separate call sites in two other files reach
//  it with a real `call`, which two instructions would never survive
//  sharing a file with any of them.
//============================================================================

unsigned int GetWidgetAgileTypeId()
{
    return 0x2B776768;
}
