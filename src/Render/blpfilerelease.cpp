//============================================================================
//  0x6F78A740 - CBlpFile::Release: give the loaded bytes back to whoever
//  produced them and drop the two scratch blocks.  `.\blp.cpp` lines 0x5B
//  and 0x5E are the module tag and the two line numbers the shipped code
//  hands Storm.
//============================================================================
#include "imagefile.h"
#include "filecache.h"

void CBlpFile::Release()
{
    if (m_data != 0)
    {
        ReleaseLoadedFile(m_data, m_ownsData);
        m_data = 0;
    }

    if (m_scratch != 0)
        SMemFree(m_scratch, kBlpCpp, 0x5B, 0);
    m_scratch = 0;

    if (m_scratch4AC != 0)
        SMemFree(m_scratch4AC, kBlpCpp, 0x5E, 0);
    m_scratch4AC = 0;
}
