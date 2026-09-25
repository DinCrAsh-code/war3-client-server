//============================================================================
//  0x6F371390 - CInfoBar::CInfoBar: the bottom-left panel and everything in
//  it.
//
//  The class name is the shipped one (??_7CInfoBar@@6B@, one vtable, so a
//  CSimpleFrame and nothing else) and the module is ".../UI/CInfoBar.cpp".
//
//  What it builds, in the shipped order:
//
//    * five FDF frames by name - the unit, building, cargo, item and
//      destructable detail panels - each with its own context 0..4;
//    * a 0x134-byte CInfoPanelGroup (line 0x4D), which is that class's own
//      size to the byte;
//    * all six of them hidden through the +0x90 flag and slot 0x64, the
//      group last;
//    * a 0x148-byte CInventoryBar (line 0x56) - again its own size exactly;
//    * a 0x124-byte plain CSimpleFrame (line 0x58) as the inventory cover,
//      0.128 x 0.175, its point 8 at (0.6, 0);
//    * a 0xE8-byte CSimpleTexture on that cover (line 0x5C) filling it, with
//      the "ConsoleInventoryCoverTexture" skin and texture coordinates
//      (0.31640625, 0, 1, 1);
//    * a 0xC4-byte CSimpleFontString on the bar itself (line 0x63) with the
//      "MasterFont" skin at the [FontHeights] Inventory size, colour
//      0xFFFED312, a 0xEE000000 shadow at (0.001, -0.001), and the
//      "INVENTORY" game string.
//
//  Every float is the shipped constant read out of .rdata; the instruction
//  score cannot see one of them, so they are values this file has to get
//  right on its own.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with a base and six owned allocations to unwind, plus the two
//  /GS cookie checks its 0x80-byte game-string buffer earns it.
//
//  Own translation unit: thirty-two real calls out of it.
//============================================================================
#include "infopanel.h"
#include "gameui.h"
#include "framereg.h"
#include "storm.h"
#include "gamestrings.h"

//  0x6F009E30 - Config/configgates.cpp.
float __fastcall GetConfigFloatHW(const char* section, const char* key,
                                  int index);
//  off_6FA8C27C - "FontHeights".
extern const char* g_pFontHeightsSection;

//  Slot 0x64 (+0x19) - the frame's own show/refresh.  Nothing pushed.
typedef void (__thiscall *FrameSlot0x64Fn)(void* self);

static const char kInfoBarFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CInfoBar.cpp";

//  GameUI/inventorybar.cpp's class, re-declared here with only its
//  constructor: MSVC puts only the class *name* in a constructor's mangled
//  name, so the two declarations are the one symbol.  No members are needed -
//  placement new never uses the size, because Storm/storm.h's
//  `operator new(unsigned int, void*)` returns its argument and MSVC inlines
//  it away.
class CInventoryBar
{
public:
    CInventoryBar(void* parent);        // 0x6F3606D0
};

class CInfoBar : public CSimpleFrame
{
public:
    CInfoBar(void* parent);         // 0x6F371390, `retn 4`

    int               m_field124;   // +0x124
    int               m_field128;   // +0x128
    int               m_field12C;   // +0x12C
    //  +0x130 .. +0x148 - the five detail panels, the group between them at
    //  +0x13C, and the destructable panel last.  The shipped constructor
    //  fills them in the order 0x130, 0x134, 0x138, 0x140, 0x144, 0x13C.
    CSimpleFrame*     m_pUnitDetail;         // +0x130
    CSimpleFrame*     m_pBuildingDetail;     // +0x134
    CSimpleFrame*     m_pCargoDetail;        // +0x138
    CInfoPanelGroup*  m_pGroup;              // +0x13C
    CSimpleFrame*     m_pItemDetail;         // +0x140
    CSimpleFrame*     m_pDestructableDetail; // +0x144
    CInventoryBar*    m_pInventory;          // +0x148
    CSimpleFrame*     m_pInventoryCover;     // +0x14C
    CSimpleFontString* m_pInventoryLabel;    // +0x150
};

