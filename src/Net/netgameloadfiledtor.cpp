//============================================================================
//  0x6F534240 - NetGameLoadFile::~NetGameLoadFile.  The derived vtable is
//  stamped on entry and the base's on the way out, which is what MSVC emits
//  for a destructor in a two-deep hierarchy and what named both classes.
//============================================================================
#include "netgameload.h"
#include "os.h"

NetGameLoadFile::~NetGameLoadFile()
{
    m_vtable = (void*)g_vftableNetGameLoadFile;

    if (m_handle != (void*)-1)
        OsCloseHandle(m_handle);

    m_vtable = (void*)g_vftableNetGameLoadSource;
}
