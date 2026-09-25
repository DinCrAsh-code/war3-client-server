//============================================================================
//  0x6F5BCA50 - read the configured font for one chat-font index out of the
//  "ChatFonts" section.
//
//  The key is built by number - "ChatFont" and the index, through Storm's
//  own bounded printf into a 0x100-byte stack buffer - and then looked up
//  as an ordinary configuration string.
//
//  What is not reproduced is the /GS buffer cookie the shipped frame
//  carries around that buffer: the load of dword_6FAAE140, the `xor esp`,
//  the slot store and the check call on the way out, none of which this
//  repo emits at its fixed /GS-.
//
//  Own translation unit: the format and the lookup are both real calls.
//============================================================================
#include "gameui.h"

//  Storm ordinal 578 - bounded printf.  Spelled exactly as
//  gameuisetgamepaused.cpp declares it.
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//  0x6F00A040 - the configuration string reader, `retn 0Ch`: the section in
//  ecx, the key in edx and the buffer, its size and a flag word on the
//  stack.  A redirect for now (Config/configstringthunks.cpp).
int __fastcall GetConfigStringValue(const char* section, const char* key,
                                    char* buffer, unsigned int size,
                                    int flags);

//: off_6FA8C288 / off_6FA8C280 - two pointer globals, not two string
//: constants: the shipped code loads the pointer out of each
//: (`mov eax, off_6FA8C288`) rather than taking a constant's address.
extern const char* g_chatFontKeyPrefix;
extern const char* g_chatFontsSection;

//: aSU - the key format, "<prefix><index>".
static const char kChatFontKeyFormat[] = "%s%u";

int __fastcall GetChatFontValue(unsigned int index, char* buffer,
                                unsigned int size)
{
    char key[0x100];

    Storm_578(key, sizeof(key), kChatFontKeyFormat, g_chatFontKeyPrefix,
              index);

    return GetConfigStringValue(g_chatFontsSection, key, buffer, size, 0);
}
