//============================================================================
//  0x6F2A0830 (`retn 0`, no stack args) - CUnit batch-19, slot 107's own
//  depth<=2 closure.  Gathers four small per-unit classifications and
//  hands them to the movement sub-object's own four-argument setter
//  (0x6F474CF0, at &m_movement) in one call:
//
//    - CUnitTestNoWorkerFlagsSet(this) (0x6F29E8A0, cunit_agent6_naked.cpp,
//      already reconstructed) - a 0/1 flag;
//    - UTQ_6F29CA60(m_footprintType) (Unit/unit_typeinstancepopulate.cpp,
//      already reconstructed as a thunk) - only its low byte is stored;
//    - GetColorPlayerOrGreyNibble() (0x6F29E8F0, unit_colornibbles.cpp,
//      already reconstructed) - only its low word is kept (movzx ax);
//    - UnitTypeField230_6F29CAC0(m_footprintType)
//      (Unit/cunit_agent1_unittypefield230.cpp, already reconstructed) -
//      again only its low byte is stored.
//
//  The shipped code stores each of the second and fourth results with a
//  one-byte `mov`, then reads the same stack slot back with a *four*-byte
//  `mov` for the argument push - not a `movzx`. Whether that is a real
//  compiler-provable invariant (the two callees never set the high three
//  bytes of eax, so the stale value already sitting there from an earlier
//  instruction happens to be zero) or an accepted piece of shipped
//  imprecision, nothing in this call tree settles; this reconstruction
//  writes the C++-correct widening (`(unsigned char)` cast, which this
//  compiler turns into an explicit `movzx`) rather than reproduce a
//  memory-content coincidence, so the two byte-truncating steps are the
//  known, called-out gap between this body and the dump.
//
//  0x6F474CF0 (`retn 10h`) is not in this agent's own worklist slice: its
//  own body resolves a second handle+typeTag pair to another unit and
//  finishes with a call this tree does not reach - declared and
//  self-redirect-thunked, same "somebody else's reconstruction" treatment
//  used throughout this session's other out-of-slice callees.
//============================================================================
#include "unit.h"

extern int __fastcall CUnitTestNoWorkerFlagsSet(void* unit);           // cunit_agent6_naked.cpp
//  unit_typeinstancepopulate.cpp declares every UTQ_<addr> inside its own
//  `extern "C"` block (unmangled __fastcall decoration) - matched here
//  exactly, since a plain C++ declaration would mangle to a different,
//  unresolved symbol (link_check.py caught exactly this).
extern "C" void* __fastcall UTQ_6F29CA60(unsigned int fourCC);
extern int __fastcall UnitTypeField230_6F29CAC0(unsigned int fourCC);  // cunit_agent1_unittypefield230.cpp

void CUnit::ApplyMovementClassificationParams()
{
    int noWorkerFlags = CUnitTestNoWorkerFlagsSet(this);
    unsigned int fourCC = m_footprintType;
    unsigned char secondaryField = (unsigned char)(unsigned int)UTQ_6F29CA60(fourCC);
    unsigned short colorNibble = (unsigned short)GetColorPlayerOrGreyNibble();
    unsigned char field230 = (unsigned char)UnitTypeField230_6F29CAC0(fourCC);

    m_movement.ApplyClassificationParams_6F474CF0(
        noWorkerFlags, secondaryField, colorNibble, field230);
}

__declspec(naked) void SUnitMovement::ApplyClassificationParams_6F474CF0(int, int, int, int)
{
    __asm { mov eax, 06F474CF0h }
    __asm { jmp eax }
}
