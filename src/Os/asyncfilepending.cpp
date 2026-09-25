//============================================================================
//  0x6F7E72D0 - AsyncFileAddPending.  One InterlockedIncrement on the file's
//  pending-operation count; a two-instruction adjustor in the image.
//============================================================================
#include "asyncfile.h"

//  `volatile long*`, matching src/Sync/critsec.cpp's own definition: MSVC
//  puts the qualifier in the mangled name, so a plain `long*` here would be a
//  second symbol that never meets the first.
long __fastcall InterlockedIncrementAt(volatile long* target);   // 0x6F6C4060

void __fastcall AsyncFileAddPending(ASYNCFILE* file)
{
    InterlockedIncrementAt((volatile long*)&file->m_pending);
}
