//============================================================================
//  0x6F0418E0 / 0x6F477D30 / 0x6F477E20 - see agenttimer.h.
//
//  Own translation unit: this cluster's addresses (0x6F0418E0, 0x6F477Dxx/
//  0x6F477Exx) sit apart from every other file this dump touches, and
//  CAgentTimer is instantiated as a plain embedded member rather than
//  constructed by any of them.
//
//  CAgentTimer::CAgentTimer() carries the same unreproducible
//  __except_handler4-shaped SEH frame as every other constructor in this
//  family (docs/msvc-vc8-idioms.md) - a base class (TRefCnt, trivially, and
//  m_target's own CAgentPtr(CAgent*) constructor concretely) with cleanup
//  to run if a later step throws is enough on its own for MSVC to build it,
//  with no explicit __try/__finally in the source.
//============================================================================
#include "agenttimer.h"

CAgentTimer::CAgentTimer() : m_target(0)
{
    m_refcount = 0;
    m_queue = 0;
    m_flags = 0;
}

//  Written naked.  The C++ it replaces is
//
//      void* queue = m_queue;
//      if (queue != 0 && !(((unsigned char*)queue)[0x12] & 1))
//          *(unsigned int*)((char*)queue + 0x10) |= 0x10000;
//      m_queue = 0;
//      CAgent* target = m_target.m_ptr;          // the same test-decrement-
//      if (target != 0)                          //   call release CAgentPtr's
//      {                                         //   own private Release()
//          if (--target->m_refcount == 0)        //   uses, spelled out here
//              target->ReleaseSelf();            //   because that one is
//      }                                         //   private and this is a
//      m_target.m_ptr = 0;                       //   different class
//      m_flags &= ~0x6u;
//
//  and it produces these instructions with two stores rescheduled: VC8
//  hoists the load of m_target.m_ptr above the `m_queue = 0` store, and
//  sinks the `m_target.m_ptr = 0` store below the flag mask so that the
//  null-target path runs it too (writing a zero over a zero).  Both are
//  choices no source ordering reaches - swapping the statements moves the
//  loads with them - and neither is observable, but they cost three
//  instructions, so the twenty-one are transcribed.
//
//  Naked also stops ~CAgentTimer inlining this, which is what the shipped
//  destructor does too: 0x6F477E20 reaches it with a real `call`.
__declspec(naked) void CAgentTimer::Cancel()
{
    __asm
    {
        push    esi
        mov     esi, ecx
        mov     eax, [esi+0Ch]                      // m_queue
        test    eax, eax
        jz      short queueCleared
        test    byte ptr [eax+12h], 1
        jnz     short queueCleared
        or      dword ptr [eax+10h], 10000h
    queueCleared:
        mov     dword ptr [esi+0Ch], 0
        mov     ecx, [esi+8]                        // m_target.m_ptr
        test    ecx, ecx
        jz      short done
        add     dword ptr [ecx+4], 0FFFFFFFFh       // --m_refcount
        jnz     short targetKept
        mov     eax, [ecx]                          // ReleaseSelf, slot 0
        mov     edx, [eax]
        call    edx
    targetKept:
        mov     dword ptr [esi+8], 0
    done:
        and     dword ptr [esi+10h], 0FFFFFFF9h     // m_flags &= ~6
        pop     esi
        retn
    }
}

CAgentTimer::~CAgentTimer()
{
    Cancel();
}
