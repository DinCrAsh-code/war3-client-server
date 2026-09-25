//============================================================================
//  0x6F2CC350 - build a CNetCommandUnitSelectionEvent (wire type 0xA001B,
//  netcommand_classes_multifield.h) from two fields of an unrelated source
//  object and hand it to a single-argument submit call, all under this
//  family's own unreproducible __except_handler4-shaped SEH frame.
//
//  The class itself is not a guess - it is already reconstructed
//  (netcommand_classes_multifield.h), and the local's field-by-field store
//  order visible in the dump (vftable, id/subIndex/ctx group, then the two
//  runtime values) matches CNetCommandBase's own constructor exactly. What
//  is not established from this call tree alone is which of the source
//  object's two read fields (+0x0C, +0x10) lands in which of
//  CNetCommandUnitSelectionEvent's own two trailing members - both are
//  plain `int`s copied straight through with no further use in this
//  function, so nothing here disambiguates the order; scored and left
//  DIFFERS rather than guessed at with false confidence.
//============================================================================
#include "netcommand_classes_multifield.h"

//  0x6F2CA010 - the submit call.  Below this agent-6 slice's own BFS cut
//  and itself a cunit_agent5_worklist.md worklist entry for a different
//  concurrent agent's slice, so it stays thunked here rather than named.
namespace {
extern "C" __declspec(naked) void __fastcall
Callee_6F2CA010(CNetCommandUnitSelectionEvent*, int)
{
    __asm { mov eax, 0x6F2CA010 }
    __asm { jmp eax }
}
}  // namespace

//  What this call site's `this` (ecx) actually is is not established by
//  this call tree - only that its own +0x0C and +0x10 are two more plain
//  ints, read and not otherwise touched.
struct SSelectionEventSource_6F2CC350
{
    char m_reserved00[0x0C];
    int  m_fieldC;    // +0x0C
    int  m_field10;   // +0x10
};

void __fastcall BuildAndDispatchNetCommandUnitSelectionEvent(
    SSelectionEventSource_6F2CC350* self, unsigned char sender)
{
    CNetCommandUnitSelectionEvent cmd(2, 0);
    cmd.m_field18 = sender;
    cmd.m_field20 = self->m_fieldC;
    cmd.m_field1C = self->m_field10;

    Callee_6F2CA010(&cmd, 0);
}
