//============================================================================
//  0x6F7CB8C0 - FACEDATA_RemoveGlyphEntry.  CItem vtable slots 32/104
//  closure, batch K continuation (depth 12-25+).
//
//  Finds the node keyed by `self` in `self->m_owner`'s own +0x18 list (via
//  the already-reconstructed FindNodeByKey_6F7CA890,
//  Misc/trivialpredicates.cpp), unlinks it (FACEDATA_Unlink), releases the
//  owner's own +8 sub-object through it (FACEDATA_ReleaseIfSet), and tears
//  the entry down (TeardownFaceRecord).  Every early-out - a null `self`,
//  a null `self->m_owner`, or nothing found - answers 0x23, the same
//  "Storm result code" this closure's other small helpers use; success is
//  0.
//============================================================================
#include "storm.h"

void* __fastcall FindNodeByKey_6F7CA890(const void* self, int key);
void __fastcall FACEDATA_Unlink(void* list, void* node);
void __fastcall FACEDATA_ReleaseIfSet(void* self, void** slot);
void __fastcall TeardownFaceRecord(void* owner, void* record, void* recordAlias);

struct SFaceGlyphEntry
{
    char    m_reserved00[0x60];
    void*   m_owner;   // 0x60
};

//  `void*` rather than `SFaceGlyphEntry*` - a caller elsewhere in this
//  closure (FACEDATA_InsertGlyphEntry,
//  Misc/citemclosure_facedata_insertglyph.cpp) declares this against its
//  own local view of the object; see FACEDATA_AllocateAndZero's own note,
//  Misc/citemclosure_facedata_release.cpp, for the `link_check.py` failure
//  a mismatched re-declaration causes.
int __fastcall FACEDATA_RemoveGlyphEntry(void* selfVoid)
{
    SFaceGlyphEntry* self = (SFaceGlyphEntry*)selfVoid;

    if (self == 0)
        return 0x23;

    void* owner = self->m_owner;
    if (owner == 0)
        return 0x23;

    void* ownerField8 = *(void**)((char*)owner + 8);
    void* listHead = (char*)owner + 0x18;
    void* found = FindNodeByKey_6F7CA890(listHead, (int)self);
    if (found == 0)
        return 0x23;

    FACEDATA_Unlink(listHead, found);
    FACEDATA_ReleaseIfSet(ownerField8, (void**)&found);
    TeardownFaceRecord(ownerField8, self, owner);
    return 0;
}
