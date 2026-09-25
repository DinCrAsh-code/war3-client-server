//============================================================================
//  `.\Jass.cpp` - load the saved function-id array and declare the function
//  names that go with it, both read off a CDataStore-shaped stream
//  (Net/cdatastore.h already has the class; ReadDword/ReadDwordAlt/
//  ReadString are all pre-existing members, and 0x6F4C3120 is a fifth
//  ICF-less duplicate of ReadDwordArray, added to CDataStore here).
//
//  0x6F454FF0 opens with the plain /GS stack-buffer cookie
//  (`mov eax,dword_6FAAE140` / `xor eax,esp`) docs/msvc-vc8-idioms.md
//  already documents as unreproducible under this build's fixed `/GS-` -
//  the check is simply off, so this build never emits the prologue/epilogue
//  pair the dump carries around its own 0x400-byte name buffer. Marked
//  IDENTICAL in BEHAVIOUR for that reason; the body past the frame matches.
//============================================================================
#include "cdatastore.h"
#include "jasssymtables.h"

void __stdcall SErrSetLastError(unsigned int error);
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int n);

//----------------------------------------------------------------------------
//  0x6F4C3120 - a fifth instance of ReadDwordArray's own body (see
//  cdatastore_readdwordarray.cpp's own note on the four /OPT:ICF-less
//  duplicates already reconstructed on the read side).
//----------------------------------------------------------------------------
CDataStore* CDataStore::ReadDwordArrayAlt(void* dest, unsigned int count)
{
    int ok = (dest == 0 && count != 0) ? 0 : -1;
    if (ok == 0)
    {
        SErrSetLastError(87);
        return this;
    }

    if ((unsigned int)m_readPos <= (unsigned int)m_field10 && count != 0)
    {
        char* out = (char*)dest;
        unsigned int left = count * 4;

        do
        {
            unsigned int at = m_readPos;
            unsigned int run = (unsigned int)m_field10 - at;
            if (run >= left)
                run = left;
            if (run >= (unsigned int)m_field0xC)
                run = m_field0xC;
            if (run <= 4)
                run = 4;
            run &= ~3u;

            if (!EnsureReadable(at, run))
                break;

            const char* src = (const char*)m_field4 - m_field8 + m_readPos;
            if (out != src)
                memcpy(out, src, run);

            m_readPos += run;
            out += run;
            left -= run;
        }
        while (left != 0);
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F454FF0.  `this` is a small header the save format keeps ahead of a
//  function scope: a raw dword array of function ids, its own count, and a
//  pointer to the JASSSCOPE (jassdeclare.cpp) that owns the matching name
//  table - `this+8` is loaded, not addressed, everywhere it is used, which
//  is what makes it a pointer rather than an embedded object.
//----------------------------------------------------------------------------
struct JASSFUNCTABLEHEADER
{
    unsigned int*  m_ids;       // +0x00
    unsigned int   m_idCount;   // +0x04
    JASSSCOPE*     m_funcScope; // +0x08

    void Load(CDataStore* stream);
};

void JASSFUNCTABLEHEADER::Load(CDataStore* stream)
{
    unsigned int unused = 0;
    stream->ReadDword(&unused);
    stream->ReadDword(&m_idCount);

    m_ids = (unsigned int*)SMemAlloc(m_idCount * 4, ".\\Jass.cpp", 0x186, 0);
    stream->ReadDwordArrayAlt(m_ids, m_idCount);

    unsigned int nameCount = 0;
    stream->ReadDwordAlt(&nameCount);

    for (unsigned int index = 1; index < nameCount; index++)
    {
        char name[0x400];
        stream->ReadString(name, 0x400);
        m_funcScope->Declare(name, index);
    }
}
