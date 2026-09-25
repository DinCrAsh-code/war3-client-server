//============================================================================
//  EvtSched.cpp - a thread pool built from Event objects and a
//  TSGrowableArray<HANDLE>.  The module name is not a guess: the root
//  function's own construction of an SMemAlloc call site debug tag is
//  literally the string ".\\EvtSched.cpp" (see sub_6F62E5B0's dump, the
//  `aEvtschedCpp` operand right before the SMemAlloc(4, tag, 0x337, 0) call
//  that allocates one HANDLE-array element on first use).
//
//  Address neighbourhood: 0x6F62D1B0-0x6F62E87D (this file's own functions)
//  plus the thread/wait helpers living inside the same object as Event
//  (0x6F6D84B0/0x6F6D8540, event.cpp's neighbourhood) and one CRT-adjacent
//  processor-count helper (0x6F6CDCB0, its own TU below).
//============================================================================
#ifndef EVTSCHED_H
#define EVTSCHED_H

#include "storm.h"
#include "event.h"
#include <windows.h>

//  sub_6F62D1B0 / sub_6F62D230: the same TSGrowableArray<T>::ComputeChunk/
//  SetAlloc already reconstructed generically in storm.h/tsarray.inl,
//  instantiated here over T=HANDLE (a plain 4-byte pointer-sized element,
//  which is why SetAlloc's per-element ctor/dtor loops disappear and the
//  disassembly shows a flat dword copy instead - trivial T, not a
//  different function).  See growablehandlearray.cpp.

//  sub_6F62E000 - TSGrowableArray<HANDLE>::SetCount's own growth arithmetic,
//  inlined and specialised to "grow by exactly one", the same shape
//  agentregistry.cpp's AgentSlotArrayAppend already documents for a
//  non-trivial element type. See growablehandlearray.cpp.
HANDLE* __fastcall HandleArrayAppend(TSGrowableArray<HANDLE>* array);

//  0x6F6CDCB0 - GetSystemInfo()->dwNumberOfProcessors, defaulted to 1 if the
//  OS ever reports 0.  Its own TU: getprocessorcount.cpp.
unsigned int GetProcessorCount();

//  0x6F6D84B0 - wait on up to 64 Events at once.  Each array slot is an
//  `Event*`; a null slot or a not-yet-created Event (a null m_handle) is
//  skipped rather than passed to WaitForMultipleObjects.  Lives in the same
//  object as Event itself (0x6F6D8410-0x6F6D89D1 is one contiguous run), so
//  it belongs conceptually to event.cpp's module even though it is not an
//  Event member - own TU: waitforevents.cpp.
DWORD __fastcall WaitForEvents(unsigned int count, Event* const* events,
                                BOOL bWaitAll, DWORD dwMilliseconds);

//  0x6F6D8540 - _beginthreadex wrapper.  `outHandle` is where the new
//  thread's handle is stored (through whatever pointer the caller passes -
//  in this dump's one call site, that pointer is an Event* reinterpreted as
//  a HANDLE* because Event::m_handle is that class's first and only member,
//  so `(HANDLE*)event` and `&event->m_handle` are the same address; the
//  reconstruction below takes the pointer as `outHandle` directly rather
//  than assuming the Event connection, since nothing in this function's own
//  body depends on it being an Event). The second stack parameter is pushed
//  by the one caller in this tree but never read here, the same
//  documented-and-left-alone shape as CMemBlock::Init's own unread pair.
//  Own TU: launchworkerthread.cpp.
BOOL __fastcall LaunchWorkerThread(void* startAddress, void* argList,
                                    HANDLE* outHandle, void* unused);

//  0x6F62CF50 - the other dump's own root, reached here only as the
//  StartAddress LaunchWorkerThread hands to _beginthreadex.  Its own call
//  tree (36 functions) is reconstructed separately; only the address and
//  _beginthreadex-compatible signature are needed to compile this call.
unsigned int __stdcall EvtWorkerThreadMain(void* arg);

//  Globals this module's root (sub_6F62E5B0) reads and writes.  Contiguous
//  layout of g_evtSchedThreads' four fields (0x6FACEB80/84/88/8C) confirmed
//  by the root zeroing three of them (m_alloc/m_count/m_data) in one run of
//  consecutive stores, and by WaitForEvents's own two register loads
//  (m_count into ecx, m_data into edx) matching that call's own argument
//  order exactly.
extern unsigned int          g_evtSchedActive;       // dword_6FACEB58
extern unsigned int          g_evtSchedThreadCount;   // dword_6FACEB64 - 0 skips spawning
extern TSGrowableArray<HANDLE> g_evtSchedThreads;     // dword_6FACEB80 (m_alloc)/84(m_count)/88(m_data)/8C(m_chunk)
extern void*                 g_evtSchedUnknown5C;     // dword_6FACEB5C - freed with a bare "delete[]" tag, never otherwise read in this tree
extern void*                 g_evtSchedCritSecBlock;  // dword_6FACEB60 - a count-prefixed CRITICAL_SECTION[] block ([-4]=count)

#endif
