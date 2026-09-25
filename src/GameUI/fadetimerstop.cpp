//============================================================================
//  0x6F310690 - CFadeTimer::StopFading: leave whatever fade is in progress
//  and put the layer it drives back where the timer's own state says it
//  should sit.  `retn 4`.
//
//  CItem vtable-closure batch 2.  `m_field38` (+0x38) is a `CLayer*` here -
//  the receiver of the two calls is CLayer::PropagateEnabled (0x6F5FDEC0,
//  already named) - fading out (state 0) forces the layer's subtree fully
//  disabled (`force=0xFF`), fading in (state 1) propagates normally
//  (`force=0`), and an already-idle timer (state 2) skips both and the
//  reset below entirely.
//
//  `recordLast` (the one stack argument) additionally recycles a pooled
//  resource at +0x3C/+0x40 back through the resource's own vtable slot 3,
//  passing the address of a module-global free-list head
//  (off_6FA761E8/off_6FA761F0 - the same pair 0x6F31D300/0x6F31F830 read
//  and write, CDataRecycler-shaped, out of this batch).  The shipped body
//  reaches that call as a genuine tail-jmp - it overwrites its own
//  argument slot on the stack with the global's address and `jmp`s into
//  the resource's vtable slot, so the recycler's own `retn` returns
//  straight into *this function's caller*, never back here.  Nothing in
//  standard C++ spells a guaranteed tail call, so this reads as an
//  ordinary call-then-return instead: DIFFERS for that shape, not for the
//  logic - the recycled resource's own method still runs with the same
//  receiver and the same argument either way.
//
//  Own translation unit: ResetToIdle (fadetimerreset.cpp) is a real call.
//============================================================================
#include "fadetimer.h"
#include "frame.h"   // CLayer::PropagateEnabled

class CFadeTimer : public CObserver
{
public:
    void StopFading(int recordLast);
    void ResetToIdle();   // 0x6F30C510, fadetimerreset.cpp

    CEventTimer  m_channel;      // +0x0C
    float        m_from;         // +0x28
    float        m_to;           // +0x2C
    float        m_step;         // +0x30
    int          m_state;        // +0x34
    CLayer*      m_pLayer;       // +0x38
    void*        m_pooledResource;   // +0x3C
    int          m_pooledExtra;      // +0x40
};

//  off_6FA761E8/off_6FA761F0 - the CDataRecycler-shaped free-list head
//  0x6F31D300/0x6F31F830 also touch; not in this batch.
extern void* g_fadeTimerResourceFreeList;   // off_6FA761E8
extern int   g_lastRecycledFadeResourceExtra;  // dword_6FA761F8

void CFadeTimer::StopFading(int recordLast)
{
    if (m_state == 2)
        return;

    if (m_state == 0)
        m_pLayer->PropagateEnabled((unsigned char)0, 0xFF);
    else if (m_state == 1)
        m_pLayer->PropagateEnabled((unsigned char)0, 0);

    ResetToIdle();

    if (recordLast != 0 && m_pooledResource != 0)
    {
        g_lastRecycledFadeResourceExtra = m_pooledExtra;
        void* resource = m_pooledResource;
        m_pooledResource = 0;
        m_pooledExtra = 0;

        typedef void (__thiscall *RecycleFn)(void*, void**);
        ((RecycleFn)(*(void***)resource)[3])(resource,
                                             &g_fadeTimerResourceFreeList);
    }
}
