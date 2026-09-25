//============================================================================
//  0x6F5BCAC0 - the font the chat window should use: the one the player's
//  own preference picks, and the skin's "MasterFont" when that preference
//  names nothing.
//
//  Preference 0x4E is read through the store's own GetIntValue, whose `out`
//  is left uninitialised on a miss exactly as the shipped code leaves it -
//  it is the value that is then handed to the lookup either way.
//
//  Own translation unit: four real calls out of it.
//============================================================================
#include "gameui.h"
#include "preferences.h"

//  0x6F5BCA50 - GameUI/chatfontvalue.cpp.
int __fastcall GetChatFontValue(unsigned int index, char* buffer,
                                unsigned int size);
//  Storm ordinal 501 - copy into a bounded buffer.  Spelled exactly as
//  Config/gamestrings.h declares it.
void __stdcall Storm_501(char* dest, const char* src, unsigned int size);

//: aMasterfont - the skin key the fallback reads.
static const char kMasterFontKey[] = "MasterFont";

void __fastcall GetChatFontName(char* buffer, unsigned int size)
{
    //  The singleton named first: the shipped code issues its call before
    //  it forms either argument, which is the order a named intermediate
    //  gives and a single nested expression does not.
    int index;
    CPreferencesWar3* preferences = GetPreferences();
    preferences->GetIntValue(0x4E, &index);

    if (GetChatFontValue((unsigned int)index, buffer, size) != 0)
        return;

    Storm_501(buffer, ResolveSkinValue(kMasterFontKey, 0), size);
}
