//============================================================================
//  0x6F309720 - RegisterCursorModeTokens: the twelve name -> id mappings the
//  cursor-mode tokenizer is built with.
//
//  "HoldItem" really is registered twice, both times as 5.  That is in the
//  shipped code, not a transcription slip; AddToken is called twelve times
//  for eleven distinct names.
//
//  Own translation unit: GetCursorModeTokenizer calls it for real, right
//  after making the tokenizer it fills.
//============================================================================
#include "tokenizer.h"

extern CTokenizer* g_pCursorModeTokenizer;   // dword_6FAB50FC

void RegisterCursorModeTokens()
{
    AddToken(g_pCursorModeTokenizer, "Normal", 0);
    AddToken(g_pCursorModeTokenizer, "Select", 1);
    AddToken(g_pCursorModeTokenizer, "Target", 2);
    AddToken(g_pCursorModeTokenizer, "TargetSelect", 3);
    AddToken(g_pCursorModeTokenizer, "InvalidTarget", 4);
    AddToken(g_pCursorModeTokenizer, "HoldItem", 5);
    AddToken(g_pCursorModeTokenizer, "HoldItem", 5);
    AddToken(g_pCursorModeTokenizer, "Scroll", 6);
    AddToken(g_pCursorModeTokenizer, "Left", 7);
    AddToken(g_pCursorModeTokenizer, "Right", 8);
    AddToken(g_pCursorModeTokenizer, "Up", 9);
    AddToken(g_pCursorModeTokenizer, "Down", 10);
}
