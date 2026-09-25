//============================================================================
//  The three lazily-created model token tables (tokenizer.h).
//
//  Each getter is the same shape: read the global, return it if it is
//  already there, otherwise create a tokenizer over the shared delimiter
//  set " \t\r\n\",;", store it, fill it, and re-read the global rather than
//  returning the value it just stored.  That re-read is in the shipped code
//  and is not redundant in the way it looks - the fill functions could in
//  principle replace the global - so it is reproduced rather than tidied
//  away; it also costs nothing, being the same load the early-out path uses.
//============================================================================
#ifndef MODELTOKENS_H
#define MODELTOKENS_H

#include "tokenizer.h"

extern CTokenizer* g_pBoneTokenizer;        // dword_6FAB4DEC
extern CTokenizer* g_pAttachmentTokenizer;  // dword_6FAB4DF0
extern CTokenizer* g_pAnimationTokenizer;   // dword_6FAB4DE8

//  asc_6F932BF4 - whitespace, quote, comma and semicolon.
extern const char g_tokenizerDelimiters[];

//  0x6F2C7F50 / 0x6F2C8030 / 0x6F2C7A80 - modeltokenfill.cpp.
void FillBoneTokenizer();
void FillAttachmentTokenizer();
void FillAnimationTokenizer();

//  0x6F2C82C0 / 0x6F2C82F0 / 0x6F2C8270 - modeltokens.cpp.
CTokenizer* GetBoneTokenizer();
CTokenizer* GetAttachmentTokenizer();
CTokenizer* GetAnimationTokenizer();

#endif
