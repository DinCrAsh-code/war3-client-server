//============================================================================
//  0x6F2ACDE0 - CWidget vtable[0x27] (index 39): give this widget its
//  sprite and configure everything about it from one art descriptor.
//
//  Shared, unoverridden, by CWidget, CSelectable, CDestructable, CUnit,
//  CItem and CCaptainAI - all six vtables carry this same address in slot
//  39 - so it is the base implementation for every widget-shaped object in
//  the game, and the reason it can be is that everything type-specific
//  about it goes out through virtual setters rather than being decided
//  here.  Slots 48-51, 73, 80 and 81 are `nullsub` on CWidget and CItem and
//  real only on CUnit (user_knowledge.json), so on an item this function's
//  whole second half is seven calls that do nothing.
//
//  Its own translation unit, not widget.cpp's, even though 0x6F2ACDE0 is in
//  the same address neighbourhood as MoveTo (0x6F2AC220), MoveForward
//  (0x6F2ABFB0) and the constructor (0x6F2ACAE0): it calls
//  CMatrix3::ToOrientationMatrix (0x6F2ABDC0), which is in that same
//  neighbourhood too, and the two would be inlined together.  Same
//  trade-off widget.h already documents once.
//
//  What it does, in order:
//
//   1. record the footprint type;
//   2. MoveTo the descriptor's position, with the descriptor's facing and
//      the caller's two flags - through vtable[0xD8], not as a direct call,
//      so a CUnit that overrides it gets its own;
//   3. build and configure the sprite (CreateSpriteFromDescriptor);
//   4. resolve this widget's handle-bearing sub-object and tell it the new
//      facing, asking it to republish;
//   5. clear all sixteen low state bits on the fresh sprite;
//   6. ask that same sub-object what its facing actually ended up being,
//      turn it into a rotation about the Z axis, and hand the sprite the
//      resulting 3x3 matrix;
//   7. push seven descriptor fields through the type-specific setters;
//   8. reset the two sentinel fields and copy the last two flags across.
//
//  Step 6 is where the (0, 0, 1) axis comes from: the rotation is built
//  from a default-constructed (identity) CMatrix3, so the result is the
//  Z rotation alone.  g_slopeThreshold1 is the encoded CFloat 1.0 - see
//  cmatrix3_axisrot.cpp for why that misnamed global is the one used.
//============================================================================
#include "widget.h"
#include "sprite.h"
#include "cmatrix3.h"
#include "itemplacementdispatch.h"
#include "handlemoved.h"

//  CWidget's own vtable, by raw offset - the same idiom widget.cpp already
//  uses rather than inventing thirty-odd virtuals in front of these.
typedef void  (__thiscall *MoveToFn)(void*, const CFloat*, const CFloat*, int,
                                     int, int, int, int, int, const CFloat*,
                                     int, int);
typedef SHandleWithType* (__thiscall *GetHandleObjectFn)(void*);
typedef void  (__thiscall *SetByPtrFn)(void*, const void*);
typedef void  (__thiscall *SetByValueFn)(void*, int);

//  The handle-bearing sub-object's own vtable[0x1C] - write its facing into
//  *out.  Same slot widget.cpp reads facing through.
typedef void  (__thiscall *GetFacingFn)(void*, CFloat*);

extern const CFloat g_CFloatZero;        // dword_6FAAE470
extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4 - encoded 1.0

void CWidget::InitSprite(unsigned int footprintType,
                         const SWidgetArtDescriptor* desc, int forward, int a5)
{
    void* self = this;

    m_footprintType = footprintType;

    ((MoveToFn)(*(void***)self)[0xD8 / 4])(self, &desc->m_x, &desc->m_y,
                                           forward, 1, a5, 0, 0, 1,
                                           &desc->m_facing, -1, 0);

    CWar3Image::CreateSpriteFromDescriptor(desc);

    ((GetHandleObjectFn)(*(void***)self)[0xB8 / 4])(self)
        ->NotifyHandleObjectMoved(&desc->m_facing, 1);

    SetSpriteStateBits(m_pSprite, 0xFFFF, 0);

    //  The matrix is constructed first: the shipped code stores its three
    //  1.0 diagonal slots before it touches the axis at all, then comes back
    //  for the six zeroes.  Declaring the axis first reverses that.
    CMatrix3 orientation;
    CWorldVec3 axis(g_CFloatZero, g_CFloatZero, g_slopeThreshold1);

    CFloat facing(kCFloatNoInit);
    SHandleWithType* handleObject = ((GetHandleObjectFn)(*(void***)self)[0xB8 / 4])(self);
    ((GetFacingFn)(*(void***)handleObject)[0x1C / 4])(handleObject, &facing);

    orientation.RotateAxis(facing, axis, 1);
    SetItemPlacementOrientation(m_pSprite, orientation.ToOrientationMatrix());

    ((SetByPtrFn)(*(void***)self)[0xC0 / 4])(self, &desc->m_setting40);
    ((SetByPtrFn)(*(void***)self)[0xC4 / 4])(self, &desc->m_setting44);
    ((SetByValueFn)(*(void***)self)[0xC8 / 4])(self, desc->m_setting48);
    ((SetByValueFn)(*(void***)self)[0xCC / 4])(self, desc->m_setting4C);
    ((SetByPtrFn)(*(void***)self)[0x140 / 4])(self, &g_CFloatZero);
    ((SetByPtrFn)(*(void***)self)[0x144 / 4])(self, &desc->m_setting3C);
    ((SetByPtrFn)(*(void***)self)[0x124 / 4])(self, &desc->m_setting38);

    m_reserved38 = (unsigned int)-1;
    m_reserved3C = 0;
    m_reserved40 = desc->m_field50;

    if (desc->m_flag54 != 0)
        m_flags |= 0x10;
    else
        m_flags &= ~0x10;
}
