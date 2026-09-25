//============================================================================
//  0x6F26F230 - collapses a 64-way "unit class" id into one of four
//  movement-group codes.  The compiler's own range check (`lea eax,[ecx-1];
//  cmp eax,63`) proves the switch really is labelled 1..64 - not just the
//  three ids that produce a non-zero result - because MSVC derives the
//  table's span from the lowest and highest case labels actually present,
//  and a switch with only cases 2/16/32 plus a default would check a span
//  of 0..30, not 0..63.  1 and 64 are real case labels here, just ones
//  that return the same thing `default` does.
//
//  Called from CUnit's still-thunked slot 107 (0x6F2A0E30,
//  docs/targets/CUnit__vtable.md) right before the movement sub-object is
//  told which movement type to use (sub_6F476250, not yet reconstructed).
//  What the four codes and the sixty-one collapsed ids actually mean is not
//  recoverable from this leaf alone; only the mechanical mapping is.
//============================================================================
int __fastcall UnitClassMovementCode_6F26F230(int classId)
{
    switch (classId)
    {
    case 1:  return 0;
    case 2:  return 6;
    case 16: return 4;
    case 32: return 2;
    case 64: return 0;
    default: return 0;
    }
}
