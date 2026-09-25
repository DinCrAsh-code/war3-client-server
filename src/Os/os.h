//============================================================================
//  The platform layer: Storm's OS abstraction as it is statically linked
//  into Game.dll - the path helpers at 0x6F6C5xxx, the file and directory
//  wrappers at 0x6F6D7xxx and the CRC-32 at 0x6F6B2660.
//
//  Everything here is a thin wrapper over a kernel32 import, and every one
//  of them is __fastcall with the "interesting" argument in ecx: that is
//  what the shipped stream shows at every call site, and it is the only
//  spelling that puts the same bytes on the stack for the callee-cleanup
//  `retn <n>` each of them has.
//
//  Reached from `ReloadGameCachesFromDisk` (0x6F3BC430) through the object
//  data reload, and from `RestartGame` (0x6F3BBC00) through CSaveGame's
//  header read.
//============================================================================
#ifndef OS_H
#define OS_H

#include <windows.h>

//----------------------------------------------------------------------------
//  Storm string imports.  Ordinal-only exports, so the dump names them
//  Storm_<ordinal> and funcmap.py maps the ordinal onto the name declared
//  here - see docs/msvc-vc8-idioms.md, "Declaring imports so the diff can
//  see them".  569/570/571 are the three character searches; which of them
//  is "first" and which "last" is readable from the call sites (570 is used
//  to find the *last* separator, 569 to step forward through them) but the
//  Storm names themselves are not recoverable from this image, so the
//  ordinal is the name, exactly as Storm_506/508 already are here.
//----------------------------------------------------------------------------
unsigned int __stdcall Storm_506(const char* s);
void         __stdcall Storm_501(char* dest, const char* src,
                                 unsigned int size);
void         __stdcall Storm_503(char* dest, const char* src,
                                 unsigned int size);
//  Storm_281 - an archive-open-shaped seven-argument call
//  (CNetData::LoadReplayHeader, Net/netdata_loadreplayheader.cpp): a mode
//  dword, a filename, two out-parameters (a handle and a size, both written
//  through), and three more int-shaped arguments this tree's own single
//  call site always passes as 0/1/0.  The real Storm name behind ordinal
//  281 is not recoverable from this image; the ordinal is the name, exactly
//  as Storm_506/508 already are here.
int          __stdcall Storm_281(int mode, const char* fileName,
                                 void** outHandle, unsigned int* outSize,
                                 int a5, int a6, int a7);
void         __stdcall SErrSetLastError(unsigned int error);
char*        __fastcall Storm_569(const char* s, char ch);
char*        __fastcall Storm_570(const char* s, char ch);
char*        __fastcall Storm_571(const char* s, char ch);

//----------------------------------------------------------------------------
//  Path helpers.
//----------------------------------------------------------------------------
int   __fastcall OsPathRootLength(const char* path);       // 0x6F6C57D0
void  __fastcall OsPathStripFileName(char* path);          // 0x6F6C5980
void  __fastcall OsPathStripTrailingName(char* path);      // 0x6F6C59C0
int   __fastcall OsPathIsRelative(const char* path);       // 0x6F6C5B20
DWORD __fastcall OsGetModuleFileName(char* buffer, DWORD size);   // 0x6F6C5860
void  __fastcall OsGetModuleDirectory(char* buffer, DWORD size);  // 0x6F6C6000

//----------------------------------------------------------------------------
//  File and directory wrappers.  The trailing `tag` of OsCreateFile is a
//  four-character sentinel the one call site in this tree passes as
//  0x3F3F3F3F ('????') and the body never reads; it is part of the ABI all
//  the same, because the function is callee-cleanup and pops it (`retn 10h`
//  against three forwarded arguments).
//----------------------------------------------------------------------------
HANDLE __fastcall OsCreateFile(const char* lpFileName, DWORD dwDesiredAccess,
                               DWORD dwShareMode, DWORD dwCreationDisposition,
                               DWORD dwFlagsAndAttributes, DWORD tag);   // 0x6F6D7390
void   __fastcall OsCloseHandle(HANDLE h);                 // 0x6F6D73D0
DWORD  __fastcall OsGetFileAttributes(const char* path);   // 0x6F6D7590
BOOL   __fastcall OsDeleteFile(const char* path);          // 0x6F6D7600
BOOL   __fastcall OsCreateDirectoryTree(const char* path, int wholeTree);  // 0x6F6D7620
int    __fastcall OsFileExists(const char* path);          // 0x6F6D7DE0

//----------------------------------------------------------------------------
//  0x6F6D7880 - walk `directory` + `pattern` with FindFirstFile/FindNextFile
//  and hand each match to `callback` until it returns nonzero.  The record
//  the callback gets is built on the stack: size, attribute flags remapped
//  to Storm's own bits, name, and the last-write FILETIME.
//----------------------------------------------------------------------------
struct OSFINDDATA
{
    unsigned int    m_size;             // +0x00 - nFileSizeLow
    unsigned int    m_flags;            // +0x04 - remapped attributes
    char            m_name[0x108];      // +0x08 - cFileName
    unsigned int    m_writeTimeLow;     // +0x110
    unsigned int    m_writeTimeHigh;    // +0x114
};

typedef int (__fastcall *OsFindCallback)(OSFINDDATA* found, void* context);

int __fastcall OsEnumerateFiles(const char* directory, const char* pattern,
                                OsFindCallback callback, void* context,
                                int wantHidden, int wantSystem);   // 0x6F6D7880

//----------------------------------------------------------------------------
//  0x6F6B2660 - CRC-32, table at 0x6F972308, pre- and post-inverted.
//----------------------------------------------------------------------------
unsigned int __fastcall OsCrc32(unsigned int crc, const void* data,
                                unsigned int length);

//: 0x6F6DA080 - a bare InterlockedIncrement, no null test.
long __fastcall OsInterlockedIncrement(long* addend);

#endif
