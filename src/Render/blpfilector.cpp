//============================================================================
//  0x6F4DDCD0 - CBlpFile's constructor.  Out of line, and a real call at its
//  one call site.
//
//  The header is cleared wholesale and then two of its fields are written
//  back: the 'BLP1' magic, so that a Load which never reaches the file still
//  fails the magic test the same way, and the picture type, whose default of
//  2 is the switch's own `default` arm in CBlpTexture::Create.  The path
//  buffer is cleared separately because it is not part of the header.
//============================================================================
#include "imagefile.h"

extern "C" void* __cdecl memset(void* dest, int value, unsigned int n);

CBlpFile::CBlpFile()
{
    m_reserved00  = 0;
    m_scratch     = 0;
    m_scratch4AC  = 0;
    m_budget      = 0x8000;
    m_quality     = 100;
    m_reserved5CC = 0;
    m_reserved5D0 = 0;
    m_reserved5D4 = 1;

    memset(&m_header, 0, sizeof(m_header));

    //  'BLP1', least significant byte first.
    m_header.m_magic       = 0x31504C42;
    m_header.m_pictureType = 2;

    memset(m_path, 0, sizeof(m_path));

    m_data = 0;
}
