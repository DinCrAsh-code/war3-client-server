//============================================================================
//  0x6F30EC90 - GetCursorModeTokenizer: the cursor-mode tokenizer, made and
//  filled on first use.
//
//  Same lazy-singleton shape as the three model tokenizers
//  (modeltokens.cpp) and the same delimiter set; the global is read a second
//  time on the way out rather than the freshly stored pointer being reused,
//  which is what the shipped `mov eax, dword_6FAB50FC` after the fill is.
//
//  Own translation unit: two real calls out of it.
//============================================================================
#include "tokenizer.h"
#include "modeltokens.h"   // g_tokenizerDelimiters - the same delimiter set
                          // the three model tokenizers use; declared once there.

//  dword_6FAB50FC - the game's own global; declared, never defined here.
extern CTokenizer* g_pCursorModeTokenizer;

//  0x6F309720 - cursormodetokens.cpp.
void RegisterCursorModeTokens();

CTokenizer* GetCursorModeTokenizer()
{
    if (g_pCursorModeTokenizer == 0)
    {
        g_pCursorModeTokenizer = CreateTokenizer(g_tokenizerDelimiters);
        RegisterCursorModeTokens();
    }
    return g_pCursorModeTokenizer;
}
