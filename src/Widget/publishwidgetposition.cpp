//============================================================================
//  0x6F39F280 - SWorldObjectSingleton::PublishWidgetPosition: forward a
//  widget position to the player-table submit, unless the sub-object says
//  the submit is suppressed.
//
//  Was a `mov eax,<address> / jmp eax` redirect.  Sixteen instructions, and
//  the thing it tail-jumps to is SPlayerTableSubmit::SubmitMasked
//  (0x6F38B5C0, playertablesubmit.cpp) - reconstructed in this same pass -
//  so the whole function is now readable end to end.
//
//  It is an argument-rewriting tail call: the shipped code overwrites its
//  own first two incoming stack slots (`mov [esp+arg_4],edx` /
//  `mov [esp+arg_0],eax`) and jumps, so the callee runs on this frame with
//  the sub-object's +0x3C word where the position was and the position
//  where the flags were.  The last two arguments travel through untouched.
//  Both functions clean sixteen bytes, which is what makes the rewrite
//  legal.
//
//  `int` and not `void`: the tail jump forwards the callee's answer, and a
//  `void` declaration makes MSVC emit a call and a separate `ret` instead.
//  Its one caller (widget_slot64.cpp) still ignores the value.
//
//  Own translation unit: CWidget::PublishPosition reaches it with a real
//  call and it tail-jumps to the submit.
//============================================================================
#include "widget.h"

class CWorldVec3;

//  Reconstructed in playertablesubmit.cpp; declared here the same way
//  widgetregistersubmit.cpp declares it at its own call site.
struct SPlayerTableSubmit
{
    int SubmitMasked(unsigned int playerMask, CWorldVec3* pos, int resultMask,
                     int* out);
};

//  The sub-object at +0x34.  Two fields: bit 0 of the byte at +0x24 forces
//  the suppressed path, and the word at +0x3C is what the submit is given
//  in the position's place.
struct SWorldObjectSubmitState
{
    char           m_reserved00[0x24];
    unsigned char  m_suppress;      // +0x24
    char           m_reserved25[0x3C - 0x25];
    unsigned short m_submitKey;     // +0x3C
};

struct SWorldObjectSingleton
{
    int PublishWidgetPosition(CWorldVec3* pos, int flags, int resultMask,
                              int* out);

    char                     m_reserved00[0x34];
    SWorldObjectSubmitState* m_pState;   // +0x34
};

int SWorldObjectSingleton::PublishWidgetPosition(CWorldVec3* pos, int flags,
                                                 int resultMask, int* out)
{
    SWorldObjectSubmitState* state = m_pState;

    if ((state->m_suppress & 1) != 0)
        flags |= 1;

    //  `return 1`, and it is the same 1: the shipped body opens `mov eax,1`
    //  and uses that one register as the operand of both bit tests, as the
    //  value OR-ed into the flags, and as what is left in eax when the test
    //  skips the tail jump - exactly the shape SPlayerTable::SubmitWidget
    //  (widgetregistersubmit.cpp) already documents one function along.
    if ((flags & 1) != 0)
        return 1;

    return ((SPlayerTableSubmit*)this)->SubmitMasked(state->m_submitKey, pos,
                                                     resultMask, out);
}
