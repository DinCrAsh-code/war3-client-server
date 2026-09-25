//============================================================================
//  0x6F4E7C40 - allocate, construct and take the first reference on the
//  0x170-byte object the camera's controller slot holds.
//
//  The name is the one gameui_thunks.cpp/cameraresetdefaults.cpp already
//  gave this address while it was a redirect; one shipped address carries
//  one C++ name, so it keeps it.  What the *shipped* code says about the
//  object is the allocation tag: "HCAMERA", line -2 - Storm's handle-type
//  tag standing in for __FILE__, the same convention CreateTokenizer's
//  "HTOKENIZER" already documents (storm.h, tokenizercreate.cpp).
//
//  The tail loads edx with the "HCAMERA" tag before calling TRefCnt::AddRef,
//  and that instruction is reproduced now - AddRef takes the tag as its one
//  __fastcall argument, so it lands in edx by itself.  One shipped
//  instruction is still not reproduced and cannot be:
//
//  * the whole body sits inside the cookie-XORed, frame-pointer-omitted
//    __CxxFrameHandler3 frame MSVC emits for the placement-new cleanup
//    (`operator delete(raw)` if the constructor throws), plus its two
//    `mov [esp+18h+var_4], <state>` stores and the trailing per-function
//    funclet.  docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped
//    frame this toolchain cannot reproduce", is the whole story: nothing
//    at this repo's fixed /GS- /EHs-c- reaches that frame shape.
//
//  Own translation unit: the construction call is real.
//============================================================================
#include "refcnt.h"
#include "storm.h"

//  0x6F4E7790 - the object's constructor proper, `this` in ecx, `retn 0`.
TRefCnt* __fastcall ConstructCameraControllerObject(void* raw);

void* __fastcall MakeCameraController()
{
    void* raw = SMemAlloc(0x170, "HCAMERA", -2, 0);

    //  Spelled as "null stays null" rather than as two early returns: the
    //  shipped stream joins both paths on one `test eax, eax`, which is
    //  MSVC's own placement-new shape (`if (ptr == 0)` in front of the
    //  constructor call - see docs/msvc-vc8-idioms.md, "Placement new").
    TRefCnt* self = 0;
    if (raw != 0)
        self = ConstructCameraControllerObject(raw);

    if (self == 0)
        return 0;

    return self->AddRef("HCAMERA");
}
