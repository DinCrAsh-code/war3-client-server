//============================================================================
//  0x6F001730 - CAgent::CAgent().  See agent.h for the layout this
//  establishes and why the embedded CObserver at +0x14 is raw storage
//  rather than a typed member.
//
//  Own translation unit: 0x6F001730 sits in the same low, unrelated-helper
//  address range as nullsub_45 (0x6F001700) and TRefCnt::~TRefCnt
//  (0x6F00C070, refcnt.h) rather than agent.cpp's own 0x6F4Cxxxx
//  cluster - address-neighbourhood evidence this shipped in a different
//  module, per CLAUDE.md's "one translation unit per original module".
//
//  Same SEH-frame mismatch as CObserver::~CObserver (observer.cpp) and
//  CWidget__Constructor (widget.cpp): a base-class subobject constructed
//  ahead of a call the compiler cannot prove won't throw (the second
//  CObserver::CObserver() call below) is enough on its own for MSVC to
//  wrap this in the cookie-XORed, frame-pointer-omitted __CxxFrameHandler3
//  frame the dump shows, with no explicit __try/__finally needed in the
//  source - see docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped
//  frame this toolchain cannot reproduce".  Not reproducible under this
//  build's fixed /GS- /EHs-c- for the same reason documented there; the
//  body past the frame furniture matches instruction for instruction.
//============================================================================
#include "agent.h"
#include "storm.h"   // placement new

CAgent::CAgent()
{
    m_handle = (unsigned int)-1;
    m_typeTag = (unsigned int)-1;

    CObserver* selfObserver = new ((void*)&m_selfObserverStorage[0]) CObserver();
    selfObserver->m_refcount += 1;
}
