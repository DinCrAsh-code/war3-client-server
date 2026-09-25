//============================================================================
//  gamemain-batch-3: two trivial accessors, dword_6FAB78FC (a single global
//  cell) and dword_6FAAE370 (an array also written by GameMain's own root
//  body at 0x6F009A21, `dword_6FAAE370[edi*4] = ecx` - outside this
//  session's batch, so the array's real element count is not established
//  here; declared with no bound, which this element-by-index read/write
//  shape does not need).
//============================================================================

extern unsigned int g_singleCellB3;   // dword_6FAB78FC
extern void*         g_slotTableB3[]; // dword_6FAAE370

//----------------------------------------------------------------------------
//  0x6F4BC700 - `__fastcall`, one word in ecx, no return.
//----------------------------------------------------------------------------
void __fastcall SetSingleCellB3(unsigned int value)
{
    g_singleCellB3 = value;
}

//----------------------------------------------------------------------------
//  0x6F009910 - `__fastcall`, the index in ecx.
//----------------------------------------------------------------------------
void* __fastcall GetSlotTableEntryB3(unsigned int index)
{
    return g_slotTableB3[index];
}
