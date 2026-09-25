//============================================================================
//  0x6F26ECD0 - non-virtual CUnit member, called from ClearPendingAndNotify
//  (slot 32, 0x6F284B40 / unit_slotthunks.cpp, still thunked - CUnit
//  batch-6) with `visible = 1`.
//
//  Caches the requested visibility in m_pendingVisible270 unconditionally,
//  then - only if m_handle274 is a real handle (not the -1 "none" sentinel
//  slot 106 leaves it at once released) - republishes it through the same
//  Storm-singleton bit toggle CSelectable::EnsureSelectionCircle already
//  uses (NotifyStormSingleton, 0x6F00D990): the notified value is 1 only
//  when m_notifyGate26C is itself already set *and* the caller asked for
//  visible, 0 on every other path (gate clear, handle missing, or asked to
//  hide) - two independent AND'd conditions collapsed to one boolean the
//  shipped code computes with a pair of `jz`s rather than `&&`, which is
//  why the store happens before either branch is taken.
//============================================================================
#include "unit.h"
#include "stormnotifysingleton.h"   // NotifyStormSingleton - 0x6F00D990

void CUnit::SetAttachment274Visible(int visible)
{
    unsigned int handle = m_handle274;
    m_pendingVisible270 = (unsigned int)visible;

    if ((int)handle != -1)
    {
        int notify = (m_notifyGate26C != 0 && visible != 0) ? 1 : 0;
        NotifyStormSingleton((int)handle, notify);
    }
}
