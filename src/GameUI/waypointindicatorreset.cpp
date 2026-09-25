//============================================================================
//  0x6F393270 - SWaypointIndicatorArray::ResetAll.
//
//  Put all 256 waypoint indicators back to the state SetCount had just built
//  them in: destroy every element through its own slot 0, wipe the whole
//  block, and construct every element again.  The world frame's constructor
//  calls it immediately after SetCount(0x100) (0x6F39AE16), which is the
//  only call site in this tree.
//
//  The element's destroy is a hand-written slot dispatch and
//  tools/vtable_dispatch_audit.py cannot judge it - the receiver is an array
//  element, not `this`.  What fixes its argument count is the shipped call
//  site at 0x6F3932B2: exactly one dword is pushed between the slot load and
//  the `call eax`, and a slot whose body cleaned any other number would
//  already have unbalanced the shipped stack.  There is no committed vtable
//  for SWaypointIndicator to check the slot's real target against.
//
//  Three details are the shipped code's and not decoration: the element
//  address is recomputed off `m_data` inside both loops (the shipped body
//  reloads +0x08 every iteration), the wipe is one memset of `m_count`
//  elements rather than a per-element clear, and the *second* loop keeps the
//  placement-new null test while the first does not - which is what says the
//  destroy walk is a plain loop and the rebuild is a construction.
//
//  Own translation unit: three real calls out of it, and its shipped module
//  is not SetCount's (0x6F393xxx against 0x6F399xxx).
//
//  Score capped by the __except_handler4 frame MSVC builds round a loop that
//  constructs objects - the prologue, the epilogue, the two `mov [esp+..],
//  <state>` stores inside the second loop and the appended funclet - which
//  this repo's fixed /GS- /EHs-c- does not emit, exactly as SetCount's own
//  entry records.
//============================================================================
#include "gameui.h"

extern "C" void* __cdecl memset(void* dst, int value, unsigned int count);
#pragma intrinsic(memset)

//  Slot 0 of a waypoint indicator - destroy, with the "free the storage"
//  flag.  See the note above for what fixes the argument count.
typedef void (__thiscall *WaypointIndicatorDestroyFn)(void* self, int flags);

//  0x6F38FA30 - one indicator's constructor, declared exactly as
//  GameUI/waypointindicatorarray.cpp declares it so the two call sites reach
//  one symbol.
void __fastcall ConstructWaypointIndicator(SWaypointIndicator* self);

//  The array header, declared here the way SetCount's own translation unit
//  declares it rather than moved to a header: both are the one class and one
//  set of mangled names.
struct SWaypointIndicatorArray
{
    //  0x6F393270 - this file.
    void ResetAll();

    unsigned int         m_alloc;   // +0x00
    unsigned int         m_count;   // +0x04
    SWaypointIndicator*  m_data;    // +0x08
};

void SWaypointIndicatorArray::ResetAll()
{
    unsigned int i;

    for (i = 0; i < m_count; i++)
    {
        SWaypointIndicator* element = &m_data[i];
        ((WaypointIndicatorDestroyFn)(*(void***)element)[0])(element, 0);
    }

    memset(m_data, 0, m_count * sizeof(SWaypointIndicator));

    for (i = 0; i < m_count; i++)
    {
        SWaypointIndicator* element = &m_data[i];
        if (element != 0)
            ConstructWaypointIndicator(element);
    }
}
