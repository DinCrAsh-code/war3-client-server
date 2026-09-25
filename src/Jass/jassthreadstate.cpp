//============================================================================
//  0x6F44B2F0 / 0x6F44B3A0 - the two writes into the running JASS thread
//  that the trigger natives make.  See jassthreadstate.h for where the
//  class name and the layout come from.
//
//  Own translation unit, and not the natives' one: 0x6F44Bxxx is Jass.cpp
//  and 0x6F3B2xxx is the natives' own module, and the natives reach both of
//  these with a real tail jump.
//============================================================================
#include "jassthreadstate.h"
#include "gamecontext.h"    // GetThreadLocalSlot - 0x6F4C34D0

//----------------------------------------------------------------------------
//  0x6F44B2F0 - `retn 0`, both arguments in registers.
//----------------------------------------------------------------------------
void __fastcall JassThreadSleep(const CFloat* seconds, int kind)
{
    JassThreadLocal* jass =
        (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);

    SJassThread* thread = jass->m_threads[jass->m_threadCount - 1];

    //  Three plain assignments in the shipped order.  One instruction of
    //  the sixteen is scheduled differently and no spelling moves it: this
    //  build stores the kind (already in a register) before the flag, where
    //  the shipped stream stores the flag, loads `*seconds`, pops edi and
    //  only then stores the kind.  Naming the seconds in a local between
    //  the two stores was measured and is worse - 14/16, because the value
    //  then travels in edi and the pop moves.
    thread->m_sleeping     = 1;
    thread->m_sleepKind    = kind;
    thread->m_sleepSeconds = *seconds;
}

//----------------------------------------------------------------------------
//  0x6F44B3A0 - the same reach into the running thread, one store.
//----------------------------------------------------------------------------
void __fastcall JassThreadSyncStart()
{
    JassThreadLocal* jass =
        (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);

    jass->m_threads[jass->m_threadCount - 1]->m_syncStart = 1;
}
