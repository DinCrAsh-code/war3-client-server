//============================================================================
//  0x6F7370B0 - the same "try the raw name, then fall back to
//  Scripts\\<name>" shape jscript-bin1's own root (sub_6F3B1D00's
//  0x6F3B1970) uses for `common.j`/`blizzard.j`/the map's own script, here
//  over `Storm_279` (Game/gamemainfatalmessage.cpp's own declared
//  signature) rather than `LoadCachedFile` - a different loader for a
//  different kind of file, same fallback idiom.  `sub_6F7367E0` (out of
//  this session's own closure) is the error path: `ecx`/`edx` carry a
//  fixed format ("%s: %s") and key ("WESTRING_ERROR_CANTOPEN"), with the
//  file name as its one stack argument - declared exactly as this call
//  site pushes it and left undefined.
//============================================================================
#include "os.h"   // Storm_501, Storm_503

int __stdcall Storm_279(const char* filename, void** buffer, int* length,
                        int extraBytes, void* callback);

//  0x6F7367E0 - out of this session's own closure; declared exactly as
//  this call site pushes it (fmt/key in ecx/edx, the file name as one
//  stack argument) and given a real naked-redirect body here so the .mix
//  actually links.
__declspec(naked) void __fastcall sub_6F7367E0(const char* /*fmt*/,
                                                const char* /*key*/,
                                                const char* /*arg*/)
{
    __asm { mov eax, 06F7367E0h }
    __asm { jmp eax }
}

void* __fastcall LoadJassLikeScriptOrWarn(const char* name)
{
    void* buffer;
    int length;

    if (Storm_279(name, &buffer, &length, 1, 0))
        return buffer;

    char combinedPath[0x108];
    Storm_501(combinedPath, "Scripts\\", 0x7FFFFFFF);
    Storm_503(combinedPath, name, 0x7FFFFFFF);

    if (Storm_279(combinedPath, &buffer, &length, 1, 0))
        return buffer;

    sub_6F7367E0("%s: %s", "WESTRING_ERROR_CANTOPEN", name);
    return 0;
}
