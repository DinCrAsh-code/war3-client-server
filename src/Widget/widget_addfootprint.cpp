//============================================================================
//  0x6F2AD0C0 - CWidget's vtable slot 82 (+0x148), shared unchanged by
//  CSelectable and CItem: build this widget's pathing-grid registration and
//  put its footprint on the grid.  RemoveFootprint (slot 83,
//  widgetfootprintremove.cpp) is its counterpart, and ApplyWidgetFootprint
//  (slot 84, widgetfootprintapply.cpp) re-marks the cells a registration
//  this function already built covers.
//
//  Three things it does that the smaller footprint slots do not:
//
//   * it sets three pathing flags on the handle sub-object first, and does
//     that even when the widget has no footprint shape at all - which is
//     why the shape's null check does not skip them;
//   * it *allocates* the registration (0x18 bytes through Storm, tagged
//     `.\\CWidget.cpp` at line 0x2D1 or 0x2D6 depending on which of the two
//     mask sets the shape asks for) and only builds it once, leaving an
//     existing one alone;
//   * it hands the finished bounding box to the registration itself before
//     the pathing grid ever sees it, and that half is unconditional - only
//     the grid invalidation is gated on the grid being up.
//
//  The two mask sets are {0xC2, 0x10, 8, 4} and {0xC2, 0x10, 8}: the same
//  three leading values with a fourth appended, chosen by bit 0 of the
//  shape's +0x64.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  placement-construct-into-a-Storm-allocation in this family
//  (docs/msvc-vc8-idioms.md) - the constructor call the compiler cannot
//  prove will not throw is enough on its own, and the shipped code carries
//  two unwind trampolines for it.
//
//  Own translation unit: fourteen real calls out of it.
//============================================================================
#include "widget.h"
#include "widgetfootprint.h"
#include "widgetpathrefadjust.h"
#include "quadrant.h"
#include "game.h"
#include "storm.h"   // placement new

typedef CPathRef* (__thiscall *GetHandleObjectFn)(void*);
typedef void (__thiscall *GetFacingFn)(void*, CFloat*);
//  slot 79 (+0x13C) - "does this object have a real height".
typedef int (__thiscall *Slot0x13CFn)(void* self);
//  slot 91 (+0x16C) - CItem::RefreshPlacementRadius (item_radius.cpp) and CWidget's
//  own nullsub.
typedef void (__thiscall *Slot0x16CFn)(void* self);

//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);

//  0x6F47C350 - widget_load.cpp declares and redirects SPathRefArray's own
//  constructor; re-declared here, never re-thunked.
struct SPathRefArrayCtor
{
    void Construct(CWidget* owner, void* pos, int maskCount, const int* masks,
                   int flags);
};

//  0x6F2ABEE0 - the per-cell callback this registration is pinned with.  Its
//  address is taken and never called from here, and it is a *different*
//  callback from ApplyWidgetFootprint's 0x6F2AB0F0.  Already thunked as
//  FootprintMarkCellRegion (unreconstructed_thunks.cpp, shared with
//  CWidget's slot 85); re-declared here, never re-thunked - a second naked
//  body at the same address is the two-symbols-for-one-function defect
//  docs/notes/verifier-gate-link-failures.md warns about.
void __fastcall FootprintMarkCellRegion();

//  0x6F47BB30 - hand the finished bounding box to the registration.  A
//  thiscall member with one stack argument; 145 instructions of pathing-side
//  bookkeeping in the 0x6F47Bxxx module, out of scope and redirected.
struct SPathRefArrayBox
{
    void SetBounds(const void* box);
};

__declspec(naked) void SPathRefArrayBox::SetBounds(const void*)
{
    __asm
    {
        mov     eax, 06F47BB30h
        jmp     eax
    }
}

static const char kWidgetCpp[] = ".\\CWidget.cpp";

void CWidget::AddFootprint(int blocking, int visible, int useAlternate)
{
    void* vself = this;

    CFootprintShape* shape = GetOwnFootprintShape(useAlternate);

    //  Nested, not `&&`: a widget with no shape still gets its three
    //  pathing flags set unless slot 79 says it has a real height, which is
    //  the shipped fall-through from the null test into the flag block.
    if (shape == 0)
    {
        if (((Slot0x13CFn)(*(void***)vself)[0x13C / 4])(vself))
            return;
    }

    //  The three flags take the *inverse* of the caller's own two arguments
    //  (`setz`), so "blocking" here means "do not clear the blocking flag".
    SPathRefHandle* handle =
        (SPathRefHandle*)((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself);
    handle->SetPathFlagVisible(visible == 0);
    handle->SetPathFlagWalkable(0);
    handle->SetPathFlagBlocking(blocking == 0);

    if (shape != 0)
    {
        CWorldVec3 world =
            ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself)
                ->SmartPtrToWorldVec3();

        SFootprintPos pos;
        pos.m_x = world.m_x;
        pos.m_y = world.m_y;

        //  The handle sub-object asked for a second time rather than kept
        //  across the first call - the same shipped shape
        //  ApplyWidgetFootprint documents.
        CPathRef* ref = ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself);
        CFloat facing(kCFloatNoInit);
        ((GetFacingFn)(*(void***)ref)[0x1C / 4])(ref, &facing);

        int direction = ClassifyQuadrant(facing);
        shape->ApplyDirection(&pos.m_x, &pos.m_y, direction);

        if (m_pathGridRegistration == 0)
        {
            void* built;
            if ((shape->m_kindFlags & 1) != 0)
            {
                int masks[4];
                masks[0] = 0xC2;
                masks[1] = 0x10;
                masks[2] = 8;
                masks[3] = 4;
                void* raw = SMemAlloc(0x18, kWidgetCpp, 0x2D1, 0);
                if (raw != 0)
                {
                    ((SPathRefArrayCtor*)raw)->Construct(this, &pos, 4, masks, 0);
                    built = raw;
                }
                else
                    built = 0;
            }
            else
            {
                int masks[3];
                masks[0] = 0xC2;
                masks[1] = 0x10;
                masks[2] = 8;
                void* raw = SMemAlloc(0x18, kWidgetCpp, 0x2D6, 0);
                if (raw != 0)
                {
                    ((SPathRefArrayCtor*)raw)->Construct(this, &pos, 3, masks, 0);
                    built = raw;
                }
                else
                    built = 0;
            }

            m_pathGridRegistration = (SPathRefArray*)built;

            shape->VisitCells(&pos, direction,
                                (void*)&FootprintMarkCellRegion, built);

            //  The box reuses the CWorldVec3 the position came out of - it
            //  is dead by here, and the shipped code writes the box back
            //  into the same three slots.
            shape->CellBox(&world, &pos, direction);
            ((SPathRefArrayBox*)m_pathGridRegistration)->SetBounds(&world);

            if (g_pathGridReady != 0)
                PathGridInvalidateBox(&world);
        }
    }

    if (blocking)
        ((Slot0x16CFn)(*(void***)vself)[0x16C / 4])(vself);
}
