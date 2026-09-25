//============================================================================
//  0x6F7E0FA0 - OpenFaceFileStream.  CItem vtable slots 32/104 closure,
//  batch K continuation.  Not itself part of this session's worklist, but
//  a callee FACEDATA_LoadFaceEntry (Misc/citemclosure_facedata_loadentry.cpp)
//  needs to compile against.
//
//  Opens `fileName` "rb", measures its length via fseek/ftell/fseek, and
//  fills in an eight-field load-state record: length@+4, the FILE*@+0xC,
//  the file name pointer itself@+0x10, a zeroed cursor@+8, and two function
//  pointers@+0x14/+0x18 into the shipped 0x6F7E0F50/0x6F7E0F70 decode-step
//  pair this call tree never reaches (below this session's depth cut, and
//  a `loc_` mid-function label at that - not a function this repo can name
//  or reconstruct on its own).  Returns 0x28 for a null output pointer,
//  `errno+1`-shaped (`fopen`'s null result plus one) on a failed open, and
//  0 on success.
//============================================================================
#include "storm.h"
#pragma warning(disable: 4996)   // fopen: this build targets the real CRT, not fopen_s
#include <stdio.h>

struct SFaceFileLoadState
{
    char    m_reserved00[4];
    int     m_length;      // 0x04
    int     m_reserved08;  // 0x08 - zeroed, a read cursor no reconstructed caller advances
    FILE*   m_file;         // 0x0C
    const char* m_fileName; // 0x10
    void*   m_decodeStep1;  // 0x14 - 0x6F7E0F70, out of this session's scope
    void*   m_decodeStep2;  // 0x18 - 0x6F7E0F50, out of this session's scope
};

//  `out` is `void*` rather than `SFaceFileLoadState*` - callers in this
//  closure declare it against their own local view of the object; see
//  FACEDATA_AllocateAndZero's own note,
//  Misc/citemclosure_facedata_release.cpp, for why a mismatched
//  re-declaration is a real, `link_check.py`-caught link failure.
int __fastcall OpenFaceFileStream(const char* fileName, void* outVoid)
{
    SFaceFileLoadState* out = (SFaceFileLoadState*)outVoid;
    if (out == 0)
        return (int)((char*)0 + 0x28);

    FILE* f = fopen(fileName, "rb");
    if (f == 0)
        return (int)((char*)f + 1);

    fseek(f, 0, 2);
    out->m_length = ftell(f);
    fseek(f, 0, 0);

    out->m_file = f;
    out->m_fileName = fileName;
    out->m_reserved08 = 0;
    out->m_decodeStep1 = (void*)0x6F7E0F70;
    out->m_decodeStep2 = (void*)0x6F7E0F50;

    return 0;
}
