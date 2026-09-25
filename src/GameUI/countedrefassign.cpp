//============================================================================
//  0x6F029580 - SCountedRef::Assign: adopt another one-word counted
//  reference's referent.
//
//  It is the assignment twin of RefHolder_6F029560::Release
//  (trivialaccessors_08.cpp) thirty-two bytes earlier: the same one-word
//  holder, the same "decrement and dispatch slot 0 at zero" release, and the
//  same class - the repo just had the destructor before it had this.  It is
//  named for what it is rather than for its address because nothing this
//  session adds may carry one.
//
//  Two orderings are load-bearing and neither is tidy:
//
//   * the *incoming* reference is addref'd before the held one is released,
//     which is the standard self-assignment guard and is what the shipped
//     `mov eax,[edi]` ahead of `mov ecx,[esi]` is;
//   * the store at the end re-reads `other->m_ref` (`mov eax,[edi]` a second
//     time) instead of reusing the value it just addref'd.  Writing
//     `m_ref = incoming;` folds the two reads into one and loses an
//     instruction, so the second read is spelled out.
//
//  Own translation unit: CCameraWar3::PanTo calls it for real.
//============================================================================
#include "gameui.h"
#include "refcnt.h"

SCountedRef* SCountedRef::Assign(const SCountedRef* other)
{
    TRefCnt* incoming = other->m_ref;
    if (incoming != 0)
        incoming->m_refcount++;

    TRefCnt* held = m_ref;
    if (held != 0)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }

    //  Deliberately `other->m_ref` and not `incoming` - see the header.
    m_ref = other->m_ref;
    return this;
}
