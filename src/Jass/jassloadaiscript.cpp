//============================================================================
//  0x6F2CFD10 - load a map's AI script through the shared file cache,
//  falling back to "Scripts\\<name>" the same way the JASS `.j` reader's
//  own sub_6F3B1970 does for `common.j`/`blizzard.j`/the map's own script
//  (this round's own root), and posting a system message when neither path
//  resolves.  `LoadCachedFile`/`Storm_501`/`Storm_503`/`Storm_578`/
//  `PostSysMessage` are all already-reconstructed callees
//  (Render/filecacheload.cpp, Os/os.h, Player/playerwar3_appendsummaryline.cpp,
//  Misc/sysmessagepost.cpp) reached exactly as their own files declare them.
//============================================================================
#include "filecache.h"   // LoadCachedFile
#include "os.h"          // Storm_501, Storm_503

int __cdecl Storm_578(char* dest, unsigned int size, const char* format, ...);
int __fastcall PostSysMessage(const char* message, int type);

void* __stdcall LoadAiScriptOrWarn(const char* name)
{
    void* buffer;
    int length;

    if (LoadCachedFile(name, &buffer, &length, 1))
        return buffer;

    char combinedPath[0x100];
    Storm_501(combinedPath, "Scripts\\", 0x100);
    Storm_503(combinedPath, name, 0x100);

    if (LoadCachedFile(combinedPath, &buffer, &length, 1))
        return buffer;

    char message[0x100];
    Storm_578(message, 0x100, "MAP DESIGNER: AI script '%s' not found!", name);
    PostSysMessage(message, 4);
    return 0;
}
