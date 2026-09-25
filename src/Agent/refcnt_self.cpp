//============================================================================
//  0x6F4C4030 - TRefCnt::Self(). Kept in its own translation unit: the
//  target dump still shows a real `call TRefCnt::Self` inside both
//  Release() and AddRefOrNull() (refcnt.cpp) rather than the trivial `mov
//  eax, ecx` folding straight in, which only happens if the shipped build
//  compiled this method separately from its callers - this codebase's own
//  /O2 /Ob2 will otherwise inline a one-line same-TU function on sight.
//  See refcnt.h for the class-level notes.
//============================================================================
#include "refcnt.h"

TRefCnt* TRefCnt::Self()
{
    return this;
}
