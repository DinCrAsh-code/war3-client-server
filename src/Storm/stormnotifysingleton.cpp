//============================================================================
//  0x6F00D990 - see stormnotifysingleton.h.
//============================================================================
#include "stormnotifysingleton.h"

struct SStormSingletonA;
extern SStormSingletonA* __fastcall GetStormSingletonA();

//  0x6F741AC0 - set or clear bit 2 of the indexed record at +0x2CC.  This
//  file used to carry a second naked `mov eax,<that address> / jmp eax`
//  body for it under a made-up class name, which is the
//  two-symbols-for-one-function defect
//  docs/notes/verifier-gate-link-failures.md warns about: the address was
//  *already* reconstructed as
//  SIndexedArrayFlags::SetOrClearBit2 (misc_leaves_batch3.cpp), and
//  funcmap.py has always mapped it to that name.  Declared here, called
//  there.
struct SIndexedArrayFlags
{
    void* __thiscall SetOrClearBit2(int index, int setFlag);
};

void __fastcall NotifyStormSingleton(int handle, int value)
{
    SIndexedArrayFlags* singleton = (SIndexedArrayFlags*)GetStormSingletonA();
    singleton->SetOrClearBit2(handle, value);
}
