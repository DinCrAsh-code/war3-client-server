//============================================================================
//  0x6F3BAA20 - MakeJassStringHandleFromCString: null-check a C string, then
//  intern it into the shared hashed-string table (CItemSlotSingletonHost::
//  InternCString, 0x6F39F5F0, itemhandlemain.h) for its JASS string handle.
//
//  Shared by two natives in this batch - GetSaveBasicFilename
//  (jasssavebasicfilename.cpp) and GetEventPlayerChatStringMatched
//  (jasseventgamestate.cpp) - which is why it gets its own translation
//  unit rather than living beside either caller.
//============================================================================
#include "jassnatives.h"
#include "itemhandlemain.h"

int __fastcall MakeJassStringHandleFromCString(const char* str)
{
    //  The singleton is fetched unconditionally, even on the null-string
    //  exit that never uses it - the shipped `mov ecx,g_unk6FAB65F4` sits
    //  ahead of the branch, not inside it.
    CItemSlotSingletonHost* host = g_pItemSlotHost;

    if (!str)
        return 0;

    return (int)host->InternCString(str);
}
