//============================================================================
//  0x6F3C1F80 - `GetSaveBasicFilename` "()S".  Own translation unit: the
//  0x6F3C1F80 neighbourhood, calling nothing else in this batch.
//
//  Extract the save-basic-filename event data's own path (its +0x20 field,
//  read the ordinary GetHandleOrZero way), strip it down to the last path
//  component (ExtractPathTail, 0x6F6C5A00, Net/netdata_replayfilenamemunge
//  .cpp), drop the extension if it has one, and hand the result to
//  MakeJassStringHandleFromCString.
//
//  Carries a VC8 /GS stack cookie around its own local buffer in the
//  shipped image - this build's fixed /GS- cannot reproduce that guard, the
//  same gap docs/msvc-vc8-idioms.md and netdata_loadreplayheader.cpp's own
//  header comment already record for every stack-array function in this
//  codebase.
//============================================================================
#include "jassevents.h"
#include "jassnatives.h"    // MakeJassStringHandleFromCString
#include "handleobject.h"
#include "os.h"             // Storm_570

//  0x6F6C5A00 - Net/netdata_replayfilenamemunge.cpp.
void __fastcall ExtractPathTail(const char* src, char* dest, unsigned int size);

int GetSaveBasicFilename()
{
    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) != 0x80303)
        return 0;

    CScriptEventData* data = GetCurrentScriptEventData();
    if (!data)
        return 0;

    const char* path =
        (const char*)GetHandleOrZeroAlias((SHandleHolder*)&data->m_ownerRef);
    if (!path || !*path)
        return 0;

    char buffer[0x104];
    ExtractPathTail(path, buffer, 0x104);

    char* dot = Storm_570(buffer, '.');
    if (dot)
        *dot = 0;

    return MakeJassStringHandleFromCString(buffer);
}
