//============================================================================
//  0x6F4E9F80 - the same allocate/construct/AddRef shape as
//  cameracontrollercreate.cpp (which see for the two instructions this
//  toolchain cannot emit), for the 0x70-byte object the camera's second
//  factory hands back.
//
//  The shipped allocation tag is "HLISTENER" - Storm's handle-type tag, and
//  the strongest evidence in the function about what the object *is*: an
//  audio listener, the same one CCameraWar3::ResetToDefaults configures out
//  of the "Listener"/"ListenerDistance"/"ListenerAOA" keys.  The C++ name
//  stays MakeCameraEventSink because gameui_thunks.cpp and
//  cameraresetdefaults.cpp already gave this address that name and one
//  shipped address carries exactly one name.
//
//  One shape difference from its HCAMERA twin, and it is in the shipped
//  stream rather than a choice here: this one branches `jz` to the failure
//  exit and falls through into the AddRef, where 0x6F4E7C40 branches `jnz`
//  to the AddRef and falls through into the failure exit.  Writing the
//  success path as the fall-through (`if (self != 0) return AddRef(); return
//  0;`) is what reproduces it - see docs/msvc-vc8-idioms.md, "Two `return`s
//  of the same constant: which one is the fall-through".
//============================================================================
#include "refcnt.h"
#include "storm.h"

//  0x6F4E9DA0 - the object's constructor proper, `this` in ecx, `retn 0`.
TRefCnt* __fastcall ConstructCameraEventSinkObject(void* raw);

void* __fastcall MakeCameraEventSink()
{
    void* raw = SMemAlloc(0x70, "HLISTENER", -2, 0);

    TRefCnt* self = 0;
    if (raw != 0)
        self = ConstructCameraEventSinkObject(raw);

    if (self != 0)
        return self->AddRef("HLISTENER");

    return 0;
}
