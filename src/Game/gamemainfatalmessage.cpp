//============================================================================
//  0x6F0068C0 - GameMain's own fatal-startup message box.  Loads
//  "ui\startupstrings.txt", if present, and tokenises up to `lineCount`
//  (capped at 6) lines of it into a message buffer through Storm_504; if
//  nothing was ever written to the buffer (no file, or an empty read),
//  the default "Warcraft III was unable to initialize." text is shown
//  instead.  Reached from GameMainInitSequence (Game/gamemaininitsequence.cpp)
//  and GameMainResetState (Game/gamemainresetstate.cpp), both times with a
//  small literal count.
//============================================================================
#include <windows.h>

extern "C" void* __cdecl memset(void* dest, int value, unsigned int count);

//  Storm ordinal 504 - not otherwise declared in this tree.  Reads one
//  more delimited token out of `*cursor` into `outBuffer` (bufSize-bounded)
//  and advances `*cursor`; the shipped signature order is read off this
//  call site's own push sequence.
int __stdcall Storm_504(char** cursor, char* outBuffer, unsigned int bufSize,
                        const char* delimiters, int flag);

//  Storm ordinal 279/280 - already declared this shape (Render/filecache.h).
int  __stdcall Storm_279(const char* filename, void** buffer, int* length,
                         int extraBytes, void* callback);
int  __stdcall Storm_280(void* buffer);

//  0x6F6BAD70 - already reconstructed elsewhere (GetSelectedWindow).
HWND __fastcall GetSelectedWindow(unsigned int which);

//  0x6F6C2620 - out of this session's scope (gamemain-batch-2 owns it); a
//  message-box-shaped call - window and a zero in registers, text and
//  title on the stack, in that order.  Thunked.
__declspec(naked) void __fastcall DisplayFatalMessageBox_6F6C2620(HWND, int, const char*, const char*)
{
    __asm { mov eax, 0x6F6C2620 }
    __asm { jmp eax }
}

void __fastcall GameMainShowFatalMessage(unsigned int lineCount)
{
    char messageBuffer[0x100];
    messageBuffer[0] = 0;
    memset(messageBuffer + 1, 0, 0xFF);
    char defaultMessage[] = "Warcraft III was unable to initialize.";

    void* fileBuffer = 0;
    int fileLength = 0;
    if (Storm_279("ui\\startupstrings.txt", &fileBuffer, &fileLength, 1, 0))
    {
        char* cursor = (char*)fileBuffer;
        for (unsigned int line = 0; line < 6 && line != lineCount; ++line)
        {
            Storm_504(&cursor, messageBuffer, 0x100, "\n\r", 0);
        }
        Storm_280(fileBuffer);
    }

    const char* text = messageBuffer[0] ? messageBuffer : defaultMessage;
    HWND hWnd = GetSelectedWindow(2);
    DisplayFatalMessageBox_6F6C2620(hWnd, 0, text, "Warcraft III");
}
