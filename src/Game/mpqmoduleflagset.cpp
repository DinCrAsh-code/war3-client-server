//============================================================================
//  0x6F0010B0 - remember one module flag globally, mirror it into a second
//  module's own global (0x6F4BC700, outside this batch's own 43), and tell
//  Storm ordinal 263 whether it is set (3) or not (0).
//============================================================================

extern int g_mpqModuleFlag;   // dword_6FAAE2AC

//  0x6F4BC700 is SetSingleCellB3 (Misc/gamemainslottable.cpp,
//  gamemain-batch-3's own real reconstruction of this address - a bare
//  `mov [global],ecx` store into a different module's own global).  This
//  file used to carry a second, naked redirect to the same address under
//  the name MirrorMpqModuleFlag - a duplicate; the call site below now
//  goes through the real reconstruction instead.
void __fastcall SetSingleCellB3(unsigned int value);   // gamemainslottable.cpp, 0x6F4BC700

void __stdcall Storm_263(int flag);

void __fastcall SetMpqModuleFlag(int value)
{
    g_mpqModuleFlag = value;
    SetSingleCellB3(value);
    Storm_263(g_mpqModuleFlag != 0 ? 3 : 0);
}
