//============================================================================
//  ConditionVariable - see threadwaitpool.cpp (Wait/NotifyOne's real bodies,
//  both hand-written with non-standard register conventions) and
//  gameaction0x26_notify.cpp (SignalOne, a thin naked wrapper around
//  NotifyOne). Declared here only for callers that just need to invoke it
//  through an embedded instance (ActionQueueTable::FindOrCreateRecord) -
//  matches every other file's own minimal per-TU redeclaration of this
//  class rather than pulling in a shared class layout nothing establishes.
//============================================================================
#ifndef CONDITIONVARIABLE_H
#define CONDITIONVARIABLE_H

class ConditionVariable
{
public:
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

#endif
