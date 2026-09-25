//============================================================================
//  0x6F4EAFE0 - the twin of fogobjectcreate.cpp forty-eight bytes further
//  on: a 0xDC-byte allocation tagged "HLIGHT", a constructor of its own
//  (0x6F4EAA30) and the same conditional AddRef.  See that file for the two
//  shipped instructions this toolchain cannot emit.
//
//  Its own translation unit rather than sharing fogobjectcreate.cpp's: the
//  two are separate shipped functions and neither calls the other, so
//  putting them in one TU would only invite MSVC to fold them.
//============================================================================
#include "refcnt.h"
#include "storm.h"

//  0x6F4EAA30 - the light object's constructor proper, `this` in ecx,
//  `retn 0`.
TRefCnt* __fastcall ConstructLightObject(void* raw);

TRefCnt* __fastcall CreateLightObject()
{
    void* raw = SMemAlloc(0xDC, "HLIGHT", -2, 0);

    TRefCnt* self = 0;
    if (raw != 0)
        self = ConstructLightObject(raw);

    if (self == 0)
        return 0;

    return self->AddRef("HLIGHT");
}