//  Hide one frame: clear its +0x90 and run slot 0x64.  Six identical
//  sequences in the shipped body, which is what an inlined helper is.
static __forceinline void HideFrame(CSimpleFrame* frame)
{
    FrameSlot0x64Fn show = (FrameSlot0x64Fn)(*(void***)frame)[0x64 / 4];
    frame->m_shown = 0;
    show(frame);
}

CInfoBar::CInfoBar(void* parent)
    : CSimpleFrame(parent)
{
    m_field124 = 1;
    m_field128 = 7;
    m_field12C = 0;

    SetPointTo(6, (CLayoutFrame*)parent, 6, 0.309375f, 0.0f, 1);

    SetWidth(0.188125f);
    SetHeight(0.1140625f);

    m_pUnitDetail =
        CreateSimpleFrameByName("SimpleInfoPanelUnitDetail", this, 0);
    m_pBuildingDetail =
        CreateSimpleFrameByName("SimpleInfoPanelBuildingDetail", this,
                                (void*)1);
    m_pCargoDetail =
        CreateSimpleFrameByName("SimpleInfoPanelCargoDetail", this, (void*)2);
    m_pItemDetail =
        CreateSimpleFrameByName("SimpleInfoPanelItemDetail", this, (void*)3);
    m_pDestructableDetail =
        CreateSimpleFrameByName("SimpleInfoPanelDestructableDetail", this,
                                (void*)4);

    void* group = SMemAlloc(0x134, kInfoBarFile, 0x4D, 0);
    m_pGroup = group ? new (group) CInfoPanelGroup(this) : 0;

    HideFrame(m_pUnitDetail);
    HideFrame(m_pBuildingDetail);
    HideFrame(m_pCargoDetail);
    HideFrame(m_pItemDetail);
    HideFrame(m_pDestructableDetail);
    HideFrame((CSimpleFrame*)m_pGroup);

    void* inventory = SMemAlloc(0x148, kInfoBarFile, 0x56, 0);
    m_pInventory = inventory ? new (inventory) CInventoryBar(this) : 0;

    void* cover = SMemAlloc(0x124, kInfoBarFile, 0x58, 0);
    m_pInventoryCover = cover ? new (cover) CSimpleFrame(this) : 0;

    m_pInventoryCover->SetWidth(0.128f);
    m_pInventoryCover->SetHeight(0.175f);
    m_pInventoryCover->SetPoint(8, 0.6f, 0.0f, 1);

    void* texture = SMemAlloc(0xE8, kInfoBarFile, 0x5C, 0);
    CSimpleTexture* cover2 =
        texture ? new (texture) CSimpleTexture(m_pInventoryCover, 2, 1) : 0;

    cover2->SetAllPoints((CLayoutFrame*)m_pInventoryCover, 1);
    cover2->SetTexturePath(ResolveSkinValue("ConsoleInventoryCoverTexture", 0),
                           0);

    float coords[4];
    coords[0] = 0.31640625f;
    coords[1] = 0.0f;
    coords[2] = 1.0f;
    coords[3] = 1.0f;
    cover2->SetTexCoords(coords);

    void* label = SMemAlloc(0xC4, kInfoBarFile, 0x63, 0);
    m_pInventoryLabel =
        label ? new (label) CSimpleFontString(this, 2, 0) : 0;

    float labelHeight =
        GetConfigFloatHW(g_pFontHeightsSection, "Inventory", 0);
    m_pInventoryLabel->SetFont(ResolveSkinValue("MasterFont", 0), labelHeight,
                               0);

    unsigned int colour = 0xFFFED312;
    m_pInventoryLabel->SetColor(&colour);

    m_pInventoryLabel->SetPointTo(0, this, 2, 0.015625f, 0.0165625f, 1);
    m_pInventoryLabel->SetHeight(0.01125f);
    m_pInventoryLabel->SetWidth(0.076875f);

    unsigned int shadowColour = 0xEE000000;
    unsigned int shadowOffset[2];
    *(float*)&shadowOffset[0] = 0.001f;
    *(float*)&shadowOffset[1] = -0.001f;
    m_pInventoryLabel->SetShadow(&shadowColour, shadowOffset);

    char text[0x80];
    GetGameString("INVENTORY", text, sizeof(text));
    m_pInventoryLabel->SetText(text);
}
