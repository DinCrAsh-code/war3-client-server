//============================================================================
//  0x6F526F70 - TEXTUREIMAGE's constructor: the header dword, both pairs in
//  one two-turn loop off a walking pointer, and the trailing dword.  Written
//  with the pointer the shipped code walks (`[edx-8]` and `[edx]` off
//  `&m_result[0]`) rather than as two indexed stores, because that is what
//  the shipped stream has and an indexed pair unrolls.
//============================================================================
#include "renderdevice.h"

TEXTUREIMAGE::TEXTUREIMAGE()
{
    m_kind = 0;

    int* p = m_result;
    int n = 2;
    do
    {
        p[-2] = 0;
        p[0]  = 0;
        p++;
    }
    while (--n != 0);

    m_extra = 0;
}
