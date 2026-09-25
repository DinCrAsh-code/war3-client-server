//============================================================================
//  0x6F012850 - look up the ground-offset record for a world (x, y) point in
//  the large Storm singleton's table, defaulting to (0, 0, 0) when the point
//  falls outside the table or the cell it lands in carries no record.
//
//  Fetches SStormSingletonA::QueryCellRecordIndex(x, y) (0x6F747450,
//  stormsingletonaquery.cpp) and, if that comes back other than -1, uses the
//  returned value as a *second*, direct index into the same table's m_data
//  array (not the (x, y) pair again) to copy a 3-float payload at that
//  record's +0x10 into the caller's output.  What the returned index and its
//  3-float payload actually represent (a per-cell doodad/placement record's
//  own position or normal, most likely, given where this feeds - see
//  JASS_SetItemPosition_callee1.cpp/docs/targets/
//  JASS_SetItemPosition_callee1.md) is not established from this call tree
//  alone.
//============================================================================
#include "fvec3.h"
#include "stormsingletona.h"

SStormSingletonA* __fastcall GetStormSingletonA();

//  __fastcall, not a plain free function: the shipped code takes the output
//  vector in `ecx` (`mov esi, ecx`) and leaves both floats on the stack, then
//  cleans them itself (`retn 8`).  MSVC never register-allocates a float
//  argument, so __fastcall puts the pointer in ecx and both floats on the
//  stack - exactly the shipped shape.  Spelling it __cdecl instead builds a
//  `retn 0` that cleans nothing, and hooking *that* over 0x6F012850 leaks
//  eight bytes of the real caller's frame on every single call.
void __fastcall GetItemGroundOffset(FVec3* out, float worldX, float worldY)
{
    //  `out` is deliberately *not* cleared here.  The shipped code leaves it
    //  untouched on the -1 path and jumps straight to its epilogue; its one
    //  caller (BuildItemGroundOrientation) is what zeroes the vector, before
    //  the call.  Clearing it here as well is three stores the shipped
    //  function does not make - and it is a real behavioural difference, not
    //  a cosmetic one, for any caller that relies on the value surviving.
    FVec3 point;
    point.m_x = worldX;
    point.m_y = worldY;
    point.m_z = 0.0f;

    SStormSingletonA* singleton = GetStormSingletonA();
    int recordIndex = singleton->QueryCellRecordIndex(&point);
    if (recordIndex == -1)
        return;

    singleton = GetStormSingletonA();
    const unsigned char* record = singleton->m_data + recordIndex * 0x1C + 0x10;
    out->m_x = *(const float*)(record + 0);
    out->m_y = *(const float*)(record + 4);
    out->m_z = *(const float*)(record + 8);
}
