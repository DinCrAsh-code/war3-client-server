//============================================================================
//  0x6F7CB930 - FACEDATA_CreateGlyphEntry.  CItem vtable slots 32/104
//  closure, batch K continuation (depth 12-25+).
//
//  Allocates a fresh record (sized from `self->m_owner->m_desc->m_size`)
//  and a second, fixed 0x0C-byte side record, links the record's `self`
//  and side-record fields, calls an optional per-owner-descriptor
//  constructor callback (`m_desc->m_ctor`, a raw function pointer - the
//  same "read a slot, test it, call it" shape this closure's other
//  hand-written dispatches use), and on success hands the record back
//  through `*outPtr` and appends it to `self->m_list` (FACEDATA_LinkTail).
//  Any allocation or constructor-callback failure releases both records
//  and answers that failure code instead.  Three cheap early-outs (a null
//  `self`, a null `outPtr`, and a null `self->m_owner`) answer 0x23/0x24/
//  0x22 without allocating anything.
//============================================================================
#include "storm.h"

int __fastcall FACEDATA_AllocateAndZero(void* self, int size, void** outPtr);
void __fastcall FACEDATA_ReleaseIfSet(void* self, void** slot);
void __fastcall FACEDATA_LinkTail(void* list, void* node);

struct SFaceOwnerDesc
{
    char                     m_reserved00[0x28];
    int                      m_size;    // 0x28
    char                     m_reserved2C[0x38 - 0x2C];
    int (__thiscall *m_ctor)(void*);    // 0x38
};

struct SFaceOwner
{
    char             m_reserved00[0x14];
    SFaceOwnerDesc*  m_desc;    // 0x14
    char             m_reserved18[0x64 - 0x18];
    void*            m_allocator;   // 0x64
};

struct SFaceGlyphTable
{
    char         m_reserved00[0x60];
    SFaceOwner*  m_owner;   // 0x60
    char         m_reserved64[0x6C - 0x64];
    char         m_list[8];   // 0x6C - SFaceList (m_head/m_tail)
};

//  `void*` rather than `SFaceGlyphTable*` - a caller elsewhere in this
//  closure (FACEDATA_InsertGlyphEntry, Misc/citemclosure_facedata_insertglyph.cpp)
//  declares this against its own local view of the object, and MSVC bakes
//  the parameter type into the mangled name; see FACEDATA_AllocateAndZero's
//  own note, Misc/citemclosure_facedata_release.cpp, for the
//  `link_check.py` failure a mismatched re-declaration causes.
int __fastcall FACEDATA_CreateGlyphEntry(void* selfVoid, void** outPtr)
{
    SFaceGlyphTable* self = (SFaceGlyphTable*)selfVoid;

    if (self == 0)
        return 0x23;
    if (outPtr == 0)
        return 0x24;
    if (self->m_owner == 0)
        return 0x22;

    *outPtr = 0;

    SFaceOwner* owner = self->m_owner;
    void* allocator = owner->m_allocator;
    SFaceOwnerDesc* desc = owner->m_desc;

    void* record = 0;
    void* side = 0;
    int status;

    status = FACEDATA_AllocateAndZero(allocator, desc->m_size,
                                       (void**)&record);
    if (status != 0)
        goto release_both;

    status = FACEDATA_AllocateAndZero(allocator, 0x0C, (void**)&side);
    if (status != 0)
        goto release_both;

    *(void**)record = self;
    *((void**)record + (0x28 / 4)) = (void*)0;

    if (desc->m_ctor != 0)
    {
        status = desc->m_ctor(record);
        if (status != 0)
            goto release_side_only;
    }

    *outPtr = record;
    *((void**)side + (8 / 4)) = record;
    FACEDATA_LinkTail(&self->m_list, record);
    return status;

release_side_only:
release_both:
    FACEDATA_ReleaseIfSet(allocator, (void**)&side);
    FACEDATA_ReleaseIfSet(allocator, (void**)&record);
    return status;
}
