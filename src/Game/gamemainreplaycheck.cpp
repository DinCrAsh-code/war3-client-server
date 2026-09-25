//============================================================================
//  0x6F007020 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp): run the GPU hardware compatibility
//  scan, then check whether command-line switch #5 names a ".w3g" replay
//  file whose header is present and is not the bare "WAR3" magic (0) -
//  storing the (possibly special-cased) hardware profile index into
//  dword_6FA4F6E8 either way.
//============================================================================

//  0x6F009950/0x6F009920 - already reconstructed (Game/gamearchivehwcheck.cpp,
//  Game/mpqhardwareprofileindex.cpp).
void GameArchiveHardwareCheck();
int  __fastcall GetHardwareProfileIndex();

//  0x6F0068C0 - already reconstructed above (Game/gamemainfatalmessage.cpp).
void __fastcall GameMainShowFatalMessage(unsigned int lineCount);

//  0x6F4C19C0/0x6F4C19D0 - already reconstructed
//  (Misc/gamemain_small_wrappers.cpp).  Both take a `const char*` (Storm
//  ordinal 472/474's own parameter) - this call tree's own literal
//  integers (3, 5) are passed straight through uninterpreted, the same way
//  the existing reconstructions never dereference their own argument
//  either.
int   __fastcall ForwardToStorm472(const char* text);
char* __fastcall CacheTextAndReturn(const char* text);

//  Storm ordinal 572 - already declared this shape elsewhere
//  (Render/textureextension.cpp).
const char* __fastcall Storm_572(const char* text, char ch);

//  Storm ordinal 509 - already declared this shape (SStrnICmp).
int __stdcall SStrnICmp(const char* a, const char* b, unsigned int maxLen);

//  0x6F534A70 - already reconstructed (Net/gamedatapeekheader.cpp).
int __fastcall GameDataPeekHeader(const char* path, unsigned int* outWord30,
                                  unsigned int* outWord34, unsigned int* outWord38,
                                  unsigned int* outWord3C, unsigned int* outCrc);

extern int g_rowTableCurrentRow;   // dword_6FA4F6E8

int __fastcall GameMainReplayHardwareCheck()
{
    GameArchiveHardwareCheck();

    int profileIndex = GetHardwareProfileIndex();
    if (profileIndex == -1)
    {
        GameMainShowFatalMessage(profileIndex + 5);
        return 0;
    }

    int switchCount = ForwardToStorm472((const char*)3);
    char* path = CacheTextAndReturn((const char*)5);

    if (path && *path)
    {
        const char* ext = Storm_572(path, '.');
        if (ext && SStrnICmp(ext, ".w3g", 0x7FFFFFFF) == 0)
        {
            unsigned int header;
            if (GameDataPeekHeader(path, &header, 0, 0, 0, 0))
            {
                if (header != 0 && header != 0x57415233)
                {
                    g_rowTableCurrentRow = profileIndex;
                    return 1;
                }
                g_rowTableCurrentRow = 0;
                return 1;
            }
        }
    }

    (void)switchCount;
    g_rowTableCurrentRow = profileIndex;
    return 1;
}
