//============================================================================
//  0x6F60F360 - CSpriteFrame::SetSpritePath.
//
//  Make a sprite for the current detail level, make a tokenizer over the
//  model format's delimiter set, load the model through the global entry
//  point at off_6FA9A410, and hand the result to the two halves of the
//  frame's own set-up before dropping both references.
//
//  The delimiter string " \t\r\n\",;" is asc_6F932BF4, the same set every
//  other model loader in this binary uses.
//============================================================================
#include "frame.h"
#include "framethunks.h"
#include "modeltokens.h"  // g_tokenizerDelimiters (asc_6F932BF4) - the shared
                          // delimiter set the shipped build references here.
                          // Spelling it as a string literal instead makes MSVC
                          // emit a private per-TU COMDAT copy of the string, and
                          // that was the one instruction this function missed.

#include "refcnt.h"

void CSpriteFrame::SetSpritePath(const char* path, int a, int b)
{
    CSprite* sprite = CreateSpriteForDetailLevel(1);
    CTokenizer* tokenizer = CreateTokenizer(g_tokenizerDelimiters);

    g_pSpriteTokenizeEntry(sprite, path, tokenizer, 0, 0, 0);

    SetSprite(sprite, m_scale);
    ConfigureSprite(sprite, a, b);

    ((TRefCnt*)sprite)->Release();
    ((TRefCnt*)tokenizer)->Release();
}
