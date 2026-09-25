//============================================================================
//  CItemDatabase's own vtable slot 3 - the scalar deleting destructor,
//  0x6F2576D0.  Byte-identical shape to CUnitDatabase's own
//  (Unit/unitdatabase_destructor.cpp): call the real destructor, then free
//  `this` through Storm_403 tagged "delete" only when both the caller
//  asked for it (`flags & 1`) and `this` is non-null.
//
//  **The real destructor (0x6F2574C0) stays a redirect, not a
//  reconstruction**, same reasoning as CUnitDatabase's own
//  (Unit/unitdatabase_destructor.cpp's own header note): its own
//  `push -1 / push offset SEH_6F2574C0 / mov eax, fs:0` prologue is the
//  toolchain-unreproducible __CxxFrameHandler3 shape
//  docs/msvc-vc8-idioms.md documents, wrapping a per-field teardown loop
//  (33 fields here, one call to sub_6F252C10 per field) over
//  CItemDatabase's own layout - out of scope for this batch, the same
//  "much larger batch of work" every other real destructor in this family
//  is.  Modelled as a naked, declared-not-defined member for the same
//  MSVC-will-not-take-naked-on-a-real-destructor reason.
//============================================================================
#include "itemdatabase.h"

//  Storm.dll ordinal 403 - see docs/msvc-vc8-idioms.md, "Declaring imports
//  so the diff can see them".
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F2574C0 - the real ~CItemDatabase().  Redirect; see this file's own
//  header note.
//----------------------------------------------------------------------------
__declspec(naked) void CItemDatabase::DestructInternal()
{
    __asm
    {
        mov eax, 06F2574C0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F2576D0 - vtable slot 3.
//----------------------------------------------------------------------------
CItemDatabase* CItemDatabase::DeleteSelf(unsigned int flags)
{
    this->DestructInternal();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
