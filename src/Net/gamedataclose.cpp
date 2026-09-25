//============================================================================
//  0x6F535810 - flush, then close the async file and forget it.
//
//  The file is re-read from the field after the flush rather than cached:
//  the flush can close it, which is why the second null test is there at
//  all.
//============================================================================
#include "asyncfile.h"

void __fastcall AsyncFileCloseAlias(ASYNCFILE* file);   // 0x6F7E8000

struct SGameDataStream
{
    char       m_reserved000[0x100];
    ASYNCFILE* m_file;      // +0x100
};

void __fastcall GameDataFlush(SGameDataStream* self);

void __fastcall GameDataClose(SGameDataStream* self)
{
    GameDataFlush(self);

    if (self->m_file == 0)
        return;

    AsyncFileCloseAlias(self->m_file);
    self->m_file = 0;
}
