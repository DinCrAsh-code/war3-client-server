//============================================================================
//  CUnitDatabase's own vtable slot 3 - the scalar deleting destructor,
//  0x6F257340 - the "last unclaimed slot of an otherwise-finished class"
//  the checksum-provider registry batch (see
//  docs/notes/checksum-provider-registry.md and
//  src/Unit/unitdatabase_checksum.cpp's own header note, which left this
//  slot explicitly out of scope) flagged as a quick win.
//
//  Same two-step shape every scalar deleting destructor in this family
//  already uses (Item/abilitycustomdata_deleteself.cpp's own
//  CCustomData::DeleteSelf/CAbilityCustomData::DeleteSelf): call the real
//  destructor, then free `this` through Storm_403 tagged "delete" only
//  when both the caller asked for it (`flags & 1`) and `this` is non-null.
//
//  **The real destructor (0x6F251D70) stays a redirect, not a
//  reconstruction.**  820 instructions, its own
//  `push -1 / push offset SEH_6F251D70 / mov eax, fs:0` prologue (the
//  toolchain-unreproducible __CxxFrameHandler3 shape
//  docs/msvc-vc8-idioms.md already documents and gives up on for
//  sub_6F42CB50/sub_6F02F780), and a long per-table teardown loop over
//  CUnitDatabase's own ~0x8FC-byte field layout - exactly the "much larger
//  batch of work this session did not take on" unitdatabase_checksum.cpp's
//  own header note already reserves for CUnitDatabase's two real
//  constructors.  Modelled as a naked, declared-not-defined member (not a
//  real C++ destructor) for the same reason
//  Containers/hashtablectors.cpp's own constructor redirects are: MSVC will
//  not take __declspec(naked) on a real constructor or destructor.
//============================================================================
#include "unitdatabase.h"

//  Storm.dll ordinal 403 - see docs/msvc-vc8-idioms.md, "Declaring imports
//  so the diff can see them".
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F251D70 - the real ~CUnitDatabase().  Redirect; see this file's own
//  header note.
//----------------------------------------------------------------------------
__declspec(naked) void CUnitDatabase::DestructInternal()
{
    __asm
    {
        mov eax, 06F251D70h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F257340 - vtable slot 3.
//----------------------------------------------------------------------------
//  Vtable slot 3 - a NEW virtual CUnitDatabase adds beyond
//  CWar3SlkDatabase's own three (see unitdatabase.h's own header note).
CUnitDatabase* CUnitDatabase::DeleteSelf(unsigned int flags)
{
    this->DestructInternal();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
