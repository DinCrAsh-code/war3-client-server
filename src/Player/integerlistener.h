#ifndef INTEGERLISTENER_H
#define INTEGERLISTENER_H

#include "refcnt.h"

//============================================================================
//  IntegerListener - a plain TRefCnt-derived object with two more reserved
//  ints, built by sub_6F473180 (playerwar3_ability5integerlistener.cpp) and
//  torn down through the ordinary TRefCnt::ReleaseSelf-shaped release
//  every other refcounted field in this file uses (CPlayerWar3::~CPlayerWar3,
//  playerwar3_dtor.cpp, `??_7IntegerListener@@6B@` in the dump's own
//  allocation is the only evidence for the name and for deriving from
//  TRefCnt - the shipped construction writes `[obj+4]=0`, matching
//  TRefCnt::m_refcount's own +4 slot, then `[obj+8]=[obj+0Ch]=-1` for two
//  fields this session's scope never reads back).  No new virtual override
//  is known or needed - the one call this session's scope makes through
//  its vtable (the destructor's release) is slot 0, TRefCnt::ReleaseSelf
//  itself.
//============================================================================
class IntegerListener : public TRefCnt
{
public:
    IntegerListener()
    {
        m_refcount = 0;
        m_reserved08 = -1;
        m_reserved0C = -1;
    }

    int m_reserved08;   // +0x08
    int m_reserved0C;   // +0x0C
};

//  0x6F480D50 - see integerlistenerbindthunk.cpp.  Out of this session's
//  depth-3 window (depth 4 from the slot-30 root).
struct IntegerListenerBindHelper
{
    void Bind(void* slot5Result, void* receiver, int six, void* owner,
              int minusOne, int zero);
};

#endif
