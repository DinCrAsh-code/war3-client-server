//============================================================================
//  0x6F7C9BF0 - FACEDATA_InstallOwnerHookRecord.  CItem vtable slots
//  32/104 closure, batch K continuation (depth 12-25+).
//
//  Allocates a record and a fixed 0x20-byte side block through `table`'s
//  own allocator (+0x08), stamps the record's own owner/allocator/context
//  fields (+0x60/+0x64/+0x68), and calls an install callback off `table`'s
//  own descriptor (+0x14 -> +0x30) with the context and three caller-
//  supplied values.  On success the record is handed back through
//  `*outPtr`; any failure - either allocation, or the install callback
//  itself - calls the descriptor's own uninstall callback (+0x34) when the
//  install failed, then releases both blocks and answers the failure code
//  with `*outPtr` left at 0.  No `ecx`/`edx` oddities here - every value
//  this function reads comes off its own six stack parameters or fields
//  it derives from them, so this is a plain `__stdcall`, not naked.
//============================================================================
#include "storm.h"

int __fastcall FACEDATA_AllocateAndZero(void* self, int size, void** outPtr);
void __fastcall FACEDATA_ReleaseIfSet(void* self, void** slot);

struct SFaceHookDesc
{
    char     m_reserved00[0x24];
    int      m_recordSize;      // 0x24
    char     m_reserved28[0x30 - 0x28];
    int (__fastcall *m_install)(void* ctx, void* record, int a, int b, int c);
                                  // 0x30
    void (__thiscall *m_uninstall)(void* record);   // 0x34
};

struct SFaceHookOwner
{
    char             m_reserved00[8];
    void*            m_allocator;   // 0x08
    char             m_reserved0C[0x14 - 0x0C];
    SFaceHookDesc*   m_desc;        // 0x14
};

//  `table` is `void*` rather than `SFaceHookOwner*` - a caller elsewhere in
//  this closure (FACEDATA_InsertGlyphEntry,
//  Misc/citemclosure_facedata_insertglyph.cpp) declares this against its
//  own local view of the object; see FACEDATA_AllocateAndZero's own note,
//  Misc/citemclosure_facedata_release.cpp, for the `link_check.py` failure
//  a mismatched re-declaration causes.
int __stdcall FACEDATA_InstallOwnerHookRecord(void* tableVoid, void* ctx,
                                               int a, int b, int c,
                                               void** outPtr)
{
    SFaceHookOwner* table = (SFaceHookOwner*)tableVoid;
    void* allocator = table->m_allocator;
    SFaceHookDesc* desc = table->m_desc;

    void* record = 0;
    void* side = 0;
    int status;

    status = FACEDATA_AllocateAndZero(allocator, desc->m_recordSize,
                                       (void**)&record);
    if (status != 0)
        goto release_both;

    status = FACEDATA_AllocateAndZero(allocator, 0x20, (void**)&side);
    if (status != 0)
        goto release_both;

    *(void**)((char*)record + 0x80) = side;
    *(void**)((char*)record + 0x60) = table;
    *(void**)((char*)record + 0x64) = allocator;
    *(void**)((char*)record + 0x68) = ctx;

    status = desc->m_install(ctx, record, a, b, c);
    if (status != 0)
        goto release_both_uninstall;

    *outPtr = record;
    return 0;

release_both_uninstall:
    desc->m_uninstall(record);
release_both:
    //  Matches the shipped stream exactly, unsafe edge case included: on
    //  the record allocation's own failure `record` is null here (Alloc-
    //  AndZero's *outPtr is written even on failure) and this dereferences
    //  it anyway, exactly as 0x6F7C9C87's own `lea edx,[esi+80h]` does -
    //  not a defect this reconstruction introduces.
    FACEDATA_ReleaseIfSet(allocator, (void**)((char*)record + 0x80));
    FACEDATA_ReleaseIfSet(allocator, (void**)&record);
    *outPtr = 0;
    return status;
}
