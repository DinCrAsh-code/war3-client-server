//============================================================================
//  The game-data stream's writer face - the two members 0x6F535D70 forwards
//  into, plus 0x6F534CE0's base construction.  Declared in a header so the
//  definition in gamedatasmalls.cpp and the redirects in
//  Game/gamerestartthunks.cpp cannot drift into two mangled names.
//============================================================================
#ifndef GAMEDATAWRITER_H
#define GAMEDATAWRITER_H

struct SGameDataWriter
{
    //  0x6F5349B0 / 0x6F535A90 - neither is in this tree; redirected.
    int  WriteRecord(int a, int b, int c, int d);
    void Abandon();

    //  0x6F535D70.
    void WriteOrAbandon(int a, int b, int c, int d);
};

struct SGameDataBase
{
    //  0x6F534CE0 - not in this tree; redirected.
    void Construct(int capacity, int flags);
};

#endif
