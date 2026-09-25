//============================================================================
//  0x6F2C82C0 / 0x6F2C82F0 / 0x6F2C8270 - the three lazy getters.  See
//  modeltokens.h for the shape they share; their own translation unit
//  because the CreateTokenizer and Fill* calls are all real.
//============================================================================
#include "modeltokens.h"

CTokenizer* GetBoneTokenizer()
{
    if (g_pBoneTokenizer == 0)
    {
        g_pBoneTokenizer = CreateTokenizer(g_tokenizerDelimiters);
        FillBoneTokenizer();
    }
    return g_pBoneTokenizer;
}

CTokenizer* GetAttachmentTokenizer()
{
    if (g_pAttachmentTokenizer == 0)
    {
        g_pAttachmentTokenizer = CreateTokenizer(g_tokenizerDelimiters);
        FillAttachmentTokenizer();
    }
    return g_pAttachmentTokenizer;
}

CTokenizer* GetAnimationTokenizer()
{
    if (g_pAnimationTokenizer == 0)
    {
        g_pAnimationTokenizer = CreateTokenizer(g_tokenizerDelimiters);
        FillAnimationTokenizer();
    }
    return g_pAnimationTokenizer;
}

