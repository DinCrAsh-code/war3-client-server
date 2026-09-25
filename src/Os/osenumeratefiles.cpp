//============================================================================
//  0x6F6D7880 - enumerate `pattern` in the directory `pathTemplate` names,
//  handing every match to `callback` until one of them returns nonzero.
//
//  `pathTemplate` is a *file* path, not a directory: its last component is
//  cut off and the pattern appended in its place, which is why the two Storm
//  string calls bracket OsPathStripFileName.
//
//  The record handed to the callback carries Storm's own attribute bits, not
//  Windows', and the shipped code builds them one test at a time rather than
//  masking: FILE_ATTRIBUTE_DIRECTORY *sets* the field (it does not or into
//  it), and readonly/hidden/system or into whatever that left.  The
//  last-write time is written as a 64-bit value or-ed into a zero, which is
//  what the two `xor`/`or` pairs in the stream are.
//============================================================================
#include "os.h"

int __fastcall OsEnumerateFiles(const char* pathTemplate, const char* pattern,
                                OsFindCallback callback, void* context,
                                int wantHidden, int wantSystem)
{
    char             search[MAX_PATH];
    WIN32_FIND_DATAA fd;
    OSFINDDATA       found;
    int              result;

    Storm_501(search, pathTemplate, 0x7FFFFFFF);
    OsPathStripFileName(search);
    Storm_503(search, pattern, 0x7FFFFFFF);

    HANDLE handle = FindFirstFileA(search, &fd);
    result = 0;
    if (handle == INVALID_HANDLE_VALUE)
        return 0;


    for (;;)
    {
        Storm_501(found.m_name, fd.cFileName, 0x7FFFFFFF);

        ULARGE_INTEGER written;
        written.QuadPart = 0;
        written.LowPart |= fd.ftLastWriteTime.dwLowDateTime;
        found.m_writeTimeLow = written.LowPart;
        found.m_size = fd.nFileSizeLow;
        written.HighPart |= fd.ftLastWriteTime.dwHighDateTime;

        DWORD attributes = fd.dwFileAttributes;
        found.m_writeTimeHigh = written.HighPart;
        found.m_flags = 0;

        if (attributes & FILE_ATTRIBUTE_DIRECTORY)
            found.m_flags = 0x10;
        if (attributes & FILE_ATTRIBUTE_READONLY)
            found.m_flags |= 1;
        if (attributes & FILE_ATTRIBUTE_HIDDEN)
            found.m_flags |= 2;
        if (attributes & FILE_ATTRIBUTE_SYSTEM)
            found.m_flags |= 4;

        if ((wantHidden || !(attributes & FILE_ATTRIBUTE_HIDDEN)) &&
            (wantSystem || !(attributes & FILE_ATTRIBUTE_SYSTEM)))
        {
            if (callback(&found, context))
            {
                result = 1;
                break;
            }
        }

        //  Nothing assigns `result` on this path: the shipped stream reads
        //  the slot back (`mov edi,[esp+16]`) rather than re-zeroing a
        //  register, which is what a plain `break` compiles to and what
        //  `result = 0; break;` does not.
        if (!FindNextFileA(handle, &fd))
            break;
    }

    FindClose(handle);
    return result;
}
