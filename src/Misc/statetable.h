//============================================================================
//  0x6F763F00's receiver: a state table with a 32-byte-per-row array at
//  +0x79C and three named presets in front of it.
//
//  The class itself is not established by this call tree - it stamps no
//  vtable, carries no allocation tag and its only reachable field is the
//  table - so it is named for that table and nothing more.  What *is*
//  established is every callee's calling convention, read off the shipped
//  `retn <n>` and the register each argument arrives in.
//============================================================================
#ifndef STATETABLE_H
#define STATETABLE_H

struct SStateTable
{
    //  0x6F763F00 - apply `value` to the slot `kind` names.  Negative kinds
    //  are the three presets; anything else indexes the table directly.
    void ApplyByKind(void* value, int kind);
    //  0x6F741720 - the indexed arm, a shipped function of its own that
    //  ApplyByKind tail-jumps into.
    void ApplyIndexed(void* value, int index);

    //  0x6F741700 (`retn 4`) and 0x6F763D50 (`retn 8`) - the other two arms.
    //  Below this batch; thunked in statetablethunks.cpp.
    void ApplyDefault(void* value);
    void ApplyPreset(void* value, int preset);

    int   m_00;                     // +0x00 - the presets are only taken
                                    //         when this is non-zero
    char  m_reserved04[0x79C - 4];
    char* m_pRows;                  // +0x79C - 32 bytes per row
};

//  0x6F7415D0 (`retn 8`) - hand one row the value.  Nothing in a register:
//  __stdcall, not the __fastcall a two-argument helper would usually be.
void __stdcall ApplyStateRow(void* row, void* value);

#endif
