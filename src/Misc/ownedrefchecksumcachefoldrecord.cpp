//============================================================================
//  0x6F6FA650 - SOwnedRefChecksumCache::FoldRecord: one tagged field
//  record (stride 0x418, elements of the record array
//  SOwnedRefChecksumCache::FoldGroup walks) folded through `this`'s own
//  vtable (slot 5/+0x14 - a query taking one int argument, "2" at every
//  call site this closure has found; slot 2/+0x08 - a no-argument lookup
//  returning a further object whose own vtable slot 6/+0x18 compares this
//  record's own +0x00 field, 0x0C bytes' worth, via
//  SFieldRecordCompareRef::NormalizeCompare - fieldrecordcompareref.cpp,
//  its own TU for the same inlining reason as every other split in this
//  chain).  None of the three receivers involved is a named class here -
//  see customdatachecksumcache.h's own header.
//============================================================================
#include "customdatachecksumcache.h"
#include "CFloat.h"

unsigned int __thiscall SOwnedRefChecksumCache::FoldRecord(SFieldRecord* record)
{
    typedef unsigned int (__thiscall *QueryExtFieldsFn)(void*, int);   // vtbl[5]
    typedef void*        (__thiscall *LookupCompareRefFn)(void*);      // vtbl[2]

    unsigned int hash = (record->m_seedA << 3) | (record->m_seedA >> 29);
    hash ^= record->m_type;
    hash = (hash << 3) | (hash >> 29);

    if (((QueryExtFieldsFn)(*(void***)this)[5])(this, 2) != 0)
    {
        hash ^= record->m_extA;
        hash = (hash << 3) | (hash >> 29);
        hash ^= record->m_extB;
        hash = (hash << 3) | (hash >> 29);
    }

    switch (record->m_type)
    {
    case 0:
    case 1:
        hash ^= record->m_intValue;
        hash = (hash << 3) | (hash >> 29);
        break;

    case 2:
    {
        //  0x6F6EEC20 - Math/softfloat.cpp's own CFloatToInt (CFloat.h).
        int converted = CFloatToInt(CFloat(record->m_floatBits));
        hash ^= (unsigned int)converted;
        hash = (hash << 3) | (hash >> 29);
        break;
    }

    case 3:
    {
        //  0x6F39E5C0 - Containers/rotatinghash.cpp.
        extern unsigned int __fastcall RotatingHash(const void* data, unsigned int length);
        //  Storm.dll ordinal 506 - SStrLen-shaped, already declared this
        //  way throughout src/ (see e.g. Render/cstatus.h).
        extern unsigned int __stdcall Storm_506(const char* s);

        void* ref = ((LookupCompareRefFn)(*(void***)this)[2])(this);
        if (!((SFieldRecordCompareRef*)ref)->NormalizeCompare(record->m_seedA))
        {
            unsigned int len = Storm_506(record->m_string);
            unsigned int contribution = RotatingHash(record->m_string, len);
            hash ^= contribution;
            hash = (hash << 3) | (hash >> 29);
        }
        break;
    }

    default:
        break;
    }

    return hash;
}
