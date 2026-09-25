//============================================================================
//  0x6F5341D0 - NetGameLoadFile::NetGameLoadFile.  Written as a real
//  constructor because the shipped body is one: it stamps the derived
//  vtable and then opens the file, and its caller destroys it with the
//  matching destructor on both exits.
//
//  The trailing 0x3F3F3F3F is OsCreateFile's own dead tag argument - see
//  src/Os/os.h.
//============================================================================
#include "netgameload.h"
#include "os.h"

NetGameLoadFile::NetGameLoadFile(const char* path)
{
    m_vtable = (void*)g_vftableNetGameLoadFile;
    m_handle = OsCreateFile(path, 0x80000000, 1, 3, 0x80, 0x3F3F3F3F);
}
