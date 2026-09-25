//============================================================================
//  0x6F78A800 - CBlpFile::Load: drop whatever is held, remember the path and
//  the ownership flag, pull the file through the cache, copy the header out
//  of it and work out how many mip levels it has.
//
//  The header copy is a struct assignment of the whole 0x49C block out of
//  the file's first bytes - the shipped `rep movsd` of 0x127 dwords - and
//  only *then* is the magic tested, which is why a short file is caught by
//  the magic and not by a length check.
//============================================================================
#include "imagefile.h"
#include "filecache.h"
#include "renderthunks.h"

int CBlpFile::Load(const char* path, int takeOwnership)
{
    Release();

    Storm_501(m_path, path, 0x104);
    m_ownsData = takeOwnership;

    if (LoadCachedFile(path, &m_data, &m_dataSize, takeOwnership) == 0)
        return 0;

    m_header = *(const BLPHEADER*)m_data;

    if (m_header.m_magic != 0x31504C42)
        return 0;

    if (m_header.m_hasMips != 0)
        m_mipCount = BlpMipCount(m_header.m_width, m_header.m_height);
    else
        m_mipCount = 1;

    return 1;
}
