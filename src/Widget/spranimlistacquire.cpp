//============================================================================
//  0x6F4ED140 - hand back the shared SprAnimList for this key, building it
//  the first time and taking a reference every time.
//
//  The class name is the shipped allocation tag, ".\SprAnimList.cpp" line
//  0x58, and the size it asks Storm for is 0x2C.
//
//  What is not reproduced is the frame, and it is the same one
//  MakeCameraController (GameUI/cameracontrollercreate.cpp) already
//  records: the cookie-XORed, frame-pointer-omitted __CxxFrameHandler3
//  registration MSVC emits so that the placement-new cleanup runs if the
//  constructor throws, its two unwind-state stores and the trailing
//  per-function funclet.  Nothing at this repo's fixed /GS- /EHs-c-
//  reaches that shape (docs/msvc-vc8-idioms.md).
//
//  Own translation unit: the lookup, the construction and the insert are
//  all real calls.
//============================================================================
#include "sprresource.h"
#include "storm.h"

SSprAnimList* __fastcall AcquireSprAnimList(void* key, void* model,
                                            const char* path)
{
    SSprAnimList* found = g_sprAnimLists.Find(key);

    if (found == 0)
    {
        void* raw = SMemAlloc(0x2C, ".\\SprAnimList.cpp", 0x58, 0);

        //  "Null stays null" and not an early return, for the reason
        //  MakeCameraController's own comment gives: it is MSVC's
        //  placement-new shape, one `test eax,eax` in front of the
        //  constructor call with both paths joining after it.
        SSprAnimList* made = 0;
        if (raw != 0)
        {
            made = (SSprAnimList*)raw;
            made->Construct(model, path);
        }

        g_sprAnimLists.Insert(made, key);
        found = made;
    }

    found->m_refs++;
    return found;
}
