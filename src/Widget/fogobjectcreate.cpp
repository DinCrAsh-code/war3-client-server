//============================================================================
//  0x6F4EAF50 - allocate, construct and take the first reference on a
//  0xD4-byte fog object.
//
//  The class name is the shipped allocation tag: "HFOG", Storm's handle-type
//  tag in the __FILE__ slot with line -2 (storm.h documents the convention;
//  CreateTokenizer's "HTOKENIZER" is the same shape).  The neighbouring
//  0x6F4EAFE0 does the identical thing for "HLIGHT" - see lightobjectcreate.cpp.
//
//  Instruction for instruction the same body as
//  src/GameUI/cameracontrollercreate.cpp, and it loses the same two things:
//  the `mov edx, <tag>` the shipped tail hands TRefCnt::AddRef (which has no
//  such parameter - refcnt.cpp), and the cookie-XORed __CxxFrameHandler3
//  frame MSVC emits around the placement-new cleanup, which this repo's
//  fixed /GS- /EHs-c- cannot reach at all (docs/msvc-vc8-idioms.md, "An
//  `__except_handler4`-shaped frame this toolchain cannot reproduce").
//============================================================================
#include "refcnt.h"
#include "storm.h"

//  0x6F4EA730 - the fog object's constructor proper, `this` in ecx, `retn 0`.
TRefCnt* __fastcall ConstructFogObject(void* raw);

TRefCnt* __fastcall CreateFogObject()
{
    void* raw = SMemAlloc(0xD4, "HFOG", -2, 0);

    TRefCnt* self = 0;
    if (raw != 0)
        self = ConstructFogObject(raw);

    if (self == 0)
        return 0;

    return self->AddRef("HFOG");
}
