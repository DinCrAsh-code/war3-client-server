//============================================================================
//  0x6F30FFE0 - GetAnimNameTokenizer: the sprite animation-name tokenizer,
//  made and filled on first use.
//
//  The twin of GetCursorModeTokenizer thirty kilobytes earlier, against a
//  different global and a different filler - 0x6F30B250, which registers
//  "Stand", "Birth", "Death" and "AllyPing", which is what names this one.
//
//  Own translation unit: two real calls out of it.
//============================================================================
#include "tokenizer.h"
#include "modeltokens.h"   // g_tokenizerDelimiters - the same delimiter set
                          // the three model tokenizers use; declared once there.

//  dword_6FAB5540 - the game's own global; declared, never defined here.
extern CTokenizer* g_pAnimNameTokenizer;

//  0x6F30B250 - the four name -> id mappings this tokenizer is built with.
void RegisterAnimNameTokens();

CTokenizer* GetAnimNameTokenizer()
{
    if (g_pAnimNameTokenizer == 0)
    {
        g_pAnimNameTokenizer = CreateTokenizer(g_tokenizerDelimiters);
        RegisterAnimNameTokens();
    }
    return g_pAnimNameTokenizer;
}

//============================================================================
//  0x6F30B250 - CItem vtable-closure batch 2.  Own translation unit per the
//  cursor-mode twin's own reasoning (cursormodetokens.cpp): GetAnimNameTokenizer
//  calls it for real, right after making the tokenizer it fills.
//============================================================================
void RegisterAnimNameTokens()
{
    AddToken(g_pAnimNameTokenizer, "Stand", 0);
    AddToken(g_pAnimNameTokenizer, "Birth", 1);
    AddToken(g_pAnimNameTokenizer, "Death", 2);
    AddToken(g_pAnimNameTokenizer, "AllyPing", 3);
}
