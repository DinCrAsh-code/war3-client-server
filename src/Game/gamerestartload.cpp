//============================================================================
//  0x6F3A4C70 - SWorldRestart::RequestLoad.  `retn 0Ch`.
//
//  Work out what `path` names, put it in the world's map slot, and announce
//  the request.  A `.w3z` is a saved game and everything else is a map, and
//  that one test decides both which of two lookups runs and whether the
//  session keeps its slots.
//
//  `m_mapPath` is spelled out at every use rather than hoisted into a local:
//  the shipped code re-forms `lea ecx,[esi+2F0h]` at each of its five call
//  sites in the first half and only holds it in a register across the second
//  (docs/msvc-vc8-idioms.md, "A pointer built once and reused across an
//  `if`/`else` is not what the shipped source did").
//
//  The lookup is tried twice, and the second try is the interesting part: if
//  the path as given resolves to nothing, the whole thing is retried with
//  the module's own directory prefixed.  The retry is guarded by
//  `RCString::EqualsCStr(slot, 0)` - the map slot is still *empty* - rather
//  than by a flag, so the two halves share no state at all.
//
//  A map is looked up twice per attempt as well: an archive lookup first
//  (Storm ordinal 266) and then the same thing with a leading null argument
//  (ordinal 293), which is what lets a loose file on disk stand in for one
//  inside an archive.  The handle each opens is closed immediately (ordinal
//  252) - the lookup is only ever asking "does this exist".
//
//  `m_endReason` is this family's "a load is pending" word (gamerestart.h),
//  and it is set to *whether reloading is currently un-suppressed*, not to
//  1: `[+0x3C8] == 0`.  Every one of the four arms that manages to fill the
//  map slot writes it the same way, which is why it is spelled out four
//  times rather than once at the bottom.
//============================================================================
#include "gamerestart.h"
#include "savegame.h"
#include "os.h"                 // Storm_506, Storm_503, OsGetModuleDirectory
#include "rcstring.h"
#include "gameuithunks.h"       // SPathField::Set, 0x6F01FCB0
#include "jassnatives.h"        // STStringField::Assign, 0x6F4C5CF0

int __stdcall Storm_508(const char* a, const char* b, unsigned int max);

//  The three Storm file-lookup ordinals this function uses.  Named by
//  ordinal: nothing in these dumps says what they are beyond their shapes -
//  266 and 293 both answer "does this name resolve, and here is a handle",
//  293 with one more leading argument, and 252 closes what either opened.
int  __stdcall Storm_266(const char* name, unsigned int a, unsigned int b, void** outHandle);
int  __stdcall Storm_293(int first, const char* name, unsigned int a, unsigned int b, void** outHandle);
int  __stdcall Storm_252(void* handle);

void SWorldRestart::RequestLoad(const char* path, int showScoreScreen, int keepSlots)
{
    SWorldRestart* self = this;

    if (path == 0 || path[0] == 0)
        return;

    if (showScoreScreen == 0)
        GameDataDiscardTempFile();

    unsigned int length = Storm_506(path);

    int isSaveGame = 0;
    if (length > 4)
    {
        const char* extension = path + length - 4;
        if (extension != 0 && Storm_508(extension, ".w3z", 0x7FFFFFFF) == 0)
            isSaveGame = 1;
    }

    self->m_pathIsSaveGame = isSaveGame;
    self->m_showScoreScreen = showScoreScreen;
    self->m_loadKeepsSlots = (isSaveGame != 0 && keepSlots != 0) ? 1 : 0;

    if (isSaveGame == 0)
    {
        void* handle;
        if (Storm_266(path, 0x10, 4, &handle) != 0)
        {
            Storm_252(handle);
            ((STStringField*)self->m_mapPath)->Assign(path);
        }
        else if (Storm_293(0, path, 0x10, 4, &handle) != 0)
        {
            Storm_252(handle);
            ((SPathField*)self->m_mapPath)->Set(path);
        }
        else
        {
            goto retry;
        }

        self->m_endReason = (WorldSuppressReload(self) == 0);
    }
    else
    {
        CSaveGame save;
        SaveGameConstruct(&save);

        if (save.ReadHeader(path))
        {
            ((STStringField*)self->m_mapPath)->Assign(path);
            self->m_endReason = (WorldSuppressReload(self) == 0);
        }

        SaveGameDestruct(&save);
    }

retry:
    //  Only when the slot is still empty: everything below is the same four
    //  arms again over the module directory's own copy of the name.
    if (((RCString*)self->m_mapPath)->EqualsCStr(0) == 0)
    {
        PostReloadRequestedEvent(self->m_eventHost);
        return;
    }

    {
    char fileName[0x104];
    OsGetModuleDirectory(fileName, 0x104);
    Storm_503(fileName, path, 0x104);

    if (self->m_pathIsSaveGame == 0)
    {
        void* handle;
        if (Storm_266(fileName, 0x10, 4, &handle) != 0)
        {
            Storm_252(handle);
            ((STStringField*)self->m_mapPath)->Assign(fileName);
            self->m_endReason = (WorldSuppressReload(self) == 0);
        }
        else if (Storm_293(0, fileName, 0x10, 4, &handle) != 0)
        {
            Storm_252(handle);
            ((STStringField*)self->m_mapPath)->Assign(fileName);
            self->m_endReason = (WorldSuppressReload(self) == 0);
        }
    }
    else
    {
        CSaveGame save;
        SaveGameConstruct(&save);

        if (save.ReadHeader(fileName))
        {
            ((STStringField*)self->m_mapPath)->Assign(fileName);
            self->m_endReason = (WorldSuppressReload(self) == 0);
        }

        SaveGameDestruct(&save);
    }
    }

    PostReloadRequestedEvent(self->m_eventHost);
}
