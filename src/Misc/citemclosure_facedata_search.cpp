//============================================================================
//  One more FACEDATA leaf - CItem vtable slots 32/104 closure, batch K,
//  depth 12-25+.  See Misc/citemclosure_facedata_metrics.cpp's own header
//  for how "FACEDATA" is ground truth rather than a guess.
//============================================================================
#include "storm.h"

//----------------------------------------------------------------------------
//  0x6F7C9DC0 - FACEDATA::FindEntryByField4: a plain linear scan over a
//  pointer array (`m_data`/`m_count` at +0x28/+0x24) for the entry whose
//  own +0x04 field equals `key`, stashing the hit at +0x5C.  Two null-ish
//  guards ahead of the scan (`this == 0` -> 0x23, `m_data == 0` -> 0x26)
//  and a not-found code of 6, the same small-integer "Storm result code"
//  shape FACEDATA_LinkTail/Unlink's own callers use elsewhere in this
//  closure.
//----------------------------------------------------------------------------
struct SFaceSearchTable
{
    char    m_reserved00[0x24];
    unsigned int m_count;   // 0x24
    void**  m_data;         // 0x28
    char    m_reserved2C[0x5C - 0x2C];
    void*   m_found;        // 0x5C
};

int __fastcall FACEDATA_FindEntryByField4(SFaceSearchTable* self, void* key)
{
    if (self == 0)
        return 0x23;

    void** data = self->m_data;
    if (data == 0)
        return 0x26;

    void** end = data + self->m_count;
    for (void** p = data; p < end; p++)
    {
        void* entry = *p;
        if (*(void**)((char*)entry + 4) == key)
        {
            self->m_found = entry;
            return 0;
        }
    }
    return 6;
}
