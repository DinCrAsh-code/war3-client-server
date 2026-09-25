//============================================================================
//  0x6F502BD0 / 0x6F507CE0 - two more probes on the same "model instance"
//  object spritesethidden.cpp's SModelInstanceNotify already stands for
//  (0x6F50D8C0, in that file).  Kept in their own translation unit rather
//  than folded into that one: both are real, non-trivial bodies (not the
//  two-line naked glue RefreshDrawState is), and address-neighbourhood-wise
//  they belong with each other, not with 0x6F4D31D0's own module.
//
//  Reached from spritesethidden.cpp's 0x6F4D3500/0x6F4D3510 (same module as
//  SpriteSetHidden) with real calls, in turn from
//  CUnit::RefreshSpriteVisibility's slot-61 override
//  (unit_refreshspritevisibility.cpp).  Declared here taking `void*` rather
//  than a named model type: spritesethidden.cpp's call sites only ever hand
//  over the raw +0x20 sprite field, and MSVC mangles the parameter type
//  into the symbol name, so both sides of that call have to spell the
//  parameter identically.
//
//  The struct here only names the fields these two functions touch, the
//  same "offsets are recoverable, the surrounding layout is not" choice
//  spritesethidden.cpp's own SSpriteHiddenFlag makes for the sprite side.
//============================================================================
struct SModelInstanceProbe
{
    char m_reserved00[0x94];
    unsigned char m_flags94;      // +0x94 - bit 0x10 gates every walk below
    char m_reserved95[0x22];
    int  m_attachCount;           // +0xB8
    void** m_attachSlots;         // +0xBC - array of SCountedOwner*
    char m_reservedC0[4];
    int  m_childCount;            // +0xC4
    char* m_childList;            // +0xC8 - array of 0xC-stride list heads
    char m_reservedCC[0x10];
    int  m_probeACount;           // +0xDC
    char* m_probeAArray;          // +0xE0 - array of 0x68-stride elements
    int  m_probeBCount;           // +0xE8
    void** m_probeBArray;         // +0xEC - array of pointers, dereferenced once
};

struct SCountedOwner;
extern void __fastcall SetSlotValueGuarded_6F7B4420(SCountedOwner*, int);

//  0x6F4F4A80 / 0x6F4F1EC0 - SModelProbeElementA::ApplyFlag_6F4F4A80 and
//  SModelProbeElementB::ApplyFlag_6F4F1EC0.  Declared (not defined) here -
//  see modelprobeelement.h and modelprobeelementa_applyflag.cpp /
//  modelprobeelementb_applyflag.cpp: each body lives in its own TU so
//  neither gets inlined into this file's own 0x6F507CE0 walk.
#include "modelprobeelement.h"

//----------------------------------------------------------------------------
//  0x6F502BD0 - push `flag` at every attachment slot in +0xB8/+0xBC, gated
//  by bit 0x10 of +0x94.  `edx` really does carry `flag` in (`mov ebp, edx`
//  before anything is read off the stack), so this is a plain
//  __fastcall(void*, int) free function, matching every other probe on this
//  object.
//----------------------------------------------------------------------------
void __fastcall SModelInstance_PropagateAttachmentFlag_6F502BD0(void* modelPtr, int flag)
{
    SModelInstanceProbe* model = (SModelInstanceProbe*)modelPtr;

    if ((model->m_flags94 & 0x10) != 0)
    {
        //  Count read into a local once, ahead of the loop: the shipped
        //  code loads +0xB8 a single time into its own loop-bound register
        //  rather than re-reading it through `model` every iteration.
        int count = model->m_attachCount;
        void** slots = model->m_attachSlots;
        for (int i = 0; i < count; ++i)
            SetSlotValueGuarded_6F7B4420((SCountedOwner*)slots[i], flag);
    }
}

//----------------------------------------------------------------------------
//  0x6F507CE0 - three separate probes over the same object, gated by the
//  same +0x94 bit 0x10 test as the function above:
//
//    * every 0x68-stride element of +0xDC/+0xE0 gets `flag` through
//      0x6F4F4A80;
//    * every pointer in the +0xE8/+0xEC array is dereferenced once and gets
//      `flag` through 0x6F4F1EC0;
//    * if `includeChildren` is non-zero, every 0xC-stride list head in
//      +0xC4/+0xC8 is walked (its own +0x8 field is the list head) and each
//      linked node (+0x4 next, +0x8 child model pointer) recurses through
//      this same function with `flag` unchanged and `includeChildren`
//      forced to 0 - one level of fan-out, never two.
//
//  `includeChildren` arrives on the stack (`retn 4`), not in a third
//  register: `__fastcall` only ever claims ecx/edx for the first two
//  parameters, so a third one is always a stack argument regardless of
//  type.  The shipped `test esi, esi` / `jle` (signed, not `jz`) is kept as
//  a signed `> 0` test rather than a null check on the list-walk locals -
//  it is what the dump has, whatever it really guards against.
//----------------------------------------------------------------------------
void __fastcall SModelInstance_PropagateFlagRecursive_6F507CE0(
    void* modelPtr, int flag, int includeChildren)
{
    SModelInstanceProbe* model = (SModelInstanceProbe*)modelPtr;

    if ((model->m_flags94 & 0x10) == 0)
        return;

    {
        SModelProbeElementA* p = (SModelProbeElementA*)model->m_probeAArray;
        for (int i = model->m_probeACount; i > 0; --i, p = (SModelProbeElementA*)((char*)p + 0x68))
            p->ApplyFlag_6F4F4A80(flag);
    }
    {
        void** p = model->m_probeBArray;
        for (int i = model->m_probeBCount; i > 0; --i, ++p)
            ((SModelProbeElementB*)*p)->ApplyFlag_6F4F1EC0(flag);
    }

    if (includeChildren != 0)
    {
        char* entry = model->m_childList + 8;
        for (int i = model->m_childCount; i > 0; --i, entry += 0xC)
        {
            for (int node = *(int*)entry; node > 0; node = *(int*)(node + 4))
            {
                SModelInstance_PropagateFlagRecursive_6F507CE0(
                    *(void**)(node + 8), flag, 0);
            }
        }
    }
}
