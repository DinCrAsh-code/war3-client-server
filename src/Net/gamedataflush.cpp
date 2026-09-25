//============================================================================
//  0x6F5357F0 - flush whatever is still queued on this stream's async file
//  and then let the stream itself finish the write.  A stream with no file
//  open does neither.
//============================================================================
#include "asyncfile.h"

void __fastcall AsyncFileFlushAlias(ASYNCFILE* file);   // 0x6F7E7FF0

//  0x6F535660 - the stream's own flush; not in this tree.
void __fastcall GameDataStreamFlush(void* self);

struct SGameDataStream
{
    char       m_reserved000[0x100];
    ASYNCFILE* m_file;      // +0x100
};

void __fastcall GameDataFlush(SGameDataStream* self)
{
    if (self->m_file == 0)
        return;

    AsyncFileFlushAlias(self->m_file);
    GameDataStreamFlush(self);
}
