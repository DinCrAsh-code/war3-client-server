//============================================================================
//  0x6F26FAE0 - CUnit::CtorHelper_6F26FAE0().  See unit.h.
//
//  CUnit::CUnit()'s (unit_ctor.cpp) own tail-end field-init leaf: a real
//  out-of-line `call`, not inlined, sixteen instructions of plain field
//  stores with no control flow.  Own translation unit for the same reason
//  every other ctor-helper leaf in this class is split out - the shipped
//  build kept it a genuine call site rather than folding it into the
//  constructor.
//============================================================================
#include "unit.h"

extern const unsigned int g_unk6F932954;   // funcmap.py's DATA table -
                                            // the "none" handle sentinel
                                            // (unit_attachments.cpp etc.)

void CUnit::CtorHelper_6F26FAE0()
{
    m_int2D0        = (unsigned int)-1;
    m_flags280      = 0;
    m_notifyHandle25C = (unsigned int)-1;
    m_handle260     = (unsigned int)-1;
    //  +0x264/+0x268 - the two dwords of m_reserved264 that this call
    //  tree ever writes (unit.h).
    *(unsigned int*)((char*)this + 0x264) = 1;
    *(unsigned int*)((char*)this + 0x268) = 0;
    m_notifyGate26C = 0;
    m_handle274     = (unsigned int)-1;
    //  +0x300 - one dword inside m_reserved2E4 (unit.h).
    *(float*)((char*)this + 0x300) = 1.0f;
    m_handle278     = g_unk6F932954;
    m_animMode2D8   = 1;
}
