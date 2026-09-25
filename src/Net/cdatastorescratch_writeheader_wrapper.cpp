//============================================================================
//  0x6F684000 - a thin forwarding wrapper around WriteHeaderFrom (0x6F683FC0,
//  cdatastorescratch_writers.cpp). Kept in its own translation unit so the
//  compiler cannot inline that call away - see
//  actionqueuerecord_arrays_find.cpp's own header for why this pattern is
//  preferred over __declspec(noinline) in this codebase.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteHeaderFrom(CDataStoreScratch* self, const unsigned char* src);

CDataStoreScratch* __fastcall WriteHeaderFromWrapper(CDataStoreScratch* self, const unsigned char* src)
{
    WriteHeaderFrom(self, src);
    return self;
}
