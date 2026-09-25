//============================================================================
//  0x6F4434D0 - is `path` a saved game this build can read?
//
//  Three things have to hold: its header reads, its magic matches (which
//  0x6F537F10 answers, falling back to the literal 'WAR3' when the header
//  carries no expected value), and its format version is no newer than the
//  one the caller will accept.  The second of the header's five fields is
//  read into a local nothing looks at - the peek takes all five or none.
//============================================================================
#include "netgameload.h"

//  0x6F537F10 - Net/gamedatamagic.cpp.
int __fastcall GameDataMagicMatches(unsigned int magic);

int __fastcall SaveGameHeaderProbe(const char* path, unsigned int maxVersion)
{
    unsigned int magic;
    unsigned int unused;
    unsigned int version;

    int ok = GameDataPeekHeader(path, &magic, &unused, &version, 0, 0);

    if (GameDataMagicMatches(magic) != 0 && ok != 0 && version <= maxVersion)
        return 1;

    return 0;
}
