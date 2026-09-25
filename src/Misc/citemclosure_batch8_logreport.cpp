//============================================================================
//  0x6F78BF30 - reached from CItem's own vtable BFS (batch 8 of 8, see
//  docs/targets/CItem-callee-batch8.md) via sub_6F501790 and
//  FindModelRecord (0x6F501930, out of this batch's scope).  A `/GS-`
//  cookied frame (dword_6FAAE140 XOR `esp`) of the same unreproducible shape
//  docs/msvc-vc8-idioms.md already records for this fixed `/GS-` build (see
//  citemclosure_terrain_gates.cpp's EnsureTerrainSubsystemA/B) that formats
//  a "%s\n" report through an unnamed logger object's own vtable slot 4
//  after a call through GetIndirectField0xC_6F4C3680 (already EXACT
//  elsewhere in this tree) whose second/third arguments this closure never
//  establishes, and whose primary work - sub_6F78BD80, a ~0x100-byte stack
//  buffer builder - is a further callee out of scope again.  `retn 4`.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

struct SLogReportThunk
{
    int __thiscall ReportOrLog(int loggerOrNull);
};
ADDR_THUNK(int __thiscall SLogReportThunk::ReportOrLog(int), 0x6F78BF30)
