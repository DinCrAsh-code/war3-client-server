//============================================================================
//  0x6F33AE40 - `retn`: read the "portraitModelPath" config string (through
//  sub_6F32CFF0, out of this agent's own worklist slice - not owned by any
//  of the eight parallel agents this run, but still a whole additional
//  target's worth of body rather than this one's, so declared only), and
//  if it names anything, tail-jump the current full-detail flag
//  (GetModelDetailFlags(1), 0x6F333760, Widget/modeldetail.cpp) into
//  sub_6F4D95C0 (`retn`, __fastcall(ecx,edx), likewise out of scope and
//  declared only) alongside the path string itself.
//============================================================================
//  0x6F333760 - already reconstructed (Widget/modeldetail.cpp); redeclared
//  here the way every other caller in this codebase does.
int __fastcall GetModelDetailFlags(int wantsFullDetail);

//  0x6F32CFF0 - not this agent's own worklist target this run; declared,
//  not defined.
const char* GetPortraitModelPath();

//  0x6F4D95C0 - likewise out of scope; `retn`, two register arguments,
//  declared, not defined.
void __fastcall ApplyModelDetailToPath(const char* path, int detailFlags);

void ApplyPortraitModelDetail()
{
    const char* path = GetPortraitModelPath();
    if (path && *path)
        ApplyModelDetailToPath(path, GetModelDetailFlags(1));
}

//----------------------------------------------------------------------------
//  Link-completion thunks - both addresses are out of scope for every one
//  of the eight parallel agents this run (see the header comment), so
//  nothing else in the tree defines them either. Naked redirect thunks -
//  the same idiom this repo uses for any other out-of-scope callee - make
//  this TU link without claiming either address as a real reconstruction
//  (no funcmap.py row points at these).
//----------------------------------------------------------------------------
__declspec(naked) const char* GetPortraitModelPath()
{
    __asm { mov eax, 06F32CFF0h }
    __asm { jmp eax }
}

__declspec(naked) void __fastcall ApplyModelDetailToPath(const char*, int)
{
    __asm { mov eax, 06F4D95C0h }
    __asm { jmp eax }
}
