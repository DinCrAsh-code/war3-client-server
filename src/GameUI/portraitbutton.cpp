//============================================================================
//  0x6F36F8D0 - CPortraitButton::CPortraitButton.
//
//  The class name is the shipped one (??_7CPortraitButton@@6B@ at +0x00 and
//  its `_0` twin at +0x0B4) and the base is CButtonFrame: the constructor it
//  calls is 0x6F6220C0, and CControl's fields end at +0x1E4, which is exactly
//  where this class's own start.
//
//  Three CEventTimers, a '+w3u' checked slot, four fields, a size and an
//  anchor, one event listener on the parent, two more on the global event
//  host through its own slot 2, and then a light: created, given a 0.33
//  intensity on slot 2 and a full white on slots 0, 1 and 3, tilted by
//  (-1, 0.8, -1) on slot 4, handed to the frame and released.
//
//  Three spellings here are the shipped body's and not a reading of it:
//
//    * everything before the first call is a *member initialiser*, in
//      declaration order - three timer constructors, the slot's constructor
//      and three stores - which is the only shape that gets that order with
//      no placement-new null test;
//    * the anchor's `parent` goes through the null-checked +0xB4 conversion
//      (`cmp ebx, ebp` / `lea eax, [ebx+0B4h]` over an already-zeroed
//      register) because it is a CFrame* being converted, where `this` in the
//      same position never is;
//    * the tilt vector's third component is the *same* -1.0f as its first:
//      the shipped code keeps one `fld` live across the store of the second
//      and pops it into the third.
//
//  The two dispatches through the global event host's slot 2 are hand-written
//  and vtable_dispatch_audit.py cannot judge them - the receiver is not
//  `this`.  Their argument count is the shipped call sites' (0x6F36FA11 and
//  0x6F36FA25 push exactly three dwords each).
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with a base and four sub-objects to unwind.
//
//  Own translation unit: eighteen real calls out of it.
//============================================================================
#include "frame.h"
#include "agenttypedslots.h"
#include "controllerslot.h"
#include "refcnt.h"

//  0x6F30F2F0 - Misc/trivialaccessors_02_03.cpp: the global event host.
void* __fastcall GetGlobal_6F30F2F0();
//  0x6F4EAFE0 - Widget/lightobjectcreate.cpp.
TRefCnt* __fastcall CreateLightObject();

//  Slot 2 (+0x08) of the global event host - register one event id against a
//  key and a context.  Three dwords, which is what both call sites push.
typedef void (__thiscall *EventHostRegisterFn)(void* self, int key, int event,
                                               void* context);

class CPortraitButton : public CButtonFrame
{
public:
    CPortraitButton(void* parent);      // 0x6F36F8D0, `retn 4`

    CEventTimer      m_timerA;          // +0x1E4
    CEventTimer      m_timerB;          // +0x200
    CEventTimer      m_timerC;          // +0x21C
    SCheckedUnitSlot m_unit;            // +0x238
    int              m_field23C;        // +0x23C
    int              m_field240;        // +0x240
    int              m_field244;        // +0x244
    char             m_reserved248[0x14]; // +0x248
    float            m_float25C;        // +0x25C
    int              m_field260;        // +0x260
};

CPortraitButton::CPortraitButton(void* parent)
    : CButtonFrame((int)parent, 0, 0),
      m_timerA(),
      m_timerB(),
      m_timerC(),
      m_unit(0),
      m_field23C(0),
      m_float25C(0.25f),
      m_field260(0)
{
    AddPendingState(0x80);

    SetWidth(0.09f);
    SetHeight(0.0856f);
    SetPointTo(6, (CFrame*)parent, 6, 0.2075f, 0.030625f, 1);

    m_1AC = 0;

    SetEventListener(0x50064, parent, 1);

    m_flags |= m_flagMask | 5;

    void* host = GetGlobal_6F30F2F0();
    ((EventHostRegisterFn)(*(void***)host)[8 / 4])(host, 0xC0100, 0x50065, this);
    ((EventHostRegisterFn)(*(void***)host)[8 / 4])(host, 0xC0101, 0x50066, this);

    TRefCnt* light = CreateLightObject();

    SetControllerSlotFloat(light, 2, 0.33f);

    float white[3];
    white[0] = 1.0f;
    white[1] = 1.0f;
    white[2] = 1.0f;
    ResetControllerSlot(light, 0, white);

    SetControllerSlotFloat(light, 3, 1.0f);

    float white2[3];
    white2[0] = 1.0f;
    white2[1] = 1.0f;
    white2[2] = 1.0f;
    ResetControllerSlot(light, 1, white2);

    float tilt[3];
    tilt[0] = -1.0f;
    tilt[1] = 0.8f;
    tilt[2] = -1.0f;
    MergeControllerSlotVec3(light, 4, tilt, 0);

    AddLight(light, 0);
    light->Release();

    m_field240 = 0;
    m_field244 = 0;
}
