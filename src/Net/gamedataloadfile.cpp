//============================================================================
//  0x6F535410 - open `path` and load the whole stream out of it.
//
//  Two of this object's own virtuals bracket the load: slot 7 clears
//  whatever it already held, slot 8 is only reached when the load succeeded.
//  On success the path is kept in the object's own 0x104-byte buffer at
//  +0x240 - the game data remembers where it came from.
//============================================================================
#include "netgameload.h"
#include "gamedatawriter.h"
#include "os.h"          // Storm_501


//  Slots 7 and 8 of the game data's own vtable, reached as hand-written
//  dispatches: nothing in these dumps gives the class a vtable to declare.
typedef void (__thiscall* GameDataResetFn)(void* self);
typedef void (__thiscall* GameDataCommitFn)(void* self);

int SGameDataLoad::LoadFile(const char* path)
{
    void* self = this;

    NetGameLoadFile file(path);

    ((GameDataResetFn)(*(void***)self)[0x1C / 4])(self);

    int err = LoadBlocks((NetGameLoadSource*)&file);

    if (err == 0)
    {
        ((GameDataCommitFn)(*(void***)self)[0x20 / 4])(self);
        Storm_501((char*)self + 0x240, path, 0x104);
    }

    return err == 0;
}
