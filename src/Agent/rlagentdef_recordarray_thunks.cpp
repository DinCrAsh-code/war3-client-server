//============================================================================
//  Naked real-signature thunks for CRlAgentDef::Save/Load's own declared-
//  but-not-defined record-array callees (SaveRecordArray/LoadRecordArray -
//  see rlagentdef_save.cpp/rlagentdef_load.cpp's own header comments on
//  why they were left declared only). link_check.py finds the missing
//  definition the moment anything actually calls them; same fix as
//  basemap_outofscope_thunks.cpp.
//============================================================================

__declspec(naked) void __fastcall SaveRecordArray(void*, void*)
{
    __asm { mov eax, 06F4A4850h
            jmp eax }
}

__declspec(naked) void __fastcall LoadRecordArray(void*, void*)
{
    __asm { mov eax, 06F4A4900h
            jmp eax }
}
