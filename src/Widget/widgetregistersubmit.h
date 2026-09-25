//============================================================================
//  0x6F39F2C0 - see widgetregistersubmit.cpp.
//
//  A thiscall member of the player-slot table, not a __fastcall free
//  function: it takes *five* stack arguments and cleans twenty bytes
//  (`retn 14h`).  Spelled __fastcall, `playerIndex` lands in edx, only four
//  arguments are pushed, and the callee cleans four bytes nobody pushed -
//  the FATAL ret-count class tools/abi_audit.py exists to catch.
//============================================================================
#ifndef WIDGETREGISTERSUBMIT_H
#define WIDGETREGISTERSUBMIT_H

class CWorldVec3;

class SPlayerTable
{
public:
    //  0x6F39F2C0.  `int`: CWidget::SubmitToPlayerTable
    //  (widgetregisterquery.cpp) tail-forwards whatever it leaves in eax,
    //  which is what lets that slot be declared int for CUnit's sake.
    int SubmitWidget(unsigned int playerIndex, CWorldVec3* pos, int flags,
                     int a, int b);

    //  0x6F3A38F0 (`retn 14h` - five stack arguments and `this` in ecx, the
    //  same shape SubmitWidget has).  CUnit's own override of slot 63
    //  submits the unit itself rather than a position.
    int SubmitUnit(unsigned int playerIndex, void* unit, int flags, int a, int b);

    char  m_reserved00[0x3E0];
    //  +0x3E0 - CUnit's slot 63 refuses to submit anything at all unless
    //  this is set.  Nothing here reads it as anything but "set or not".
    void* m_field3E0;
};

#endif
