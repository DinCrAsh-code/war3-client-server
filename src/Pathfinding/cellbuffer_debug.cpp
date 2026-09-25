//============================================================================
//  0x6F4B34E0, 0x6F4B34F0 - the allocation tag CMemBlock hands to Storm.
//
//  Their own translation unit: CMemBlock::SetSize calls both of them four
//  times between them and the shipped code makes every one a real call, which
//  a two-instruction function in the same TU would never survive.
//============================================================================
#include "cellbuffer.h"

//----------------------------------------------------------------------------
//  0x6F4B34E0
//----------------------------------------------------------------------------
const char* CMemBlockDebugFile()
{
    return ".\\cmemblock.cpp";
}

//----------------------------------------------------------------------------
//  0x6F4B34F0
//----------------------------------------------------------------------------
int CMemBlockDebugLine()
{
    return 0x174;
}
