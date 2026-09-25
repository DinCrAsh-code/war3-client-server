//============================================================================
//  0x6F283470 - CUnit's vtable slot 53 (+0xD4), RefreshWidgetPlacement.
//
//  Two paths, picked by m_int1FC:
//
//    * the common one (m_int1FC != 0x10) just forwards to the base class'
//      own re-placement (CWidget::RefreshWidgetPlacement, 0x6F2ACF90,
//      already reconstructed in widgetrefreshplacement.cpp) and returns
//      whatever it left in eax - see unit.h's note on why this slot is
//      declared `int` rather than `void`;
//
//    * the 0x10 one asks the movement sub-object for the cell the unit's
//      collision-grid box currently sits over (or, if nothing is
//      registered yet, the point its CPathTrace has reached), looks up a
//      per-cell scalar for that cell in the Storm terrain singleton, and
//      turns it into a bucket index with the same "round down to a power
//      of two, then shift" shape WorldToGrid (game.h) already uses.
//
//  Own translation unit: sub_6F474460 (SUnitMovement::GetCellMidpoint,
//  declared in unit.h) is reached only from here; sub_6F00F600
//  (GetGridCellScalar) has its own TU, gridcellscalar.cpp.
//============================================================================
#include "unit.h"
#include "game.h"
#include "stormsingletona.h"
#include "spatialgrid.h"

//----------------------------------------------------------------------------
//  0x6F474460 - SUnitMovement::GetCellMidpoint.  `this` resolves through
//  LookupHandle to a CPathTrace; its collision-grid registration's box
//  gives the midpoint cell directly.  Each axis is (max+min)/2, the
//  compiler's own round-toward-zero division by two (`cdq`/`sub`/`sar`) -
//  written as plain `/2` here, not shifted by hand, so MSVC picks that
//  idiom itself.
//----------------------------------------------------------------------------
void SUnitMovement::GetCellMidpoint(int* out)
{
    CPathTrace* trace = (CPathTrace*)LookupHandle(m_handle, m_handleType);
    CGridRegistration* reg = trace->m_collisionGrid;

    int avgX = (reg->m_box.m_x1 + reg->m_box.m_x0) / 2;
    //  Through an explicit pointer taken after the X sum, not a reference:
    //  the shipped code computes X straight off `reg` and only then
    //  advances a pointer to `reg->m_box` (`add ecx, 1Ch`) for the Y sum -
    //  a `const SCellBox&` bound at the same point folds back to the same
    //  base-relative offsets instead of materialising the advance.
    const SCellBox* box = &reg->m_box;
    int avgY = (box->m_y1 + box->m_y0) / 2;
    out[0] = avgX;
    out[1] = avgY;

    if (avgX < 0 || avgY < 0)
    {
        const CFloat& travelled = trace->GetTravelDistance();
        CGridVec2 point = trace->PointAtDistance(travelled);
        out[0] = CFloatToInt(point.m_x);
        out[1] = CFloatToInt(point.m_y);
    }
}

//  0x6F00F600 - gridcellscalar.cpp.  Its own translation unit so /O2 cannot
//  inline it away - it is a real, separately addressed function in the
//  dump and the shipped code reaches it with a `call`.
CFloat* __fastcall GetGridCellScalar(CFloat* out, int rowCoord, int colCoord);

//  vtable[0xB8] - GetHandleObject, called through the vtable rather than
//  straight off `&m_movement`: the shipped code really does dispatch here
//  (`mov eax,[edx+0B8h]` / `call eax`), even though CUnit's own body at
//  that slot (unit_thunks.cpp) is nothing but `lea eax,[ecx+164h]` - it is
//  not inlined at this call site.
typedef void* (__thiscall *GetHandleObjectFn)(void*);

//  The `if` tests the fast condition (`m_int1FC != 0x10`) and returns the
//  base call's own result directly - CWidget::RefreshWidgetPlacement is
//  `int` too (see its own note in widget.h), so this is a real early
//  `return`, not a fallthrough: matches the shipped `cmp`/`jz`/`call`/`ret`
//  exactly, where a `void` base (needing a bare `CWidget::...(); return;`
//  fallthrough with nothing after the `if`) let the compiler fold the call
//  into a tail `jmp` instead once nothing followed it.
int CUnit::RefreshWidgetPlacement()
{
    if (m_int1FC != 0x10)
        return CWidget::RefreshWidgetPlacement();

    //  Zero-initialised through one register, not two immediate stores -
    //  the shipped code is `xor eax,eax` then two `mov`s of it, which an
    //  aggregate `= { 0, 0 }` does not reliably produce but assigning
    //  through a zeroed local does.  One expression with nothing naming
    //  the intermediate SUnitMovement* pointer: the shipped code pushes
    //  `cell`'s address *before* the vtable[0xB8] call that produces
    //  `this` for GetCellMidpoint, the same "push the return-adjacent
    //  buffer early, no local holds the pointer" shape
    //  widgetrefreshplacement.cpp's own note already documents.
    int cell[2];
    int zero = 0;
    cell[0] = zero;
    cell[1] = zero;
    ((SUnitMovement*)((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this))
        ->GetCellMidpoint(cell);

    //  `kCFloatNoInit`: a default-constructed CFloat zeroes itself, which
    //  is a store the shipped code does not have - `scalar` is filled in
    //  whole by GetGridCellScalar before anything reads it.
    CFloat scalar(kCFloatNoInit);

    //  CFloatToInt takes GetGridCellScalar's own *returned* pointer, not a
    //  fresh `&scalar` - the shipped code reuses eax (`mov ecx,eax`); a
    //  second `&scalar` makes the compiler re-derive the address instead
    //  (`lea ecx,[esp]`), since nothing here says the two are the same
    //  pointer.
    //
    //  The same "round down to a multiple of 128, then divide by 128"
    //  bucket shape as WorldToGrid, but on the raw int rather than on a
    //  CFloat's exponent bits - 0x20C0/0x1F81 do not spell out to a power
    //  of two on their own, so they are left as the literal constants the
    //  disassembly gives rather than reassociated into something that
    //  "reads" better and stops matching.  `-128`, not `0xFFFFFF80`: the
    //  hex spelling does not fit `int` and promotes the whole expression
    //  to `unsigned`, which turns the shipped `sar` (signed) into a `shr`
    //  (unsigned) on the final shift - same bit pattern, different
    //  instruction.
    return (((CFloatToInt(*GetGridCellScalar(&scalar, cell[0], cell[1])) + 0x20C0)
             & -128) - 0x1F81) >> 7;
}
