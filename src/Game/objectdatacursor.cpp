//============================================================================
//  0x6F3DF380 - the element the cursor is on, or null when it has run past
//  the end.  A plain bounds test against the array's own count.
//============================================================================
#include "storm.h"

struct SCursoredArray
{
    unsigned int m_alloc;       // +0x00
    unsigned int m_count;       // +0x04
    void**       m_data;        // +0x08
    char         m_reservedC[0x04];
    unsigned int m_cursor;      // +0x10
};

void* __fastcall CursoredArrayCurrent(SCursoredArray* self)
{
    if (self->m_cursor >= self->m_count)
        return 0;

    return self->m_data[self->m_cursor];
}
