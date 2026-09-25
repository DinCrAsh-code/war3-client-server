//============================================================================
//  0x6F3606D0 - CInventoryBar::CInventoryBar.
//
//  Six inventory slots in two columns of three.  The class name is the
//  shipped one (??_7CInventoryBar@@6B@, one vtable, so a CSimpleFrame and
//  nothing else) and the module is ".../UI/CInventoryBar.cpp".
//
//  The slot array at +0x128 is Storm's growable array over an eight-byte
//  element, resized once to six with the same chunk-rounding SetCount every
//  other array in this target uses - written out here rather than called,
//  because the shipped code has it inlined and there is no out-of-line
//  SetCount for this instantiation anywhere in the image.
//
//  Each slot's button is a 0x1B8-byte CCommandButton, 0.03125 square, icon
//  alpha 0.82, anchored point 0 to point 2 of the parent at
//
//      x = (i & 1) * 0.04 + 0.01875
//      y = -0.0021875 - (i >> 1) * 0.038125
//
//  - both computed as *doubles* and narrowed to float at the push, which is
//  what the `fild`/`fmul dbl`/`fadd dbl` sequences and their `jge` /
//  `fadd flt_6F875AB8` unsigned fix-ups say: the two indices are unsigned.
//
//  Each button is flagged 0x10, told to report event 0x30065 to the game UI
//  singleton, given the "DefaultButton" skin, and - when it carries both of
//  the font strings at +0x158 and +0x15C - has both of them set to 0xFFFFFF.
//  Only then does it go into the array, high word first: the shipped stream
//  writes `[data + i*8 + 4]` before `[data + i*8]`.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with a base, a sub-object and a loop that allocates.
//
//  Own translation unit: fourteen real calls out of it.
//============================================================================
#include "commandbutton.h"
#include "gameui.h"
#include "agenttypedslots.h"
#include "storm.h"
#include "frame.h"

//  0x6F3228E0 - GameUI/frameartpath.cpp.
void __fastcall ApplyArtPathUnlessModel(void* frame, const char* path);

//  0x6F603030 - Misc/sweep_field_setters.cpp, under the name that file gives
//  it.
struct SDirtyFlagOwner
{
    void __thiscall SetField140(int value);
    void __thiscall SetField124And128(int a, int b);
};

//  Slot 0x64 (+0x19) - the frame's own show/refresh.  Nothing pushed.
typedef void (__thiscall *FrameSlot0x64Fn)(void* self);

static const char kInventoryBarFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CInventoryBar.cpp";

//  One slot: whatever the low word is for, and the button that draws it.
struct SInventorySlot
{
    int             m_field00;      // +0x00
    CCommandButton* m_pButton;      // +0x04
};

struct SInventorySlotArray
{
    //  0x6F33FB10 / 0x6F33FB70 - both `retn 4`, both still redirects.
    unsigned int ComputeChunk(unsigned int count);
    void         SetAlloc(unsigned int alloc);

    unsigned int    m_alloc;    // +0x00
    unsigned int    m_count;    // +0x04
    SInventorySlot* m_data;     // +0x08
    unsigned int    m_chunk;    // +0x0C
};

class CInventoryBar : public CSimpleFrame
{
public:
    CInventoryBar(void* parent);            // 0x6F3606D0, `retn 4`

    SCheckedUnitSlot    m_unit;             // +0x124
    SInventorySlotArray m_slots;            // +0x128 .. +0x138
    char                m_reserved138[0x08];// +0x138
    int                 m_field140;         // +0x140
    int                 m_field144;         // +0x144
};

CInventoryBar::CInventoryBar(void* parent)
    : CSimpleFrame(parent),
      m_unit(0)
{
    m_slots.m_alloc = 0;
    m_slots.m_count = 0;
    m_slots.m_data  = 0;
    m_slots.m_chunk = 0;

    m_field140 = 1;
    m_field144 = 0;

    //  SetCount(6), inlined exactly as the shipped code has it.
    if (6 > m_slots.m_count && 6 > m_slots.m_alloc)
    {
        unsigned int chunk = m_slots.m_chunk;
        if (chunk == 0)
            chunk = m_slots.ComputeChunk(6);

        unsigned int alloc = 6;
        unsigned int over = 6 % chunk;
        if (over != 0)
            alloc = 6 + (chunk - over);

        m_slots.SetAlloc(alloc);
    }
    m_slots.m_count = 6;

    for (unsigned int i = 0; i < 6; i++)
    {
        void* memory = SMemAlloc(0x1B8, kInventoryBarFile, 0x45, 0);
        CCommandButton* button =
            memory ? new (memory) CCommandButton(this, 1) : 0;

        button->SetWidth(0.03125f);
        button->SetHeight(0.03125f);
        button->SetIconAlpha(0.82f);

        //  Both offsets are computed in double and narrowed at the push,
        //  which is what the shipped `fstp`/`fld`/`fstp` round trip through
        //  a stack slot is.
        float y = (float)(-0.0021875000093132257 -
                          (i >> 1) * 0.0381249999627471);
        float x = (float)((i & 1) * 0.0400000000372529 +
                          0.01875000074505806);

        button->SetPointTo(0, (CLayoutFrame*)parent, 2, x, y, 1);

        ((SDirtyFlagOwner*)button)->SetField140(0x10);
        ((SDirtyFlagOwner*)button)->SetField124And128(
            0x30065, (int)GetGameUI(1, 0));

        ApplyArtPathUnlessModel(button, ResolveSkinValue("DefaultButton", 0));

        CSimpleFontString* labelA =
            (CSimpleFontString*)*(void**)((char*)button + 0x15C);
        CSimpleFontString* labelB =
            (CSimpleFontString*)*(void**)((char*)button + 0x158);

        if (labelA != 0 && labelB != 0)
        {
            unsigned int white = 0xFFFFFF;
            labelA->SetColor(&white);

            white = 0xFFFFFF;
            labelB->SetColor(&white);
        }

        m_slots.m_data[i].m_pButton = button;
        m_slots.m_data[i].m_field00 = 0;
    }

    FrameSlot0x64Fn show = (FrameSlot0x64Fn)(*(void***)this)[0x64 / 4];
    m_shown = 0;
    show(this);
}
