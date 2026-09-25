//============================================================================
//  sub_6F66B270 / sub_6F66B2F0 / sub_6F66B370 - three byte-for-byte
//  identical functions 0x80 bytes apart (0x6F66B270, 0x6F66B2F0,
//  0x6F66B370), each reached from a different owner in NetProvider's
//  worker-thread chain. Written once as a template on a tag type and
//  explicit-instantiated three times, the same way this codebase already
//  handles a genuinely-unknown type parameter with a numeric tag
//  (TSExplicitList<CFootprintTypeData, -572662307> in footprinttypelist.cpp)
//  - the real distinguishing type per call site is unrecoverable from this
//  call tree (nothing here reads a type-specific field), so the tag is just
//  this instantiation's own target address.
//
//  Layout, read from the offsets actually touched (nothing else is ever
//  read or written, so nothing else is named):
//    +0x00  m_data       - base of a combined block: 12-byte item slots
//                           starting at +0, and a parallel array of 12-byte-
//                           strided ConditionVariable-shaped objects
//                           starting at +0x20 of that same base.
//    +0x04  m_readIndex  - signed slot index, negative before the first
//                           item has ever been produced.
//    +0x08  m_cached     - an item pointer already resolved by a previous
//                           call (skips the whole scan below when set).
//
//  Each item slot's only touched field is its own +4 (a payload pointer);
//  the found item's own +8 field is read back and clamped to >=0 before
//  being cached in m_cached, and the found item pointer itself is the
//  return value. Control flow follows the shipped block layout with goto
//  rather than a "cleaned up" loop - CondVar::Wait is only called while
//  m_readIndex stays <=0 after the increment (a bounded number of wait
//  cycles), and once it passes into positive territory the function stops
//  scanning and returns whatever m_cached already held (null if nothing was
//  ever found) - unusual, but that is what the shipped block order says,
//  reproduced literally rather than "fixed" into a plausible-looking loop.
//
//  ConditionVariable::Wait itself (sub_6F6D9820, this instantiation's own
//  callee) is a wait/contention-tracked primitive with deadlock detection -
//  declared here (matching SignalOne's already-established class) but not
//  reconstructed this session, the same call as this codebase's earlier
//  session made for the rest of that lock family.
//============================================================================

struct ConditionVariable
{
    void SignalOne(unsigned int value);
    void Wait(unsigned int value);
};

template <unsigned int Tag>
struct WaitQueueRing
{
    void*  m_data;
    int    m_readIndex;
    void*  m_cached;

    void* Pop(unsigned int value);
};

template <unsigned int Tag>
void* WaitQueueRing<Tag>::Pop(unsigned int value)
{
    void* found = m_cached;
    int item;
    if (found)
        goto found_item;

loop_top:
    if (m_readIndex >= 0)
    {
        char* condvar = (char*)m_data + 0x20 + m_readIndex * 0xC;
        ((ConditionVariable*)condvar)->SignalOne(value);
    }

    ++m_readIndex;
    if (m_readIndex >= 1)
    {
        if (!found)
            return 0;
        goto found_item;
    }

    {
        char* condvar = (char*)m_data + 0x20 + m_readIndex * 0xC;
        ((ConditionVariable*)condvar)->Wait(value);
    }

    item = *(int*)((char*)m_data + (m_readIndex + 4) * 0xC + 4);
    if (item > 0)
        goto have_item;
    found = 0;
    goto loop_top;

have_item:
    found = (void*)item;
    if (!found)
        goto loop_top;

found_item:
    {
        int tail = *(int*)((char*)found + 8);
        m_cached = (tail > 0) ? (void*)tail : 0;
    }
    return found;
}

template struct WaitQueueRing<0x6F66B270>;
template struct WaitQueueRing<0x6F66B2F0>;
template struct WaitQueueRing<0x6F66B370>;
